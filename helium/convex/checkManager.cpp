/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/convex/checkManager.h>
#include <helium/vector/operations.h>
#include <helium/os/time.h>
#include <helium/math/tAbs.h>

#include <helium/util/ioUtil.h>

using std::cout;
using std::endl;




namespace helium{

std::ostream& operator<<(std::ostream& o,const coldet::SegmentState& ss){
  return o<<"("<<ss.start<<"~"<<ss.end<<"@"<<ss.coll<<")";
}


namespace convexCheck{



  Segment::Segment(const double* b,const double* e,int n):
    begin(n),end(n),collTime(UNKNOWN){    
    assign(begin,b);
    assign(end,e);
  }



Work::Work(SegmentCheck& pwm,const Segment* ps):
  check(pwm),state(ONWORK),s(ps),collTime(UNKNOWN){

}

Work::~Work(){
  //std::cout << "reporting" << std::endl;
  check.report(this);
  
}


  SegmentCheck::SegmentCheck(const Segment* ps,Callback<double>* cb):
    segMutex("segmentCheck"),stateChanged(cb),s(ps)
  {
  }


  

bool SegmentCheck::getGhost(std::auto_ptr<Work>& work){
  segMutex.lock();
  for (std::list<Work*>::iterator it=onWork.begin();
       it!=onWork.end();
       ++it){
    if ((*it)->state==Work::REPLACED) continue;
    
    if ((*it)->state==Work::ONWORK){
	(*it)->state=Work::REPLACED;
	Work* w=(*it)->clone();
	work.reset(w);
	w->it=onWork.insert(onWork.end(),w);
    }else{//it is dead
      (*it)->state=Work::ONWORK;
      work.reset(*it);
    }


    segMutex.unlock();
    return true;
      
  }
  segMutex.unlock();
  return false;
}



  bool SegmentCheck::canBeDeleted(){
    segMutex.lock();
    for (std::list<Work*>::iterator it=onWork.begin();
       it!=onWork.end();
       ++it){
      if ((*it)->state!=Work::DEAD){
	//std::cout << "can be deleted is false, has onwork" << std::endl;
	segMutex.unlock();
	return false;
      }
      //std::cout << " for loop dead" << std::endl;
    }
    //std::cout << "can be deleted is true, no onwork" << std::endl;
    segMutex.unlock();
    return true;    
  }
  


  bool SegmentCheck::isProcessingEnded(){
    for (std::list<Work*>::iterator it=onWork.begin();
	 it!=onWork.end();
	 ++it){
      if ((*it)->state!=Work::REPLACED){
	return false;
      }
    }
    return true;
  }

  void SegmentCheck::report(Work* w){
    //cout << "segment check report" << endl;
      segMutex.lock();

      //the deconstructor was invoked, we need to remove it from the list
      onWork.erase(w->it);
      
      if (w->collTime==UNKNOWN&&w->state==Work::ONWORK){
	Work* wc=w->clone();
	wc->it=onWork.insert(onWork.end(),wc);
	wc->state=Work::DEAD;
      }

      if (w->collTime!=NONE&&w->collTime!=UNKNOWN){
	//cout << " colltime " << w->collTime << endl;
	pushCollision(w->collTime);
	endWork();
      }
      
      if (isWorkGenerationEnded()&&isProcessingEnded()){
	if (s->collTime==UNKNOWN)
	  s->collTime=NONE;
	if (stateChanged){
	  segMutex.unlock();
	  (*stateChanged)(s->collTime);
	  segMutex.lock();
	}
      }      
      segMutex.unlock();
      //cout << "segment check report end" << endl;
      return;
  }



  void SegmentCheck::disconnect(){
    segMutex.lock();
    stateChanged=NULL;
    segMutex.unlock();
  }

  void SegmentCheck::connect(Callback<double>* cb){
    segMutex.lock();
    stateChanged=cb;
    segMutex.unlock();
  }



  LinearWork::LinearWork(int pstart,int pend,int pdivisions,
	      SegmentCheck& check,const Segment* s
	       ):Work(check,s),
		 start(pstart),i(pstart),end(pend),divisions(pdivisions)
    {}
    
