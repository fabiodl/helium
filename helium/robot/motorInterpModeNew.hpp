#include <helium/core/isSame.h>
#include <helium/os/time.h>
#include <helium/callback/valueWaiter.h>
namespace helium{


 
  

    
  template<>  struct EnumStrings<intrp::NotificationType>{
    static const int NUM=2;
    static const char* strings[NUM];
  };

  template<>  struct EnumStrings<intrp::TargetType>{
    static const int NUM=4;
    static const char* strings[NUM];
  };


 
 
   
  namespace intrp{
    template<typename T,bool D,typename ME,intrp::TargetType type=intrp::VTTARGET,bool same=isSame<typename getTTClass<T,D,type>::value,ME>::value >
    struct findTTType{
      static intrp::TargetType get(){
	return findTTType<T,D,ME,intrp::TargetType(type-1)>::get();
      }
    };
    
    
    template<typename T,bool D,typename ME,intrp::TargetType type> struct findTTType<T,D,ME,type,true>{
      static intrp::TargetType get(){
	return type;
      }
    };  
    
    
    template<typename T,typename ME,bool D>
    void TargetOption<T,ME,D>::out(std::ostream& o){o<<*static_cast<ME*>(this);}
    
    template<typename T,typename ME,bool D>
    void TargetOption<T,ME,D>::in(std::istream& i){i>>*static_cast<ME*>(this);}  
    
    template<typename T,typename ME,bool D>
    intrp::TargetType  TargetOption<T,ME,D>::getTypeID(){
      return findTTType<T,D,ME>::get();
    }
    
    template<typename T,typename ME>
    void TargetOption<T,ME,true>::out(std::ostream& o){o<<*static_cast<ME*>(this);}
    
    template<typename T,typename ME>
    void TargetOption<T,ME,true>::in(std::istream& i){i>>*static_cast<ME*>(this);}

    
    template<typename T,typename ME>
    intrp::TargetType  TargetOption<T,ME,true>::getTypeID(){
      return findTTType<T,true,ME>::get();
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& o,const Target<T>& t){
      o<<en::io<<t.real->getTypeID()<<" ";
      t.real->out(o);
      return o;
    };


    template<typename T,intrp::TargetType TT> inline void streamIn(std::istream& i, Target<T>& t){
      typedef typename getTTClass<T,true,TT>::value Real; 
      t.real=new Real();//this is deleted when updated
      i>>*static_cast<Real*>(t.real); //may use in(istrem& i)
    }

    template<typename T>
    std::istream& operator>>(std::istream& i,Target<T>& t){
      intrp::TargetType type;
      i>>en::io>>type;
      switch(type){
      case PTTARGET:
	streamIn<T,PTTARGET>(i,t);
	break;       	
      case RPTTARGET:
	streamIn<T,RPTTARGET>(i,t);
	break;      
      case PVTARGET:
	streamIn<T,PVTARGET>(i,t);
	break;       
      case VTTARGET:
	streamIn<T,VTTARGET>(i,t);
	break;       	
      }      //switch
      return i;
    }
    
    template<typename T>
    std::ostream& operator<<(std::ostream& o,const TargetProps<T>& tp){
      return o<<en::io<<tp.state<<" "<<tp.val;//<<" "<<tp.remaining;
    }

    template<typename T>
    std::istream& operator>>(std::istream& i,TargetProps<T>& tp){
      i>>en::io>>tp.state;
      read(tp.val,i);
      return i;//>>tp.remaining;
    }

    template<typename T> std::ostream& operator<<(std::ostream& o,const TargetNotification<T>& tn){
      return o<<en::io<<tn.notifyType<<" "<<tn.val;
    }

    template<typename T> std::istream& operator>>(std::istream& i,TargetNotification<T>& tn){
      return i>>en::io>>tn.notifyType>>tn.val;
    }

    namespace spt{
      struct TargetDinfo:public DisconnectInfo{
      DisconnectInfo *req,*rea,*err;
    
      TargetDinfo():req(NULL),rea(NULL),err(NULL){
      }
    
