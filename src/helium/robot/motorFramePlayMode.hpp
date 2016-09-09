/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

namespace  helium{
  
  namespace framePlay{

    inline void Target::clear(){
      ticks.clear();
      for (size_t j=0;j<frames.size();j++){
	frames[j].clear();
      }
    }

    inline std::ostream& operator<<(std::ostream& o,const Target& t){
      o<<t.mp.iters<<" "<<t.mp.startTime<<" "<<t.mp.playSpeed<<" ";
      sizeWrite(o,t.ticks);
      o<<" "<<t.frames.size()<<" ";
      for (size_t j=0;j<t.frames.size();j++){
	sizeWrite(o,t.frames[j]);
	o<<" ";
      }      
      return o;
    };

    inline std::istream& operator>>(std::istream& i,Target& t){
      int n;
      i>>t.mp.iters>>t.mp.startTime>>t.mp.playSpeed;
      sizeRead(t.ticks,i);
      i>>n;
      t.frames.resize(n);      
      for (size_t j=0;j<t.frames.size();j++){
	sizeRead(t.frames[j],i);
      }      
      return i;
    };

    enum {STOPNEXT=-2};
  }


  template<typename MotorSign>
  void MotorFramePlayMode<MotorSign>::resetCursors(){
    wholeNext=0;
    wholePrev=0;
    zero(jointNext);
    zero(jointPrev);
  }

  template<typename MotorSign>
  std::string MotorFramePlayMode<MotorSign>::ControlCell::getName() const{
    return "playing control cell";
  }

  template<typename MotorSign>
  void MotorFramePlayMode<MotorSign>::ControlCell::setData(const framePlay::RequestStop& r){
    Job<Motors>* job=new AutoDeleteJob<Motors,
				       ControlCell,
				       framePlay::PlayState,
				       onlyParam<framePlay::PlayState>,
				       &ControlCell::setPlayingState,
				       true,
				       framePlay::PlayState
				       >(this,framePlay::STOPPING);		
    owner.spool.addJob(job,MotSpoolPriority::HIGH);
  }
  
  
  template<typename MotorSign>
  void MotorFramePlayMode<MotorSign>::ControlCell::do_getData(std::pair<framePlay::PlayState,Time>& p){
	p=state.get();
      }

  template<typename MotorSign>
  void MotorFramePlayMode<MotorSign>::ControlCell::getData(std::pair<framePlay::PlayState,Time>& p){
    StackJob<Motors,
	     ControlCell,
	     std::pair<framePlay::PlayState,Time>&,
	     onlyParam<std::pair<framePlay::PlayState,Time>&>,
	     &ControlCell::do_getData,
	     true,
	     std::pair<framePlay::PlayState,Time>&
	     > job(this,p);
  owner.spool.waitJob(&job,MotSpoolPriority::HIGH);
  }

  template<typename MotorSign>
  void MotorFramePlayMode<MotorSign>::ControlCell::connect(Cell::CellConnection& c){	
    state.connect(c);
  }

  template<typename MotorSign>
  void MotorFramePlayMode<MotorSign>::ControlCell::disconnect(Cell::CellConnection& c){	
    state.disconnect(c);
  }

   template<typename MotorSign>
   MotorFramePlayMode<MotorSign>::ControlCell::ControlCell(MotorFramePlayMode& powner):owner(powner),
										       state(std::pair<framePlay::PlayState,Time>(framePlay::STOPPED,0))
   {
   }
  

  template<typename MotorSign>
  void MotorFramePlayMode<MotorSign>::ControlCell::setCurrFrame(Time t){
    state.getDirectReference().second=t;
    state.signalUpdate();

  }
  
  template<typename MotorSign>
  void MotorFramePlayMode<MotorSign>::ControlCell::setPlayingState(framePlay::PlayState p){
    state.getDirectReference().first=p;
    state.signalUpdate();
    //std::cout<<"Setting state "<<p<<std::endl;
    //printTrace();
  }
  
  template<typename MotorSign>
  void MotorFramePlayMode<MotorSign>::ControlCell::setState(framePlay::PlayState p,Time t){
    state=std::make_pair(p,t);
  }

  template<typename MotorSign>
  bool MotorFramePlayMode<MotorSign>::ControlCell::isStopping(){
    return state.get().first==framePlay::STOPPING;
  }
  
