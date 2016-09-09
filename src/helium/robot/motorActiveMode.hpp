/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

namespace helium{

    static const int POTENTIOCHECKS=1;
  
    
  template<typename MotorSign> 
  void mpAdapter<MotorSign,motor::IFACELEVEL>::rotate(Motors& m,bool* moving,Position* newPos){
	m.high->rotate(moving,newPos);
  }

  template<typename MotorSign> 
  void mpAdapter<MotorSign,motor::IFACELEVEL>::getPosture(Motors& m,Position* p){
	m.hw->getPosition(p);
  }

  template<typename MotorSign> 
  typename mpAdapter<MotorSign,motor::IFACELEVEL>::Position   
  mpAdapter<MotorSign,motor::IFACELEVEL>::getPosture(Motors& m,int j){
	return m.hw->getPosition(j);
      }
  template<typename MotorSign> 
  void mpAdapter<MotorSign,motor::IFACELEVEL>::setCurrentPosition(Motors& m,int j){
      typename motor::Raw<MotorSign>::Position cpos=m.hw->getPotentiometer(j,POTENTIOCHECKS);
      //std::cout<<"potentio "<<j<<" is"<<std::hex<<(int)cpos<<std::endl;
      //std::cout<<"converted is"<<m.hw->convPos().normPos(cpos,j)<<std::endl;
      m.high->rotate(j,m.hw->convPos().normPos(cpos,j));
  }

  template<typename MotorSign>
  motor::Interface::Position mpAdapter<MotorSign,motor::IFACELEVEL>::normalize(Motors& m,int j,Position& p){
    return p;
  }

  template<typename MotorSign> 
  void mpAdapter<MotorSign,motor::RAWLEVEL>::rotate(Motors& m,bool* moving,Position* newPos){
	m.raw->rawRotate(moving,newPos);
  }
  

  template<typename MotorSign> 
  void mpAdapter<MotorSign,motor::RAWLEVEL>::getPosture(Motors& m,Position* p){
	m.hw->rawGetPosition(p);
      }

  template<typename MotorSign> 
  typename mpAdapter<MotorSign,motor::RAWLEVEL>::Position   
  mpAdapter<MotorSign,motor::RAWLEVEL>::getPosture(Motors& m,int j){
    return m.hw->rawGetPosition(j);
  }

   template<typename MotorSign> 
   void mpAdapter<MotorSign,motor::RAWLEVEL>::setCurrentPosition(Motors& m,int j){
     Position cpos=m.hw->getPotentiometer(j,POTENTIOCHECKS);
     m.raw->rawRotate(j,cpos);	
    }

   template<typename MotorSign> 
   motor::Interface::Position  mpAdapter<MotorSign,motor::RAWLEVEL>::normalize(Motors& m,int j,Position& p){
	return m.hw->convPos().normPos(p,j);
      }

  template<typename MotorSign>
  void  mpAdapter<MotorSign,motor::HWLEVEL>::rotate(Motors& m,bool* moving,Position* newPos){
    m.hw->rawRotate(moving,newPos);
  }

  template<typename MotorSign>
  void mpAdapter<MotorSign,motor::HWLEVEL>::getPosture(Motors& m,Position* p){
	m.hw->rawGetPosition(p);
      }

  template<typename MotorSign>
  typename mpAdapter<MotorSign,motor::HWLEVEL>::Position
  mpAdapter<MotorSign,motor::HWLEVEL>::getPosture(Motors& m,int j){
	return m.hw->rawGetPosition(j);
  }

  template<typename MotorSign>
  void mpAdapter<MotorSign,motor::HWLEVEL>::setCurrentPosition(Motors& m,int j){
	Position cpos=m.hw->getPotentiometer(j,POTENTIOCHECKS);
	m.hw->hwRotate(j,cpos);	
      }
  
  template<typename MotorSign>
  motor::Interface::Position mpAdapter<MotorSign,motor::HWLEVEL>::normalize(Motors& m,int j,Position& p){
	return m.hw->convPos().normPos(p,j);
  }



  
 

  template<typename MotorSign,typename Mode,typename Target> 
  void TargetCellVirtualBase<MotorSign,Mode,Target>::setData(const Target& st){
      StackJob<Motors,
	TargetCellVirtualBase,
	const Target&,
	const Target&,
	       &TargetCellVirtualBase::do_setWholeTarget,true> job(this,st);
      owner.spool.waitJob(&job,owner.wholeTargetSetPriority);
    }