    double LinearWork::getNext(){            
      double v=i>end?NONE:double(i)/divisions;
      //cout << "i "<< (double)i << " division " << divisions << " end " << end << " v " << v << endl;
      ++i;
      return v;
    }
    Work* LinearWork::clone(){
      return new LinearWork(start,end,divisions,check,s);
    }



  BisectionWork::BisectionWork(int pi,int level,int pcount,
			       SegmentCheck& check,const Segment* s
			       ):Work(check,s),
				 starti(pi),
				 startlevel(level),
				 i(pi),maxi(level),count(pcount),step(1.0/level)
  {}
    
    
    double BisectionWork::getNext(){      

      //cout << "count " << count << " i "<< i << " maxi " << maxi << " step " << step << endl;
      if (count--==0) return NONE;
      if (i==maxi){
	maxi=maxi<<1;
	step=step/2;
	i=0;
      }
      
      //std::cout<<"   ("<<step/2<<"+"<<i<<"*"<<step<<")";
      double v=step/2+i*step;
      //cout << " v " << v << endl;
      i++;
      return v;
    }

    Work* BisectionWork::clone(){
      return new BisectionWork(starti,startlevel,count,check,s);
    }



  bool LinearCheck::getWork(std::auto_ptr<Work>& work,int desired){
    segMutex.lock();
    if (next>divisions){
      segMutex.unlock();
      return false;
    }//next>division
    int start=next;
    next=(std::min)(next+desired,divisions+1);
    Work* w=new LinearWork(start,next-1,divisions,
			   *this,s);
    work.reset(w);
    w->it=onWork.insert(onWork.end(),w);
    //std::cout << "onwork insert  (lincheck getwork)" << onWork.size() << std::endl;
    segMutex.unlock();
    return true;
  }





  bool  LinearCheck::isWorkGenerationEnded(){    
    return next>divisions;
  }



  void LinearCheck::endWork(){
    next=divisions+1;
  }


  void LinearCheck::pushCollision(double d){
    s->collTime=(std::min)(d,s->collTime);
  }

  const double LinearCheck::MAXDELTA=M_PI/180;
  const double BisectionCheck::MAXDELTA=M_PI/180;

  

  LinearCheck::LinearCheck(const Segment* s,Callback<double>* cb):
    SegmentCheck(s,cb),
    next(0),
    divisions(ceil(infinityNormDistance(s->begin,s->end)/MAXDELTA)   )
  {
    
  }

  
  BisectionCheck::BisectionCheck(const Segment* s,Callback<double>* cb):
    SegmentCheck(s,cb),p(1),divisions(2),
    next(0) //fransiska 201311
  {
    double d=infinityNormDistance(s->begin,s->end);
    while(d/divisions>MAXDELTA){
      divisions=divisions<<1;
    }
    divisions=divisions-1;
  }
  


  bool BisectionCheck::getWork(std::auto_ptr<Work>& work,int desired){
    segMutex.lock();
    if (next>=divisions) {
      segMutex.unlock();
      return false;
    }
    while(next+1>=p){
      p=p<<1;
    }
    int count=(std::min)(desired,divisions-next);
    Work* w=new BisectionWork(next-(p/2-1),p/2,count,*this,s);    
    work.reset(w);
    w->it=onWork.insert(onWork.end(),w);
    next=next+count;        
    segMutex.unlock();
    //if(onWork.size())     std::cout << "onwork has size!!!!! (bisection get work)" << std::endl;
    return true;
  }


  bool BisectionCheck::isWorkGenerationEnded(){
    return next>=divisions;
  }
  
  void BisectionCheck::endWork(){
    next=divisions;
  }

  void BisectionCheck::pushCollision(double d){    
    if (tAbs(d-0.5)<tAbs(s->collTime-0.5)){
      s->collTime=d;
    }
  }