  template<typename MotorSign>
  void MotorFramePlayMode<MotorSign>::setWholeTarget(Motors& m,const framePlay::Target& t){
      this->target=t;

      currIter=0;
      resetCursors();
      //std::cout<<"staring play with offset "<<this->target.mp.startTime<<std::endl;
      zeroTime=time.getTime()-this->target.mp.startTime/this->target.mp.playSpeed;
      endTime=this->target.ticks[((int)this->target.ticks.size())-1];      
      now=this->target.mp.startTime;

      stateCell.setState(framePlay::PLAYING,this->target.mp.startTime);
      //std::cout<<"received target "<<t<<std::endl;
      this->assureSpool();
    }
  
  template<typename MotorSign>
  void MotorFramePlayMode<MotorSign>::getPosture(bool* newMoving,Position* newPos,Motors&){
    //std::cout<<"trying negation"<<std::endl;
    if (stateCell.isStopping()){
      assign(newPos,this->info.currPos,this->info.n);
      setAlln(newMoving,false,this->info.n);
      stateCell.setState(framePlay::STOPPED,now);
      //std::cout<<"STOPPED, "<<"p[2]="<<newPos[2]<<std::endl;
      //std::cout<<" play finished"<<std::endl;
      return;
    }
    if (currIter==framePlay::STOPNEXT){
      setAlln(newMoving,false,this->info.n);
      stateCell.setState(framePlay::STOPPED,endTime);
      return;
    }


    now=(time.getTime()-zeroTime)*this->target.mp.playSpeed;
    //std::cout<<"did negatioN"<<std::endl;

    //std::cout<<"getting posture "<<now<<" end time = "<<(endTime+1)<<std::endl;
    setAlln(newMoving,true,this->info.n);
    
    //std::cout<<"play "<<now<<std::endl;
    for (;now>=(currIter+1)*(endTime+1);){
      resetCursors();
      ++currIter;
      if (currIter==this->target.mp.iters){
	for (int j=0;j<this->info.n;j++){
	  newPos[j]=this->target.frames[j][this->target.frames[j].size()-1].second;
	}		 
	currIter=framePlay::STOPNEXT;
	return;
      };            
    }
    
    
    now=now-currIter*(endTime+1);
    
    for(;wholeNext<((int)this->target.ticks.size())-1&&now>=this->target.ticks[wholeNext];++wholeNext){
	wholePrev=wholeNext;
	stateCell.setCurrFrame(this->target.ticks[wholePrev]);
      }
    
    //alpha cannot be >1, because we checked  if (currIter==this->target.mp.iters)
    
    for (int j=0;j<this->info.n;j++){
      for(;jointNext[j]<((int)this->target.frames[j].size())-1&&
	    now>=this->target.ticks[this->target.frames[j][jointNext[j]].first];++jointNext[j]){
	jointPrev[j]=jointNext[j];
      }
      if (jointPrev[j]==jointNext[j]){
	newPos[j]=this->target.frames[j][jointNext[j]].second;
      }else{	
	int prevTick=this->target.frames[j][jointPrev[j]].first;
	int nextTick=this->target.frames[j][jointNext[j]].first;

	double alpha=double(now-this->target.ticks[prevTick])/
	  (this->target.ticks[nextTick]-this->target.ticks[prevTick]);			      
	newPos[j]=alpha*this->target.frames[j][jointNext[j]].second+(1-alpha)*this->target.frames[j][jointPrev[j]].second;
      }
      
    }//for j
    //std::cout<<"p[2]="<<newPos[2]<<std::endl;
  }//getPosture
  
  template<typename MotorSign>
   motor::ModeId MotorFramePlayMode<MotorSign>:: modeID(){
      return "framePlay";
    }

  template<typename MotorSign>
  MotorFramePlayMode<MotorSign>:: MotorFramePlayMode(Spool& spool,TimeSource& ptime):
    Base(spool,modeID()),jointNext(spool.getMotorNum()),jointPrev(spool.getMotorNum()),time(ptime),stateCell(*this){
      
    }//ctor


  template <typename MotorSign>
  DefDescr<MotorFramePlayMode<MotorSign> >::DefDescr(MotorFramePlayMode<MotorSign>& mlm):Description<MotorFramePlayMode<MotorSign> >(mlm){      
    }

  template <typename MotorSign>
  void DefDescr<MotorFramePlayMode<MotorSign> >::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    hopname=IDPath("motors.framePlayMode",".");
    members.push_back(scalarMapping(this->obj.cells.wholeTarget,"target"));
    members.push_back(scalarMapping(this->obj.stateCell,"state"));
    //members.push_back(scalarMapping(this->obj.currFrame,"currFrame"));
    //members.push_back(scalarMapping(this->obj.playing,"playing"));
  }
  


}
