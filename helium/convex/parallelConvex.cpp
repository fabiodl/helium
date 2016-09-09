#include <helium/convex/parallelConvex.h>
#include <helium/core/tAbs.h>
#include <helium/math/interpolation.h>
namespace helium{
Segment::Segment(Time pstart,const RobotConfig& startPosture,Time end,const RobotConfig& endPosture,double maxSpeed):
  start(pstart),posture(startPosture),checkState(ONCHECK)
{
  reset(end,endPosture,maxSpeed);
  
}

Segment::Segment(Time pstart,const RobotConfig& startPosture):
  step(-1),targetStep(0),i(0),maxi(0),id(-1),start(pstart),posture(startPosture),checkState(ONCHECK){
}



Time Segment::getWorkTime(){
  if (i==maxi){
    i=0;
    maxi=maxi<<1;
    step=step>>1;
  }
  if (step<targetStep) return INVALIDTIME;  

  Time t=step/2+i*step;
  i++;
  return t;
}

  int Segment::nextId=0;

void Segment::reset(Time end,const RobotConfig& endPosture,double maxStep){
  id=nextId++;
  step=(end-start)/2;
  
  double maxDelta=0;
  for (size_t j=0;j<endPosture.size();++j){
    maxDelta=(std::max)(tAbs(endPosture[j]-posture[j]),maxDelta);
  }
  
  targetStep=(Time)floor(maxStep*(end-start) /maxDelta);  
  maxi=1;
  i=0;
  
}


  int Segment::peekNextId(){
    return nextId;
  }


bool JobManager::State::markBadSegment(Segment::Id id){
  PtrList::iterator it=onCheck.begin();
  for (;it!=onCheck.end()&&((*it)->id!=id);++it);
  if (it==onCheck.end()) return false;
  (*it)->checkState=Segment::BAD;
  cyclicIterator=onCheck.erase(it);  
  return true;
}




void  JobManager::State::markGoodSegment(PtrList::iterator it){
  (*it)->checkState=Segment::OK;
  cyclicIterator=onCheck.erase(it);
}




size_t JobManager::State::fillWorks(JobManager:: Worker& w){
  size_t filled=0;
  for (size_t i=0;i<onCheck.size();i++){//do a maximum run over the checks
    ++cyclicIterator;
    if (cyclicIterator==onCheck.end()){
      cyclicIterator==onCheck.begin();
    }
    Time newTime;
    const Segment& start=*(*cyclicIterator);

    PtrList::const_iterator next=cyclicIterator;
    next++;
    
    const Segment& end=**next;    
    while((newTime=(*cyclicIterator)->getWorkTime())!=INVALIDTIME) { 
      
      linearInterpolation(w.work[filled].data,
			  start.posture.data,
			  end.posture.data,
			  w.work[0].size(),
			  start.start,
			  end.start,
			  newTime);
      w.times[filled++]=newTime;
    }//
    if (newTime==INVALIDTIME){
      markGoodSegment(cyclicIterator);
    }
    if (filled){
      w.segmentId=start.id;      
      for (size_t i=filled;i<w.times.size();i++){
	w.times[i]=INVALIDTIME;
      }
      return filled; //if we fill at least partially, return 
    }
  }//for i
  return filled;
}


JobManager::State::State(JobManager& powner):
  owner(powner){
}



  int JobManager::Worker::getWorkNum(){
    for (size_t i=0;i<times.size();i++){
      if (times[i]==INVALIDTIME) return i;
    }
    return times.size();
  }

void JobManager::Sleepers::wake(){
  for (std::list<Job<State>*>::iterator s=sleepers.begin();
       s!=sleepers.end();
       ++s){
    owner.spool.addJob(*s,WORKERPRIORITY);
  }
}
  
  void JobManager::Sleepers::queue(Job<State>* s){
    sleepers.push_back(s);
  }
  
  JobManager::Sleepers::Sleepers(JobManager& powner):
    owner(powner)
  {
  }
  
  
  
  bool JobManager::WorkGetterJob::operator()(JobManager::State& s){
    wl.resize(s.fillWorks(w));
    if (!wl.size()){
      s.owner.copyFirstWork(w);
      wl.resize(w.getWorkNum());
    }
    if (wl.size()){
      sem.signal();
    }else{
      s.owner.sleepers.queue(this);
    }
    return true;
  }