  TransitionCheck::TransitionCheck():transCheckMutex("transCh"),currCheck(NULL),ghostCheck(0),desired(2){
    
  }
  

bool TransitionCheck::getWork(std::auto_ptr<Work>& work){
  transCheckMutex.lock();
  if (!currCheck){    
    transCheckMutex.unlock();
    //std::cout<<"no work!"<<std::endl;
    return false;
  }
  if (currCheck->getWork(work,desired)){
    transCheckMutex.unlock();
    //std::cout<<"has work"<<std::endl;
    return true;      
  }
  if (!ghostCheck){
    ghostCheck=getSystemMillis();
    transCheckMutex.unlock();
    return false;
  }else{
    Time now=getSystemMillis();
    if (now-ghostCheck>GHOSTTIME){
      ghostCheck=now;
      transCheckMutex.unlock();
      return currCheck->getGhost(work);
    }else{
      transCheckMutex.unlock();
      return false;
    }
  }
}


void TransitionCheck::cancel(){
  transCheckMutex.lock();
  //std::cout<<"cancellig"<<std::endl;
  if (currCheck){
    currCheck->disconnect();
    currCheck=NULL;
  }
  transCheckMutex.unlock();
}

void TransitionCheck::startCheck(const Segment* s,Callback<double>* cb){
  transCheckMutex.lock();
  if (currCheck){
    if (currCheck->canBeDeleted()){
      delete currCheck;
      currCheck=NULL;
    }else{
      oldChecks.push_back(currCheck);
    }
  }
  currCheck=new LinearCheck(s,cb);
  ghostCheck=0;
  transCheckMutex.unlock();
}


void TransitionCheck::cleanup(){
  transCheckMutex.lock();
  for (std::list<LinearCheck*>::iterator it=oldChecks.begin();
       it!=oldChecks.end();){
    if ((*it)->canBeDeleted()){
      delete *it; //added fransiska 20131126
      it=oldChecks.erase(it);
    }else{
      ++it;
    }
  }
  transCheckMutex.unlock();
}


  
  MotionCheck::MotionCheck():motCheckMutex("motionCheck"),it(checks.begin()),
			     desired(2) //fransiska 20131121
  {

  }

  bool MotionCheck::getWork(std::auto_ptr<Work>& work){    
    //std::cout << "size of checks" << checks.size() << std::endl;
    motCheckMutex.lock();
    if (it==checks.end()){
      motCheckMutex.unlock();
      return false;
    }


    if(it->first->getWork(work,desired)) { //fransiska 20131120 //it->first->getWork(work,desired);
      ++it;
      if (it==checks.end()){
	it=checks.begin();
      }
      motCheckMutex.unlock();    
      return true;
    }
    else {
      motCheckMutex.unlock();
      return false;
    }

  }



  void MotionCheck::cleanup(){
    motCheckMutex.lock();
    for (std::list<BisectionCheck*>::iterator bit=oldChecks.begin();
	 bit!=oldChecks.end();){
      if ((*bit)->canBeDeleted()){
	delete *bit;
	bit=oldChecks.erase(bit);
      }else{
	++bit;
      }
    }
    motCheckMutex.unlock();
  }
  


  void MotionCheck::beginCheckList(){
    motCheckMutex.lock();
    for (CheckList::iterator cit=checks.begin();cit!=checks.end();++cit){
      cit->second=false;
    }	 
  }


  void MotionCheck::endCheckList(){
    int  u=0;
    for (CheckList::iterator cit=checks.begin();cit!=checks.end();){
      if (cit->second==false){ 

	cit->first->disconnect();
	if (cit->first->canBeDeleted()){
	  delete cit->first;
	}else{	  
	  oldChecks.push_back(cit->first);	  
	}
	cit=checks.erase(cit);
      }
      else {
	++cit;
      }
      u++;
    }//for it
    it=checks.begin();
    motCheckMutex.unlock();
  }

  void MotionCheck::addCheck(const Segment*s ,Callback<double> *cb){
    for (CheckList::iterator cit=checks.begin();cit!=checks.end();++cit){
      if (cit->first->s==s){
	cit->second=true;
	cit->first->connect(cb);
	return;
      }
    }
    checks.push_back(std::make_pair(new BisectionCheck(s,cb),true));
  }




}//convexCheck