  template<typename MotorSign,typename Mode,typename Target> 
  void TargetCellVirtualBase<MotorSign,Mode,Target>::do_reachWholeTarget(Motors& m,const ReachTargetParam* t){	
      if (!owner.controlledMotors){
	std::cerr<<"Warning: setting target with no controlled motor"<<std::endl;
      }
      owner.wholeTargetEnd.signalAndClear(TARGETCHANGED);
      owner.wholeTargetEnd.connect(t->first);//we need to connect after signalling!
      owner.setWholeTarget(m,*t->second);
    }

  template<typename MotorSign,typename Mode,typename Target> 
  void TargetCellVirtualBase<MotorSign,Mode,Target>::setData(TargetEndReason& sp,const Target& st){
    ValueWaiter<TargetEndReason,true> vw(&sp);
      ReachTargetParam p(&vw,&st);
      EhOnceJob<Motors,
	TargetCellVirtualBase,
	ReachTargetParam*,
	const ReachTargetParam*,
		&TargetCellVirtualBase::do_reachWholeTarget,true> job(this,&p,vw);
      owner.spool.addJob(&job,owner.wholeTargetSetPriority);
      vw.wait();
    }  

  template<typename MotorSign,typename Mode,typename Target> 
  void TargetCellBase<MotorSign,Mode,Target>::streamSet(std::istream& i){      
      Target* t=this->owner.getWholeTarget(i);
      this->setData(*t);
      delete t;
    }

  template<typename MotorSign,typename Mode,typename Target> 
  void TargetCellBase<MotorSign,Mode,Target>::streamSet(std::ostream& o,std::istream& i){      
      TargetEndReason sp;
      Target* t=this->owner.getWholeTarget(i);     
      this->setData(sp,*t);
      o<<en::io<<sp;
      delete t;
    }

  template<typename MotorSign,typename Mode,typename JointTarget> 
  std::string JointTargetCellBase<MotorSign,Mode,JointTarget>::getName() const{
      return "joint target cell";
    }

  template<typename MotorSign,typename Mode,typename JointTarget> 
  void JointTargetCellBase<MotorSign,Mode,JointTarget>::do_setJointTarget(Motors& m,const JointTarget& st){
      if (!owner.controlledMotors){
	std::cerr<<"Warning: setting target with no controlled motor"<<std::endl;
      }
      owner.info.targetEnd[j].signalAndClear(TARGETCHANGED);
      owner.setJointTarget(m,st,j);
    }
  
  template<typename MotorSign,typename Mode,typename JointTarget> 
  void JointTargetCellBase<MotorSign,Mode,JointTarget>::setData(const JointTarget& st){
      StackJob<Motors,
	JointTargetCellBase,
	const JointTarget&,
	const JointTarget&,
	       &JointTargetCellBase::do_setJointTarget,true> job(this,st);
      owner.spool.waitJob(&job,owner.jointTargetSetPriority);
    }

  template<typename MotorSign,typename Mode,typename JointTarget> 
  void JointTargetCellBase<MotorSign,Mode,JointTarget>::streamSet(std::istream& i){      
      JointTarget* t=owner.getJointTarget(i,j);
      setData(*t);
      delete t;
    }

  template<typename MotorSign,typename Mode,typename JointTarget> 
  void JointTargetCellBase<MotorSign,Mode,JointTarget>::do_reachJointTarget(Motors& m,const ReachTargetParam* t){	
      if (!owner.controlledMotors){
	std::cerr<<"Warning: setting target with no controlled motor"<<std::endl;
      }
      owner.info.targetEnd[j].signalAndClear(TARGETCHANGED);
      owner.info.targetEnd[j].connect(t->first);
      owner.setJointTarget(m,*t->second,j);
    }

   template<typename MotorSign,typename Mode,typename JointTarget> 
  void JointTargetCellBase<MotorSign,Mode,JointTarget>::setData(TargetEndReason& sp,const JointTarget& st){
      ValueWaiter<TargetEndReason,true> vw(&sp);
      ReachTargetParam p(&vw,&st);
      EhOnceJob<Motors,
	JointTargetCellBase,
	ReachTargetParam*,
	const ReachTargetParam*,
		&JointTargetCellBase::do_reachJointTarget,true> job(this,&p,vw);
      owner.spool.addJob(&job,owner.jointTargetSetPriority);
      vw.wait();
    }  