  bool JobManager::WorkGetterJob::isToCancel(){
    return false;
  }




  JobManager::WorkGetterJob::WorkGetterJob(JobManager:: Worker& pw):w(pw),wl(w.work){
  }

  void JobManager::WorkGetterJob::wait(){
    sem.wait();
  }





void JobManager::State::setMotion(const helium::Motion& m){
  SegList::iterator seg=currSegs.begin();
  if (m.motion.size()<2) return;
  int newIdStart=Segment::peekNextId();
  for ( Motion::MotionMap::const_iterator f=m.motion.begin();f!=m.motion.end();){
    //delete disappeared frames
    while (seg->start<f->first&&seg!=currSegs.end()){      
      seg=currSegs.erase(seg);
    }    
    if (seg->start==f->first){
      Array<double> x=seg->posture;
      Array<double> y=*(f->second);
      if (seg->posture==(*(f->second))){
	continue;
      }else{
	seg=currSegs.erase(seg);	
      }
    }
    //insert new frame when we did not continue   
    //the next seg has t>f->first
    
    Motion::MotionMap::const_iterator next=f;
    next++;
    if (next==m.motion.end()){
      seg=currSegs.insert(seg,Segment(f->first,*f->second));    
    }else{
      seg=currSegs.insert(seg,Segment(f->first,*f->second,
				      next->first,*next->second,
				      owner.maxStep
				      )); 
    }  

    //the previous segment is affected as well
    SegList::iterator prev=seg;
    --prev;	
    if (prev!=currSegs.end()&&prev->id<newIdStart){
      prev->reset(f->first,*f->second,owner.maxStep);
    }
   
 
    f=next;
  }
  onCheck.clear();
  SegList::iterator tail=currSegs.end();
  tail--;
  for(seg=currSegs.begin();seg!=tail;++seg){
    if (seg->checkState==Segment::ONCHECK){
      onCheck.push_back(seg);
    }  
  }
  cyclicIterator=onCheck.begin();
  owner.sleepers.wake();
}


void JobManager::do_setMotion(JobManager::State& s,const helium::Motion& m){
  s.setMotion(m);
  segmentStateChange();
}


void JobManager::setMotion(const helium::Motion& m){
  StackJob<State,
	   JobManager,
	   const helium::Motion&,
	   const helium::Motion&,
	   &JobManager::do_setMotion,
	   false> 
    job(this,m);  
  spool.waitJob(&job,MOTIONCHANGEPRIORITY);
}


void JobManager::do_report(JobManager::State& s,const std::pair<Time,WorkerId>& n){
  if (s.markBadSegment(workers[n.second].segmentId)){
    collidingPosture(n.first);
    segmentStateChange();
  }  
}




void JobManager::report(const std::pair<Time,WorkerId>& collision){
  StackJob<State,
	   JobManager,
	   const std::pair<Time,WorkerId>&,
	   const std::pair<Time,WorkerId>&,
	   &JobManager::do_report, 
	   false> job(this,collision);
    
  spool.waitJob(&job,WORKERPRIORITY);
}



 JobManager::Worker::Worker(int workSize,int configSize):
   segmentId(-1),times(workSize),work(workSize),isRecovering(INVALIDWORKID){
   for (size_t i=0;i<work.size();++i){
     work[i].resize(configSize);
   }
 }


  JobManager::WorkerId JobManager::getWorkerId(int desiredSize){
    WorkerId id=workers.size();
    workers.push_back(Worker(desiredSize,motorNum));
  return id;
}


void JobManager::copyFirstWork(JobManager::Worker& w){
    for (size_t i=0;i<workers.size();i++){
      if (workers[i].isRecovering==JobManager::INVALIDWORKID){
	w=workers[i];
	w.isRecovering=i;
	return;
      }
    }
  }

  JobManager::JobManager(double pMaxStep,int pmotorNum):
  state(*this),
  sleepers(*this),
  spool(state,SPOOLERPRIORITIESNUM),
  maxStep(pMaxStep),
  motorNum(pmotorNum)
{
  spool.start();
}


}