  CheckManager::CheckManager(int motorN,convex::ConvexCollisionModel& pccm):
    n(motorN),checkManMutex("checkManager"),waiters(0),running(true),ccm(pccm,NULL){
  }
  
    coldet::SegmentNotification CheckManager::getMotionNotification() {
      //cout << "getmotnot " << endl;
      MutexLock l(checkManMutex); 
      return motionNotification; 
    }
  
  void CheckManager::stop(){
    transitionCheck.cancel();
    running=false;
    //std::cout<<waiters<<" waiters"<<std::endl;
    do{
      //std::cout<<"signal waiter (stop)"<<std::endl;
      changeOccurred.signal(waiters);
    }while(waiters);
  }


  CheckManager::~CheckManager(){
    stop();
  }

  bool CheckManager::getWork(std::auto_ptr<convexCheck::Work>& wp){
    while(running){
      //std::cout<<"1"<<std::endl;
      if (transitionCheck.getWork(wp)) return true;
      //std::cout<<"2 "<< motionCheck.getWork(wp) <<std::endl;
      if (motionCheck.getWork(wp)) {
	return true;
      }
      //std::cout<<"3"<<std::endl;
      checkManMutex.lock();
      //std::cout<<"4"<<std::endl;
      waiters++;
      //std::cout<<"5"<<std::endl;
      checkManMutex.unlock();
      //std::cout<<"6"<<std::endl;
      transitionCheck.cleanup();
      //std::cout<<"7"<<std::endl;
      motionCheck.cleanup();
      //std::cout<<"8"<<std::endl;
      changeOccurred.wait();
      //std::cout<<"9"<<std::endl;
      checkManMutex.lock();
      //std::cout<<"10"<<std::endl;
      waiters--;
      //std::cout<<"11"<<std::endl;
      checkManMutex.unlock();
      //std::cout<<"12"<<std::endl;
    }
    return false;
  }//getWork


  void CheckManager::checkTransition(Callback<double>* cb,const double* start,const double* end){
  MutexLock lock(checkManMutex);
  
  std::pair<std::set<convexCheck::Segment>::iterator,bool> p=
    linSegments.insert(convexCheck::Segment(start,end,n));
  if (p.first->collTime!=convexCheck::UNKNOWN){//already inserted    
    (*cb)(p.first->collTime);
    return;
  }    

  if (binSegments.find(cmp(p.first->begin,p.first->end)<=0?
		       *p.first:convexCheck::Segment(p.first->end,p.first->begin,n)
		       )->collTime==convexCheck::NONE){
    (*cb)(convexCheck::NONE);
    return;
  }
  transitionCheck.startCheck(&*p.first,cb);  
  //std::cout << "signal waiter (checkmanager checktransition)"  << std::endl;
  changeOccurred.signal(waiters);
}


  bool CheckManager::FullCheck::operator()(const convexCheck::Segment& f,const convexCheck::Segment& s) const{
    int ret=cmp(f.begin,s.begin);
    if (ret) return ret<0;
    return cmp(f.end,s.end)<0;
  }