      ~TargetDinfo(){
	if (req!=NULL){
	  //std::cout<<"~TargetDinfo::req"<<std::endl;
	  delete req;	  
	}
	if (rea!=NULL){
	  //std::cout<<"~TargetDinfo::rea"<<std::endl;
	  delete rea;
	}
	if (err!=NULL){
	  //std::cout<<"~TargetDinfo::err"<<std::endl;
	  delete err;
	}
      }
    
    };


    inline void multipleConnect(Signal<Cell::CellSignalData>& request, Signal<Cell::CellSignalData>& reached, ExceptionSignal& onError,
			 Cell::CellConnection& ref,int flags){
      TargetDinfo* dd=new TargetDinfo();
      //std::cout<<"new dd @ "<<dd<<std::endl;
      if (flags&NOTIFYREQUEST){
	request.connect(ref);
	dd->req=ref.disc;
	//std::cout<<"conn to req "<<dd->req<<std::endl;
      }
      if (flags&intrp::NOTIFYREACHED){
	reached.connect(ref);
	dd->rea=ref.disc;
	//std::cout<<"conn to rea "<<dd->rea<<std::endl;
      }
      onError.connect(ref);
      dd->err=ref.disc;
      //std::cout<<"conn to err "<<dd->err<<std::endl;      
      ref.disc=dd;      
    }
    
    inline void multipleDisconnect(Signal<Cell::CellSignalData>& request, Signal<Cell::CellSignalData>& reached, ExceptionSignal& onError,
			    Cell::CellConnection& ref){

      TargetDinfo* dd=trust_cast<TargetDinfo*>(ref.disc);
      //std::cout<<"retrived dd @ "<<dd<<std::endl;
      if (dd->req!=NULL){
	ref.disc=dd->req;
	//std::cout<<"retrieved req "<<dd->req<<std::endl;
	request.disconnect(ref);
	dd->req=NULL;
      }
      if (dd->rea!=NULL){
	ref.disc=dd->rea;
	//std::cout<<"retrieved rea "<<dd->rea<<std::endl;
	reached.disconnect(ref);
	dd->rea=NULL;
      }	
      ref.disc=dd->err;
      //std::cout<<"retrieved err "<<dd->err<<std::endl;
      onError.disconnect(ref);
      dd->err=NULL;
      //std::cout<<"deleting "<<dd<<std::endl;
      ref.clearDisc();
    }
      
      static const int POTENTIOCHECKS=1;
    
    template<typename PT,typename PS> struct tmAdapter<PT,PS,motor::IFACELEVEL>{
      typedef typename MotorSpooler<PT,PS>::Motors Motors;
      typedef motor::Interface Motor;
      typedef Motor::Position Position;
      
      //static const PosFormat value=NORMFORMAT;
      static const motor::ModeId modeId;

      inline static void setCurrentPosition(Motors& m,int j){
	typename motor::Raw<PT,PS>::Position cpos=m.hw->getPotentiometer(j,POTENTIOCHECKS);
	m.high->rotate(j,m.hw->convPos().normPos(cpos,j));	
      }

      inline static void readPosition(Position& pos,Motors& m,int j){
	pos=m.hw->getPosition(j); //this is the Iface of the hw
      }
      inline static void assignZeroPosition(Position& pos,Motors& m,int ){
	pos=0;
      }
      inline static void rotate(Motors& m,int j,Position pos){
	m.high->rotate(j,pos);
      }

      inline static motor::Interface::Position normalize(Position& p,Motors& m,int j){
	return p;
      }
    };

    template<typename PT,typename PS> struct tmAdapter<PT,PS,motor::RAWLEVEL>{
      typedef typename MotorSpooler<PT,PS>::Motors Motors;
      typedef motor::Raw<PT,PS> Motor;
      typedef typename Motor::Position Position;

      //static const PosFormat value=RAWFORMAT;
      static const motor::ModeId modeId;

      inline static void setCurrentPosition(Motors& m,int j){
	typename motor::Raw<PT,PS>::Position cpos=m.hw->getPotentiometer(j,POTENTIOCHECKS);
	m.raw->rotate(j,cpos);	
      }


      inline static void readPosition(Position& pos,Motors& m,int j){
	pos=m.hw->rawGetPosition(j); //this 
      }
      inline static void assignZeroPosition(Position& pos,Motors& m,int j){
	pos=m.hw->convPos().unnormPos(0,j);
      }
      inline static void rotate(Motors& m,int j,Position pos){
	m.raw->rawRotate(j,pos);
      }

      inline static motor::Interface::Position normalize(Position& p,Motors& m,int j){
	return m.hw->convPos().normPos(p,j);
      }
      
    };
    