  template<typename MotorSign,typename Mode,typename JointTarget> 
  void JointTargetCellBase<MotorSign,Mode,JointTarget>::streamSet(std::ostream& o,std::istream& i){      
      TargetEndReason sp;
      JointTarget* t=owner.getJointTarget(i,j);     
      setData(sp,*t);
      o<<en::io<<sp;
      delete t;
    }

  template<typename MotorSign,typename Mode,typename JointTarget> 
  JointTargetCellBase<MotorSign,Mode,JointTarget>::JointTargetCellBase(Mode& powner,int pj):owner(powner),j(pj){
  }



  template<typename MotorSign,motor::MotorLevel L,typename Super,typename ModeSignature>
  MotorActiveMode<MotorSign,L,Super,ModeSignature>::JointInfo::JointInfo(int pn):n(pn){
	  currPos=new P[n];
	  setAlln(currPos,MotorSign::INVALIDPOS,n);

	  newPos=new P[n];
	  setAlln(currPos,MotorSign::INVALIDPOS,n);
      
	  currMoving=new bool[n];
	  setAlln(currMoving,false,n);

	  newMoving=new bool[n];
	  setAlln(newMoving,false,n);

	  mask=new bool[n];
	  setAlln(mask,false,n);

	  hlPos=new CellSignal<motor::PositionCellSignature::UT,false>[n];

	  targetEnd=new  Signal<TargetEndReason>[n];
}
  
  template<typename MotorSign,motor::MotorLevel L,typename Super,typename ModeSignature>
  MotorActiveMode<MotorSign,L,Super,ModeSignature>::JointInfo::~JointInfo(){
    delete[] currPos;
    delete[] newPos;
    delete[] currMoving;
    delete[] newMoving;
    delete[] mask;
    delete[] hlPos;
    delete[] targetEnd;
  }
  
  template<typename MotorSign,motor::MotorLevel L,typename Super,typename ModeSignature>
  void MotorActiveMode<MotorSign,L,Super,ModeSignature>:: do_assureSpool(){
      if (!isMoving){     
	isMoving=true;
	spool.addJob(this,Super::spoolingPriority);
      }
    }
  template<typename MotorSign,motor::MotorLevel L,typename Super,typename ModeSignature>
  bool MotorActiveMode<MotorSign,L,Super,ModeSignature>::operator()(Motors& m){
     static_cast<Super*>(this)->getPosture(info.newMoving,info.newPos,m);     
     elemWiseAndn(info.newMoving,info.mask,info.n);
     //Time t=getSystemMillis();
     try{

       adapter::rotate(m,info.newMoving,info.newPos);
       //std::cout<<"time for rot="<<getElapsedMillis(t)<<" ";
       if (controlledMotors==info.n){
	 adapter::getPosture(m,info.currPos);        
       }else{
	 for (int j=0;j<info.n;j++){
	   if (info.mask[j]){
	     info.currPos[j]=adapter::getPosture(m,j);  
	   }
	 }
       }
       //std::cout<<"time for gp="<<getElapsedMillis(t)<<" "<<std::endl;
     }catch(exc::Exception& ){
       for (int i=0;i<info.n;i++){
	 info.targetEnd[i].signalAndClear(TARGETERROROCCURRED);
       }
       wholeTargetEnd.signalAndClear(TARGETERROROCCURRED);
       throw;//inform the hw state monitor
     }
     
     bool newMoving=false;
     for (int i=0;i<info.n;i++){
       if (info.newMoving[i]&&!info.currMoving[i]){
	    info.hlPos[i](motor::Interface::MOVINGPOS);
       }else if (!info.newMoving[i]&&info.currMoving[i]){
	 motor::Interface::Position npos=adapter::normalize(m,i,info.currPos[i]);
	 info.hlPos[i](npos);
	 info.targetEnd[i].signalAndClear(TARGETREACHED);
       }	  
       newMoving=newMoving||info.newMoving[i];
     }
     
     assign(info.currMoving,info.newMoving,info.n);
     
     if (!newMoving){
       //std::cout<<"reached!"<<std::endl;
       wholeTargetEnd.signalAndClear(TARGETREACHED);       
     }
     isMoving=newMoving;
     //std::cout<<"new moving="<<newMoving<<std::endl;
     //std::cout<<"time to end="<<getElapsedMillis(t)<<std::endl;
     return !newMoving;    
   }
  