  /*  bool CheckManager::NoOrderCheck::operator()(const convexCheck::Segment& f,const convexCheck::Segment& s) const{
    const double *fb,*fe,*sb,*se;
    if (cmp(f.begin,f.end)<0){
      fb=f.begin;
      fe=f.end;
    }else{
      fb=f.end;
      fe=f.begin;
    }    
    if (cmp(s.begin,s.end)<0){
      sb=s.begin;
      se=s.end;
    }else{
      sb=s.end;
      se=s.begin;
    }
    int ret=cmpn(fb,sb,f.begin.size());
    if (ret) return ret<0;
    return cmpn(fe,se,f.end.size())<0;
  }

  */






void CheckManager::cancelTransition(){
  transitionCheck.cancel();
}






inline Time mapTime(Time begin,Time end,double val){
  //std::cout << begin << "~" << end << " " << val << " : ";
  if (val==convexCheck::UNKNOWN) {
    //std::cout << "returning checking" << std::endl;
    return coldet::SegmentState::CHECKING;
  }
  if (val==convexCheck::NONE) {
    //std::cout << "returning non coll" << std::endl;
    return coldet::SegmentState::NOCOLLISION;
  }
  //std::cout << "returning coll" << begin+round((end-begin)*val) << std::endl;
  return begin+round((end-begin)*val);
}

  
void CheckManager::checkMotion(const Motion::MotionMap& mm){
  //std::cout << "==checkmotion" << std::endl;
  //std::cout << mm << std::endl;

  checkManMutex.lock();
  motionNotification.clear();
  notificationInfo.clear();
  destroyIS(edgeCollidingSegments);
  Motion::MotionMap::const_iterator it=mm.begin();
  Time prevTime=it->first;
  const double* prev=(*it->second);
  ccm.rotate(prev);
  bool prevColliding=ccm.isColliding(true);
  ++it;
  bool colliding = prevColliding;
  const convexCheck::Segment* thisSegment;
  motionCheck.beginCheckList();
  for (;it!=mm.end();prev=*it->second,prevTime=it->first,prevColliding=colliding,++it){        

    helium::Posture p = it->second.getData();    

    ccm.rotate(*it->second);      
    colliding=ccm.isColliding(true);
    
    bool reverseTime=false;
    
    if (prevColliding||colliding){
      thisSegment=new convexCheck::Segment(prev,*it->second,n);
	edgeCollidingSegments.push_back(thisSegment);
	thisSegment->collTime=prevColliding?0.0:convexCheck::NONE;
    }else{
      std::pair<std::set<convexCheck::Segment>::iterator,bool> p;
      reverseTime=cmpn(prev,(*it->second),n)>0;
      p=binSegments.insert(reverseTime?
			 convexCheck::Segment((*it->second),prev,n):
			 convexCheck::Segment(prev,(*it->second),n)
			 );
      
      thisSegment=&*p.first;
      if (p.first->collTime==convexCheck::UNKNOWN){
	motionCheck.addCheck(thisSegment,this);
      }
      
    }
    
    motionNotification.push_back
      (
       coldet::SegmentState(prevTime,it->first,coldet::SegmentState::CHECKING)//coll time is recomputed in the operator function
       );
    
    notificationInfo.push_back(NotificationInfo(thisSegment,reverseTime));
  }//for 
  //the end frame
  if (colliding){
    motionNotification.push_back(coldet::SegmentState(prevTime,prevTime,coldet::SegmentState::CHECKING));
    thisSegment=new helium::convexCheck::Segment(prev,prev,n);
      thisSegment->collTime=0;
      notificationInfo.push_back(NotificationInfo(thisSegment,false));
  }
  
  motionCheck.endCheckList();

  checkManMutex.unlock();
  changeOccurred.signal(waiters);
  (*this)(0);//notify what is (un)known //signals for motionNotification update
  //std::cout<<binSegments.size()<<std::endl;
}//checkMotion  


void CheckManager::operator()(double){
  checkManMutex.lock();
  for (size_t i=0;i<motionNotification.size();i++){
    motionNotification[i].coll=mapTime(motionNotification[i].start,
				       motionNotification[i].end,
				       std::max(notificationInfo[i].reverse?1-notificationInfo[i].s->collTime:notificationInfo[i].s->collTime,
						notificationInfo[i].s->collTime) //return the enum if collTime is not a collTime (between 0~1)
				       );
    
  }
  notifications(motionNotification);
  checkManMutex.unlock();
}

  helium::Time CheckManager::getFirstCollisionTime() {
    checkManMutex.lock();
    helium::Time hasColdet = -100;
    for(coldet::SegmentNotification::iterator it=motionNotification.begin();it!=motionNotification.end();it++) {
      if(it->coll == coldet::SegmentState::CHECKING || 
	 it->coll != coldet::SegmentState::NOCOLLISION) {
	hasColdet = it->coll;
	break;
      }
    }
    checkManMutex.unlock();
    return hasColdet;
  }

}//helium