    template<typename PT,typename PS> struct tmAdapter<PT,PS,motor::HWLEVEL>{
      typedef typename MotorSpooler<PT,PS>::Motors Motors;
      typedef motor::Raw<PT,PS> Motor;
      typedef typename Motor::Position Position;

      //static const PosFormat value=RAWFORMAT;
      static const motor::ModeId modeId;

 
     inline static void setCurrentPosition(Motors& m,int j){
	typename motor::Raw<PT,PS>::Position cpos=m.hw->getPotentiometer(j,POTENTIOCHECKS);
	m.hw->rotate(j,cpos);	
      }



      inline static void readPosition(Position& pos,Motors& m,int j){
	pos=m.hw->rawGetPosition(j);
      }
      inline static void assignZeroPosition(Position& pos,Motors& m,int j){
	pos=m.hw->convPos().unnormPos(0,j);
      }
      inline static void rotate(Motors& m,int j,Position pos){
	m.hw->rawRotate(j,pos);
      }

      inline static motor::Interface::Position normalize(Position& p,Motors& m,int j){
	return m.hw->convPos().normPos(p,j);
      }

    };
      
    }//spt

    

  }//ns intrp

  template<typename PT,typename PS,motor::MotorLevel L> 
  void InterpMode<PT,PS,L>::TargetInfo::getProps(intrp::TargetProps<Position>* t){
    t->state=state;
    t->val=target;
    //t->remaining=(state==intrp::REACHED||reachTime==INVALIDTIME)?INVALIDTIME:reachTime-(getSystemMillis()-zeroTime);
  }

  template<typename PT,typename PS,motor::MotorLevel L> 
  void InterpMode<PT,PS,L>::TargetInfo::updatePosture(Motors& m,Time now){
    Time elapsed=now-zeroTime;
    if (elapsed>reachTime){
      currPos=target;
      state=intrp::REACHED;
      targetReached(j);
      motor::Interface::Position realPos=m.hw->getPosition(j);
      hlPosition(&realPos);
    }else{
      currPos=origin+double(elapsed)/reachTime*(target-origin);
    }	 
  }

  template<typename PT,typename PS,motor::MotorLevel L> 
  void InterpMode<PT,PS,L>::TargetInfo::setNew(intrp::TargetInterface<Position>* tar){
    if (state==intrp::DISABLED){
      tar->finalize();
      throw exc::InvalidOperation("Setting the target of a disabled motor");
    }	
    tar->update(target,reachTime,currPos,j);

    zeroTime=getSystemMillis();	  
    origin=currPos;
    if (state!=intrp::REACHING){
      hlPosition(&motor::Interface::MOVINGPOS);
    }
    state=intrp::REACHING;
    //std::cout<<"new target to be reached in "<<reachTime<<" now "<<zeroTime<<std::endl;
    tar->finalize();
    newTarget(j);	
  }

  template<typename PT,typename PS,motor::MotorLevel L> 
  InterpMode<PT,PS,L>::TargetInfo::TargetInfo(int pj):state(intrp::DISABLED),
						      origin(tmAdapter::Motor::INVALIDPOS),
						      currPos(tmAdapter::Motor::INVALIDPOS),
						      target(tmAdapter::Motor::INVALIDPOS),
						      zeroTime(INVALIDTIME),reachTime(INVALIDTIME),j(pj){
    onError.connect(&hlPosition);
  }

  template<typename PT,typename PS,motor::MotorLevel L> 
  void InterpMode<PT,PS,L>::ReachingSet::erase(int j){
    onReaching.erase(j);
    if (!onReaching.size()){
      transitionToInactive();
    }     
  }
  
  
  template<typename PT,typename PS,motor::MotorLevel L> 
  void InterpMode<PT,PS,L>::ReachingSet::insert(int j){
     if (onReaching.insert(j).second){
       if (onReaching.size()==1){
	 transitionToActive();
       }	 
     }
   }