  template<typename MotorSign,motor::MotorLevel L,typename Super,typename ModeSignature>
  bool MotorActiveMode<MotorSign,L,Super,ModeSignature>::isToCancel(){
    return false;
  }

  template<typename MotorSign,motor::MotorLevel L,typename Super,typename ModeSignature>
  void MotorActiveMode<MotorSign,L,Super,ModeSignature>::updateControlledMotors(){
      controlledMotors=0;
      for (int i=0;i<info.n;i++){
	if (info.mask[i]){
	  controlledMotors++;
	}
      }//for i     

  }

    template<typename MotorSign,motor::MotorLevel L,typename Super,typename ModeSignature>
  void  MotorActiveMode<MotorSign,L,Super,ModeSignature>::addMotor(Motors& m,int j,motor::ModeId prevMode){
      //std::cout<<this->getMotorModeId()<<"adding motor "<<j<<std::endl;
      info.mask[j]=true;
      info.currMoving[j]=false;
      adapter::setCurrentPosition(m,j);
      m.hw->setPowerState(j,true);	
      info.currPos[j]=adapter::getPosture(m,j);
      //std::cout<<"add motor,set the current posture["<<j<<"] as"<<info.currPos[j]<<std::endl;      
      updateControlledMotors();
    }
     
    template<typename MotorSign,motor::MotorLevel L,typename Super,typename ModeSignature>
  void MotorActiveMode<MotorSign,L,Super,ModeSignature>::removeMotor(Motors& m,int j,motor::ModeId prevMode){
      info.mask[j]=false;
      updateControlledMotors();
    }

    template<typename MotorSign,motor::MotorLevel L,typename Super,typename ModeSignature>
      motor::Interface::Position   MotorActiveMode<MotorSign,L,Super,ModeSignature>::getPosition(Motors& m,int j){
      if (info.currMoving[j]){
	return motor::Interface::MOVINGPOS; 
      }
      if (!info.mask[j]){
	return motor::Interface::INVALIDPOS; 
      }
      return adapter::normalize(m,j,info.currPos[j]);
    }

    template<typename MotorSign,motor::MotorLevel L,typename Super,typename ModeSignature>
      void MotorActiveMode<MotorSign,L,Super,ModeSignature>::connect(Cell::CellConnection& ref,int j){
      info.hlPos[j].connect(ref);
    }
    
    template<typename MotorSign,motor::MotorLevel L,typename Super,typename ModeSignature>
      void MotorActiveMode<MotorSign,L,Super,ModeSignature>::disconnect(Cell::CellConnection& ref,int j){
      info.hlPos[j].disconnect(ref);
    }
    
    template<typename MotorSign,motor::MotorLevel L,typename Super,typename ModeSignature>
       void MotorActiveMode<MotorSign,L,Super,ModeSignature>::assureSpool(){
       Job<Motors>* pjob=new AutoDeleteJob<Motors,
					   MotorActiveMode,
					   void,
					   onlyParam<void>,
					   &MotorActiveMode::do_assureSpool,true>(this);
       spool.addJob(pjob,Super::spoolingPriority);
     }

    template<typename MotorSign,motor::MotorLevel L,typename Super,typename ModeSignature>
    MotorActiveMode<MotorSign,L,Super,ModeSignature>::MotorActiveMode(Spool& pspool,motor::ModeId motorMode):
      Spool::MotorMode(motorMode),
      spool(pspool),info(spool.getMotorNum()),cells(*static_cast<Super*>(this),spool.getMotorNum()),controlledMotors(0),isMoving(false){
    }

      template<typename MotorSign> const motor::ModeId mpAdapter<MotorSign,motor::IFACELEVEL>::modeId="norm";
  template<typename MotorSign> const motor::ModeId mpAdapter<MotorSign,motor::RAWLEVEL>::modeId="raw";
  template<typename MotorSign> const motor::ModeId mpAdapter<MotorSign,motor::HWLEVEL>::modeId="hw";


     
  
}