    template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::TargetCell::NotificationBuilder::operator()(int j){
    n.val=owner.im.info[j].target;
    eventSignal(&n);	  
    owner.oneShot.signalAndClear(&n);//this is a signal
  }

  template<typename PT,typename PS,motor::MotorLevel L>
  InterpMode<PT,PS,L>::TargetCell::NotificationBuilder::NotificationBuilder(TargetCell& powner,intrp::NotificationType nt):owner(powner){
    n.notifyType=nt;
  }



 template<typename PT,typename PS,motor::MotorLevel L>
 std::string InterpMode<PT,PS,L>::TargetCell::getName() const{
   return "Target cell for joint "+toString(j);
 }

  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::TargetCell::getData(intrp::TargetProps<Position>& t){
    StackJob<Motors,TargetInfo,
      intrp::TargetProps<Position>*,
      onlyParam<intrp::TargetProps<Position>* >,
      &TargetInfo::getProps> 
    job(&im.info[j],&t);
  im.ms.waitJob(&job,Spool::HIGH);
  }
  
  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::TargetCell::setData(const intrp::Target<Position>& t){
    Job<Motors>* job=new AutoDeleteJob<Motors,
      TargetInfo,
      intrp::TargetInterface<Position>*,
      onlyParam<intrp::TargetInterface<Position>*>,
      &TargetInfo::setNew> (&im.info[j],t.dataPtr());      
  im.ms.addJob(job,Spool::HIGH);	 
  }//setData

  
  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::TargetCell::do_setData(typename InterpMode<PT,PS,L>::TargetCell::ReachParam& p){    
    oneShot.connect(p.first);
    im.info[j].setNew(p.second);
  }

  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::TargetCell::setData(intrp::TargetNotification<Position>& n,const intrp::Target<Position>& t){
    ValueWaiter<intrp::TargetNotification<Position>*,true,intrp::TargetNotification<Position> > val(&n);
    ReachParam p(&val,t.dataPtr());
    EhOnceJob<Motors,TargetCell,       
      ReachParam,
      onlyParam<ReachParam&>,
      &TargetCell::do_setData >
      job(this,p,val);
    im.ms.addJob(&job,Spool::HIGH);	
    val.wait();			
  }



   template<typename PT,typename PS,motor::MotorLevel L>
   void InterpMode<PT,PS,L>::TargetCell::do_connect(std::pair<Cell::CellConnection*,int>& p){
     intrp::spt::multipleConnect(onNewTarget.eventSignal,onTargetReached.eventSignal,im.info[j].onError,
			       *p.first,p.second);
   }
  

  
  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::TargetCell::connect(Cell::CellConnection& ref,const int& flags){
    StackJob<Motors,TargetCell,
      std::pair<Cell::CellConnection*,int>,
      onlyParam<std::pair<Cell::CellConnection*,int>&>,
      &TargetCell::do_connect> 
    job(this,std::pair<Cell::CellConnection*,int>(&ref,flags));
    im.ms.waitJob(&job,Spool::HIGH);
  }

  

  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::TargetCell::do_disconnect(Cell::CellConnection* ref){
    intrp::spt::multipleDisconnect(onNewTarget.eventSignal,onTargetReached.eventSignal,im.info[j].onError,*ref);    
  }

  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::TargetCell::disconnect(Cell::CellConnection& ref){
	StackJob<Motors,TargetCell,
      Cell::CellConnection*,
      onlyParam<Cell::CellConnection*>,
      &TargetCell::do_disconnect> job(this,&ref);
  im.ms.waitJob(&job,Spool::HIGH);
  }

  template<typename PT,typename PS,motor::MotorLevel L>
  InterpMode<PT,PS,L>::TargetCell::TargetCell(InterpMode& pim,int pj):im(pim),j(pj),onNewTarget(*this,intrp::NOTIFYREQUEST),onTargetReached(*this,intrp::NOTIFYREACHED){
    im.info[j].newTarget.connect(&onNewTarget);
    im.info[j].targetReached.connect(&onTargetReached);
  }


  template<typename PT,typename PS,motor::MotorLevel L>
  std::string InterpMode<PT,PS,L>::WholeTargetCell::getName() const{
    return "Whole target cell for motor id "+toString(im.getMotorModeId());
  }
  
  
  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::WholeTargetCell::do_getData(Array<intrp::TargetProps<Position> >* pos){
    for (size_t i=0;i<pos->size();++i){
      im.info[i].getProps(&(*pos)[i]);
    }      
  }

  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::WholeTargetCell::getData(Array<intrp::TargetProps<Position> >& pos){
	StackJob<Motors,WholeTargetCell,
      Array<intrp::TargetProps<Position> >*,
      onlyParam<Array<intrp::TargetProps<Position> > *>,	
      &WholeTargetCell::do_getData> job(this,&pos);
  im.ms.waitJob(&job,Spool::HIGH);
  }

  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::WholeTargetCell::do_setData(intrp::TargetInterface<Position>*  pos){
    for (std::set<int>::iterator it=im.controlledMotors.begin();it!=im.controlledMotors.end();++it){
      im.info[*it].setNew(pos);
    }//for 
  }//do_setData

  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::WholeTargetCell::setData(const intrp::Target<Position>& pos){
    Job<Motors>* job=new AutoDeleteJob<Motors,WholeTargetCell,
      intrp::TargetInterface<Position>*,
      onlyParam<intrp::TargetInterface<Position>*>,
      &WholeTargetCell::do_setData>
    (this,pos.dataPtr());
  im.ms.addJob(job,Spool::HIGH);
  
}

  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::WholeTargetCell::do_reachPosition(ReachParam& p){
    oneShot.connect(p.first);
    do_setData(p.second);	
  }
  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::WholeTargetCell::setData(int &n,const intrp::Target<Position>& pos){
    ValueWaiter<int,true> val(&n);
    EhOnceJob<Motors,WholeTargetCell,       
      ReachParam,
	  onlyParam<ReachParam&>,
      &WholeTargetCell::do_reachPosition >
    job(this,ReachParam(&val,pos.dataPtr()),val);
  im.ms.addJob(&job,Spool::HIGH);	
	val.wait();			
  }//setData

  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::WholeTargetCell::do_connect(std::pair<Cell::CellConnection*,int> &p){
    intrp::spt::multipleConnect(onRequest.eventSignal,onReached.eventSignal,im.anyError,
			*p.first,p.second);
      }//do_connect


  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::WholeTargetCell::connect(Cell::CellConnection& ref,const int& flags){
	StackJob<Motors,WholeTargetCell,
	  std::pair<Cell::CellConnection*,int>,
	  onlyParam<std::pair<Cell::CellConnection*,int>&>,
	  &WholeTargetCell::do_connect> 
	  job(this,std::pair<Cell::CellConnection*,int>(&ref,flags));
	im.ms.waitJob(&job,Spool::HIGH);
      }//connect

 
  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::WholeTargetCell::do_disconnect(Cell::CellConnection *c){
    intrp::spt::multipleDisconnect(onRequest.eventSignal,onReached.eventSignal,im.anyError,*c);
  }

  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::WholeTargetCell::disconnect(Cell::CellConnection& ref){
	StackJob<Motors,WholeTargetCell,
	  Cell::CellConnection*,
	  onlyParam<Cell::CellConnection*>,
	  &WholeTargetCell::do_disconnect> job(this,&ref);
      im.ms.waitJob(&job,Spool::HIGH);
    }

  template<typename PT,typename PS,motor::MotorLevel L>
  InterpMode<PT,PS,L>::WholeTargetCell::WholeTargetCell(InterpMode& pim):im(pim){
	im.onReaching.transitionToInactive.connect(this);
  }

  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::addMotor(Motors& m,int j,motor::ModeId prevMode){
    //std::cout<<"in addMotor"<<m<<std::endl;
    controlledMotors.insert(j);
    info[j].state=intrp::REACHED;
    try{
      tmAdapter::setCurrentPosition(m,j);
      m.hw->setPowerState(j,true);	
      tmAdapter::readPosition(info[j].currPos,m,j);
    }catch(exc::Exception & e){
      tmAdapter::assignZeroPosition(info[j].currPos,m,j);
      info[j].onError.handle(e);
      throw;
    }	 
  }

  template<typename PT,typename PS,motor::MotorLevel L>
  void InterpMode<PT,PS,L>::removeMotor(Motors& m,int j,motor::ModeId prevMode){	
    info[j].state=intrp::DISABLED;	
    info[j].currPos=Motor::INVALIDPOS;
    onReaching.erase(j);
    controlledMotors.erase(j);
  }

  
  template<typename PT,typename PS,motor::MotorLevel L>
  bool InterpMode<PT,PS,L>::operator()(Motors& m){
    //std::cout<<"motor loop"<<std::endl;
    Time now=getSystemMillis();      
    for (std::set<int>::const_iterator it=onReaching.getSet().begin();it!=onReaching.getSet().end();){
      int j=*it;
      it++; //we need to inc here, because erase may invalidate it
      try{
	
	info[j].updatePosture(m,now);
	tmAdapter::rotate(m,j,info[j].currPos);
	tmAdapter::readPosition(info[j].currPos,m,j);	  
      }catch(exc::Exception& e){	  
	info[j].onError.handle(e);	  
      }	
    }//for
    //std::cout<<"motor loop "<<(onReaching.size()==0?"end":"continue")<<std::endl;
    return onReaching.size()==0;
  }
  
  template<typename PT,typename PS,motor::MotorLevel L>
  motor::Interface::Position InterpMode<PT,PS,L>::getPosition(Motors& m,int j){
    switch (info[j].state){ 
    case intrp::REACHING:
      return motor::Interface::MOVINGPOS;
    case intrp::REACHED:
      return tmAdapter::normalize(info[j].currPos,m,j);      
    case intrp::DISABLED:
      return motor::Interface::INVALIDPOS;	
    }
    return motor::Interface::INVALIDPOS;	
  } 


   template<typename PT,typename PS,motor::MotorLevel L>
   void InterpMode<PT,PS,L>::connect(Cell::CellConnection& ref,int j){
     info[j].hlPosition.connect(ref);      
   }
 
  template<typename PT,typename PS,motor::MotorLevel L>
   void InterpMode<PT,PS,L>::disconnect(Cell::CellConnection& ref,int j){
      info[j].hlPosition.connect(ref);      
  }

  template<typename PT,typename PS,motor::MotorLevel L>
  InterpMode<PT,PS,L>::InterpMode(Spool& pms,int pn):MotorSpooler<PT,PS>::MotorMode(tmAdapter::modeId),
						     ms(pms),n(pn),info(n),targetCells(*this,n),wholeTarget(*this){
    onReaching.transitionToActive.connect(this);
    for (int j=0;j<n;j++){
      info[j].newTarget.connect(&onReaching.inserter);
      info[j].targetReached.connect(&onReaching.eraser);
      info[j].onError.connect(&anyError);
    }//for
  }//   InterpMode ctor  


  template<typename PT,typename PS,motor::MotorLevel L>
class DefDescr<InterpMode<PT,PS,L> >:public Description<InterpMode<PT,PS,L> > {  
public:
  DefDescr(InterpMode<PT,PS,L>& im):Description<InterpMode<PT,PS,L> >(im){
  }
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    hopname="motors";
    
    std::string name;
    switch(L){
    case motor::IFACELEVEL: 
      name="norm";
      break;
    case motor::RAWLEVEL: 
      name="raw";
      break;
    case motor::HWLEVEL: 
      name="hw";
      break;      
    }
    members.push_back(vectMapping(this->obj.targetCells,"motor",IDPath("target."+name,".")));
    members.push_back(scalarMapping(this->obj.wholeTarget,IDPath("target."+name,".")));
  }

};

    
  template<typename PT,typename PS,motor::MotorLevel L> template <intrp::NotificationType NT>
  const intrp::NotificationType InterpMode<PT,PS,L>::WholeTargetCell::NotificationBuilder<NT>::value; 

  template<typename PT,typename PS> const motor::ModeId intrp::spt::tmAdapter<PT,PS,motor::IFACELEVEL>::modeId="normInterp";
  template<typename PT,typename PS> const motor::ModeId intrp::spt::tmAdapter<PT,PS,motor::RAWLEVEL>::modeId="rawInterp";
  template<typename PT,typename PS> const motor::ModeId intrp::spt::tmAdapter<PT,PS,motor::HWLEVEL>::modeId="hwInterp";


}//helium
