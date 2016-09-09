/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.02.18
 * \version 0.2
 * Release_flags s
 */

#include <helium/util/printHex.h>

namespace helium{


  namespace motor{
    template<typename MotorSign> MotorAccess<MotorSign>::MotorAccess():
      high(&motor::Raw<MotorSign>::NOHARDWARE),
      raw(&motor::Raw<MotorSign>::NOHARDWARE),
      hw(&motor::Raw<MotorSign>::NOHARDWARE){
    }            
    
    
    template<typename MotorSign> void MotorAccess<MotorSign>::get(std::pair<motor::IfaceId*,motor::MotorLevel>& p){
      switch(p.second){
      case IFACELEVEL:
	*p.first=high->getIfaceId();
	break;
      case RAWLEVEL:
	*p.first=raw->getIfaceId();
	break;
      case HWLEVEL:
	*p.first=hw->getIfaceId();
	break;
      }
    }

    template<typename MotorSign>
    std::ostream& operator<<(std::ostream& o,const MotorAccess<MotorSign>& m){
      return o<<m.high->getIfaceId()<<" "<<m.raw->getIfaceId()<<" "<<m.hw->getIfaceId();
    }
    
  }//ns motor



   namespace spt{
    struct NoMotId{
      static const char* NOMOTMODEID;
    };
   }
  
  template<typename MotorSign> 
  typename MotorSpooler<MotorSign>::NormPosition MotorSpooler<MotorSign>::NoMotorMode::getPosition(Motors& m,int j){
    return motor::Interface::INVALIDPOS;
  }      

  template<typename MotorSign> 
  MotorSpooler<MotorSign>::NoMotorMode::NoMotorMode():MotorMode(spt::NoMotId::NOMOTMODEID){
  }

   
  template<typename MotorSign> template<motor::MotorLevel L> 
  void MotorSpooler<MotorSign>::IfaceManager<L>::do_registerIface(Motor* m){
    ifaces[m->getIfaceId()]=m;
    //std::cout<<"registerd interface "<<m->getIfaceId()<<" for level "<<toString(L)<<std::endl;
  }

  template<typename MotorSign> template<motor::MotorLevel L> 
  void MotorSpooler<MotorSign>::IfaceManager<L>::do_unregisterIface(Motor* m){
    ifaces.erase(m->getIfaceId());
  }
  
  template<typename MotorSign> template<motor::MotorLevel L> 
  void MotorSpooler<MotorSign>::IfaceManager<L>::throwIfaceError(const motor::IfaceId& id){
    std::stringstream ss;
    ss<<"Interface "+toString(id)+" is not avaliable for mode "+toString(L);
    ss<<":choose one of";
    for(typename IfaceMap::iterator it=ifaces.begin();it!=ifaces.end();++it){
      ss<<" "<<it->second->getIfaceId();
    }	
    throw exc::InvalidOperation(ss.str());
  }

  template<typename MotorSign> template<motor::MotorLevel L> 
  typename MotorSpooler<MotorSign>::template IfaceManager<L>::Motor* MotorSpooler<MotorSign>::IfaceManager<L>::get(const motor::IfaceId &id){
    typename IfaceMap::iterator it=ifaces.find(id);	
    if (it==ifaces.end()){
      throwIfaceError(id);
    }
    return it->second;
  }
  
  template<typename MotorSign> template<motor::MotorLevel L> 
  void MotorSpooler<MotorSign>::IfaceManager<L>::registerIface(typename MotorSpooler<MotorSign>::template IfaceManager<L>::Motor* m){
    StackJob<Motors,IfaceManager,
      Motor*,
	     onlyParam<Motor*>,
	     &IfaceManager::do_registerIface,true> job(this,m);
  owner.waitJob(&job,MotSpoolPriority::HIGH);
}
  
  template<typename MotorSign> template<motor::MotorLevel L> 
  void MotorSpooler<MotorSign>::IfaceManager<L>::unregisterIface(typename MotorSpooler<MotorSign>::template IfaceManager<L>::Motor* m){
    StackJob<Motors,IfaceManager,
      Motor*,
      onlyParam<Motor*>,
	     &IfaceManager::do_unregisterIface,true> job(this,m);
  owner.waitJob(&job,MotSpoolPriority::HIGH);
  }
  
  template<typename MotorSign> template<motor::MotorLevel L> 
  void MotorSpooler<MotorSign>::IfaceManager<L>::do_listIfaces(Array<motor::IfaceId>* a){
    a->resize(ifaces.size());
    int i=0;
    for (typename IfaceMap::iterator it=ifaces.begin();it!=ifaces.end();++it){
      (*a)[i]=it->first;
      ++i;
    }
  }
  
  namespace spt{
    

    template<typename MotorSign> struct manSelect<MotorSign,motor::IFACELEVEL>{
      inline static  typename MotorSpooler<MotorSign>:: template IfaceManager<motor::IFACELEVEL>& get(typename MotorSpooler<MotorSign>::IfaceManagerSet& ims){
	return ims.high;
      }
    };

    template<typename MotorSign> struct manSelect<MotorSign,motor::RAWLEVEL>{
     inline static  typename  MotorSpooler<MotorSign>::template IfaceManager<motor::RAWLEVEL>& get(typename MotorSpooler<MotorSign>::IfaceManagerSet& ims){
	return ims.raw;
      }
    };

    template<typename MotorSign> struct manSelect<MotorSign,motor::HWLEVEL>{

      inline static typename MotorSpooler<MotorSign>::template IfaceManager<motor::HWLEVEL>& get(typename MotorSpooler<MotorSign>::IfaceManagerSet& ims){
	return ims.hw;
      }
    };
  }

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::IfaceManagerSet::set(Motors& m,motor::MotorLevel l,const motor::IfaceId& id){    
    switch(l){
    case motor::IFACELEVEL:
      m.high=high.get(id);
      break;
    case motor::RAWLEVEL:
      m.raw=raw.get(id);
      break;
    case motor::HWLEVEL:
      m.hw=hw.get(id);
      break;
    }
    sel.first=l;
    sel.second=id;
    //std::cout<<"Motor configuration:"<<m<<std::endl;
    update(sel);
  }




 
  template<typename MotorSign> template<motor::MotorLevel L> 
 void MotorSpooler<MotorSign>::IfaceManagerSet::template registerIface(typename motor::motorTypes<MotorSign,L>::Motor* m){
    spt::manSelect<MotorSign,L>::get(*this).registerIface(m);
    /*if (update.listenerNum()>0){
      std::pair<Array<motor::IfaceId>, ids;
      pt::manSelect<MotorSign,L>::get(*this).listIfaces(&ids);

      update(ids);
      }*/
  }
  

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::IfaceManagerSet::do_registerAll(typename motor::Raw<MotorSign>::Hardware* hwif){
    high.do_registerIface(hwif);
    raw.do_registerIface(hwif);
    hw.do_registerIface(hwif);
  }



  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::IfaceManagerSet::registerAll(typename motor::Raw<MotorSign>::Hardware* hwif){
    high.registerIface(hwif);
    raw.registerIface(hwif);
    hw.registerIface(hwif);
  }

  
  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::IfaceManagerSet::do_listIfaces(std::pair<Array<motor::IfaceId>*,motor::MotorLevel>& p){
    switch(p.second){
    case motor::IFACELEVEL:
      high.do_listIfaces(p.first);
      break;
    case motor::RAWLEVEL:
      raw.do_listIfaces(p.first);
      break;
    case motor::HWLEVEL:
      hw.do_listIfaces(p.first);
      break;
    }
  }

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::do_getPosition(Motors& m,motor::Interface::Position& pos,int j){
    pos=modes[motorModes[j]]->getPosition(m,j);
  }

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::do_PositionConnect(Motors& m,Cell::CellConnection& c,int j){
    if (posConns[j].sig.listenerNum()==0){
      modes[motorModes[j]]->connect(posConns[j],j);
    }
    posConns[j].sig.connect(c);
  }

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::do_PositionDisconnect(Motors& m,Cell::CellConnection& c,int j){
    posConns[j].sig.disconnect(c);
    if (posConns[j].sig.listenerNum()==0){
      modes[motorModes[j]]->disconnect(posConns[j],j);
    }
  }

  
  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::do_getMode(typename motor::ModeId& mode,int j){    
    mode=motorModes[j];//.val            
  }


  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::throwMotModeError(motor::ModeId id){
    std::stringstream ss;
    ss<<"Mode "+toString(id)+" is not avaliable:choose one of";
    for(typename ModeMap::iterator it=modes.begin();it!=modes.end();++it){
      ss<<" "<<it->first;
    }	
    throw exc::InvalidOperation(ss.str());
  }


  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::do_setMotorMode(Motors& m, int j,const motor::ModeId& next){
    //std::cout<<"setting "<<j<<" to motor mode "<<next<<std::endl;

    typename ModeMap::iterator nextMode=modes.find(next);
    if (nextMode==modes.end()){
      throwMotModeError(next);
    }
    
    motor::ModeId prev=motorModes[j];

    if (prev==next) return;
    modes[prev]->removeMotor(m,j,next);//.val      
    modes[next]->addMotor(m,j,prev);//.val
    if (posConns[j].sig.listenerNum()>0){
      modes[prev]->disconnect(posConns[j],j);
      modes[next]->connect(posConns[j],j);      
      motor::Interface::Position prevPos=modes[prev]->getPosition(m,j);
      motor::Interface::Position nextPos=nextMode->second->getPosition(m,j);
      if (prevPos!=nextPos){
	posConns[j].sig(nextPos);
      }       
    }
    //motor::ModeId mo=motorModes[j];
    //std::cout<<    motorModes.size()<<"motorModes["<<j<<"]"<<mo<<"::"<<motorModes[j].listenerNum()<<"@"<<next<<std::endl;

    motorModes[j]=next;
  }//do_setMotorMode


  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::do_MotModeConnect(Motors& m,Cell::CellConnection& c,int j){
      motorModes[j].connect(c);
    }

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::do_MotModeDisconnect(Motors& m,Cell::CellConnection& c,int j){
      motorModes[j].disconnect(c);
    }

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::do_setAllMotorMode(Motors& m,const motor::ModeId& mm){    
    for (size_t i=0;i<motorModes.size();i++){
      do_setMotorMode(m,i,mm);
    }
  }


  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::do_setMultipleMotorMode(Motors& m,const HwIdList& l,const motor::ModeId& mode){    
    for (size_t i=0;i<l.ids.size();++i){
      do_setMotorMode(m,l.ids[i],mode);
    }
  }


  


  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::do_registerMode(MotorMode* mm){
      modes[mm->motorModeId]=mm;
    }

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::do_unregisterMode(Motors& m,MotorMode* mm){
    modes.erase(mm->motorModeId);
    for (int j=0;j<(int)motorModes.size();j++){
      if (motorModes[j]==mm->motorModeId){
	do_setMotorMode(m,j,MotorMode::NOMODE);
      }
    }
  }

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::do_setIface(const motor::IfaceSelection& isel){
    /*std::cout<<"setting the interface ";
    write(std::cout,isel);
    std::cout<<std::endl;*/
    interfaces.set(m,isel.first,isel.second);
  }


  
  template<typename MotorSign> 
  motor::ModeId MotorSpooler<MotorSign>::getMotorMode(int j){
    //std::cout<<"MS::getMotorMode"<<std::endl;
    motor::ModeId mode;
    typedef PairPasser<typename motor::ModeId&,int> Pair;
    StackJob<Motors,
	     MotorSpooler,
	     const Pair&,
	     onlyParam<Pair>,
	     &MotorSpooler::do_getMode,
	     true> 
      job(this,Pair(mode,j));
    
      this->waitJob(&job,MotSpoolPriority::HIGH);
      return mode;
  }

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::setMotorMode(typename motor::ModeId mode,int j){      
    typedef PairPasser<int,const typename motor::ModeId&> Pair;
    typedef PairStorage<int,typename motor::ModeId> PairStorage;
    Job<Motors>* job=new AutoDeleteJob<Motors,
				       MotorSpooler,
				       PairStorage,
				       Pair,
				       &MotorSpooler::do_setMotorMode,true,const Pair&> 
    (this,Pair(j,mode));    
    this->addJob(job,MotSpoolPriority::HIGH);
  }
  

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::setMotorMode(typename motor::ModeId mode){      
    //std::cout<<"setting mode {"<<mode<<"}"<<std::endl;
  Job<Motors>* job=new AutoDeleteJob<Motors,
				       MotorSpooler,
				       const typename motor::ModeId,
				       const typename motor::ModeId&,
				       &MotorSpooler::do_setAllMotorMode,true> 
      (this,mode);    
    this->addJob(job,MotSpoolPriority::HIGH);
  }


  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::setAndWaitMotorMode(typename motor::ModeId mode,int j){      
    typedef PairPasser<int,const typename motor::ModeId&> Pair;
    StackJob<Motors,
	     MotorSpooler,
	     const Pair&,
	     Pair,
	     &MotorSpooler::do_setMotorMode,true
	     > 
    job(this,Pair(j,mode));    
  this->waitJob(&job,MotSpoolPriority::HIGH);
  }


  /*class AutoPointPair{
    const HwIdList first;
    motor::ModeId second;
    
  public:
    AutoPointPair(const HwIdList& pfirst,const motor::ModeId& psecond):
      first(pfirst),second(psecond){
    }
    
    
    operator std::pair<const HwIdList*,const motor::ModeId*>(){
      return std::pair<const HwIdList*,const motor::ModeId*>(&first,&second);
    }
  };
  */

   template<typename MotorSign> 
   void MotorSpooler<MotorSign>::setMultipleMotorMode(const typename motor::ModeId& mode,const HwIdList& h){      
     typedef PairPasser<const HwIdList&,const motor::ModeId&> Pair;          
     typedef PairStorage<const HwIdList,const motor::ModeId> PairStorage;          

     Job<Motors>* job=new 
     
     AutoDeleteJob<Motors,
		   MotorSpooler,
		   PairStorage,
		   Pair ,
		   &MotorSpooler::do_setMultipleMotorMode,true,
		   const Pair&
		   > 
       (this,Pair(h,mode));    
     this->addJob(job,MotSpoolPriority::HIGH);
  }


   template<typename MotorSign> 
   void MotorSpooler<MotorSign>::setAndWaitMultipleMotorMode(const typename motor::ModeId& mode,const HwIdList& h){      
     typedef PairPasser<const HwIdList&,const motor::ModeId&> Pair;          
     
    StackJob<Motors,
	     MotorSpooler,
	     const Pair&, 
	     Pair ,
	     &MotorSpooler::do_setMultipleMotorMode,true
	     > 
      job(this,Pair(h,mode));    
  this->waitJob(&job,MotSpoolPriority::HIGH);
  }

  
  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::registerMode(typename MotorSpooler<MotorSign>::MotorMode* mm){
    StackJob<Motors,
      MotorSpooler,
      MotorMode*,
      onlyParam<MotorMode*>,
	     &MotorSpooler::do_registerMode,true
	     > 
    job(this,mm);
  this->waitJob(&job,MotSpoolPriority::HIGH);
  
  }

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::unregisterMode(typename MotorSpooler<MotorSign>::MotorMode* mm){
    StackJob<Motors,
      MotorSpooler,
      MotorMode*,
	     MotorMode*,&MotorSpooler::do_unregisterMode,true> job(this,mm);
    this->waitJob(&job,MotSpoolPriority::HIGH);
  }

template<typename MotorSign> 
void MotorSpooler<MotorSign>::setInterface(const motor::IfaceSelection& isel){  
  StackJob<Motors,MotorSpooler,
    const motor::IfaceSelection&,
    onlyParam<const motor::IfaceSelection&>,
	   &MotorSpooler::do_setIface,true> job(this,isel);
  this->waitJob(&job,MotSpoolPriority::HIGH);
};



  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::setInterface(const motor::IfaceId& iface){  
    
    typedef StackJob<Motors,MotorSpooler,
		     const motor::IfaceSelection&,
		     onlyParam<const motor::IfaceSelection&>,
		     &MotorSpooler::do_setIface,true> IfaceJob;

    
    motor::IfaceSelection isel(motor::IFACELEVEL,iface);
    

    IfaceJob job(this,isel);      
    this->waitJob(&job,MotSpoolPriority::HIGH);

    isel.first=motor::RAWLEVEL;
    this->waitJob(&job,MotSpoolPriority::HIGH);

    isel.first=motor::HWLEVEL;
    this->waitJob(&job,MotSpoolPriority::HIGH);           
  };




  template<typename MotorSign> 
  std::string MotorSpooler<MotorSign>::MotorPositionCell::getName() const{
    return "MotorPosition cell for joint "+toString(j);
  }



  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::getPosition(motor::Interface::Position& p,int j){
    typedef PairPasser<motor::Interface::Position&,int> Pair;
    StackJob<Motors,MotorSpooler,
	     const Pair&,
	     Pair,
	     &MotorSpooler::do_getPosition,true> 
    job(this,Pair(p,j));
    this->waitJob(&job,MotSpoolPriority::HIGH);
  }
  

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::MotorPositionCell::getData(motor::Interface::Position& p){
    owner.getPosition(p,j);    
  }

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::positionConnect(Cell::CellConnection& c,int j){
    typedef PairPasser<Cell::CellConnection&,int> Pair;    
    StackJob<Motors,MotorSpooler,
	     const Pair&,
	     Pair,
	     &MotorSpooler::do_PositionConnect,true> 
      job(this,Pair(c,j));
   this->waitJob(&job,MotSpoolPriority::HIGH);
  }

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::MotorPositionCell::connect(Cell::CellConnection& c){	
    owner.positionConnect(c,j);
  }


  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::positionDisconnect(Cell::CellConnection& c,int j){
    typedef PairPasser<Cell::CellConnection&,int> Pair;
    StackJob<Motors,MotorSpooler,
	     const Pair&,
	     Pair,
	     &MotorSpooler::do_PositionDisconnect,true> 
      job(this,Pair(c,j));
    this->waitJob(&job,MotSpoolPriority::HIGH);
  }
  
  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::MotorPositionCell::disconnect(Cell::CellConnection& c){	
    owner.positionDisconnect(c,j);
  }
  

  

  template<typename MotorSign> 
  std::string MotorSpooler<MotorSign>::MotorExactPositionCell::getName() const{
    return "MotorExactPosition cell for joint "+toString(j);
  }
  
  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::MotorExactPositionCell::getData(motor::Interface::Position& p){
    owner.getPosition(p,j);
  }


  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::MotorExactPositionCell::connect(Cell::CellConnection& c){
    owner.positionConnect(c,j);
  }

  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::MotorExactPositionCell::disconnect(Cell::CellConnection& c){
    owner.positionDisconnect(c,j);
  }

  
  template<typename MotorSign> 
  void MotorSpooler<MotorSign>::MotorExactPositionCell::formatSignal(std::ostream& o,CellSignalData data){
    rawHexWrite(o,*trust_cast<const typename motor::ExactPositionCellSignature::UT*>(data));
  }

  template<typename MotorSign> 
void MotorSpooler<MotorSign>::MotorExactPositionCell::streamGet(std::ostream& o){
  typename   motor::ExactPositionCellSignature::GT g;
  this->getData(g);
  rawHexWrite(o,g);
  }

   template<typename MotorSign> 
   std::string MotorSpooler<MotorSign>::MotorModeCell::getName() const{
     return "MotorMode cell for joint "+toString(j);
   }

  template<typename MotorSign>
  void MotorSpooler<MotorSign>::MotorModeCell::getData(typename motor::ModeId& m){
    //std::cout<<"MMCEll::getting data"<<std::endl;
    m=owner.getMotorMode(j);
  }
  
  template<typename MotorSign>
  void MotorSpooler<MotorSign>::MotorModeCell::setData(const typename  motor::ModeId& m){
    owner.setMotorMode(m,j);
  }

  template<typename MotorSign>
  void MotorSpooler<MotorSign>::MotorModeCell::setData(StdReply& r,const typename motor::ModeId& m){
    owner.setAndWaitMotorMode(m,j);  
  }

  template<typename MotorSign>
  void MotorSpooler<MotorSign>::MotorModeCell::connect(Cell::CellConnection& ref){
    typedef PairPasser<Cell::CellConnection&,int> Pair;
    StackJob<Motors,
	     MotorSpooler,
	     const Pair&,
	     Pair,
	     &MotorSpooler::do_MotModeConnect,true> job(&owner,Pair(ref,j));
	owner.waitJob(&job,MotSpoolPriority::HIGH);	  
  }

  template<typename MotorSign>
  void MotorSpooler<MotorSign>::MotorModeCell::disconnect(Cell::CellConnection& ref){
    typedef PairPasser<Cell::CellConnection&,int> Pair;
    StackJob<Motors,
	     MotorSpooler,
	     const Pair&,
	     Pair,
	     &MotorSpooler::do_MotModeDisconnect,true> 
      job(&owner,Pair(ref,j));
    owner.waitJob(&job,MotSpoolPriority::HIGH);	  
  }

  template<typename MotorSign>
  MotorSpooler<MotorSign>::MotorModeCell::MotorModeCell(MotorSpooler& powner,int pj):owner(powner),j(pj){
  }



  template<typename MotorSign> 
  MotorSpooler<MotorSign>::MultipleMotorModeCell::MultipleMotorModeCell( MotorSpooler<MotorSign>& powner):owner(powner){
  }

  template<typename MotorSign> 
  std::string MotorSpooler<MotorSign>::MultipleMotorModeCell::getName() const{
    return "Multiple motorMode cell";
  }


   template<typename MotorSign>
   void MotorSpooler<MotorSign>::MultipleMotorModeCell::setData(const std::pair<HwIdList,motor::ModeId>& m){
     owner.setMultipleMotorMode(m.second,m.first);  
  }

   template<typename MotorSign>
   void MotorSpooler<MotorSign>::MultipleMotorModeCell::setData(StdReply& r,const std::pair<HwIdList,motor::ModeId>& m){
     owner.setAndWaitMultipleMotorMode(m.second,m.first);  
  }


  template<typename MotorSign>
  std::string MotorSpooler<MotorSign>::MotorInterfaceCell::getName() const {
    return "Motor Interface cell"; 
  }
  
  template<typename MotorSign>
  void MotorSpooler<MotorSign>::MotorInterfaceCell::getData(motor::IfaceId& id,const motor::MotorLevel& l){
    StackJob<Motors,Motors,
      std::pair<motor::IfaceId*,motor::MotorLevel>,
      onlyParam<std::pair<motor::IfaceId*,motor::MotorLevel>&>,
	     &Motors::get,true> job(&ms.m,std::pair<motor::IfaceId*,motor::MotorLevel>(&id,l));
    ms.waitJob(&job,MotSpoolPriority::HIGH);
  }

  template<typename MotorSign>
  void MotorSpooler<MotorSign>::MotorInterfaceCell::setData(const motor::IfaceSelection& isel){
    Job<Motors>* job=new AutoDeleteJob<Motors,MotorSpooler,
      motor::IfaceSelection, //we need to save it,it's an autodelete job
      onlyParam<const motor::IfaceSelection&>,
				       &MotorSpooler::do_setIface,true>(&ms,isel);
    ms.addJob(job,MotSpoolPriority::HIGH);
  }


  template<typename MotorSign>
  void MotorSpooler<MotorSign>::MotorInterfaceCell::setData(StdReply& r,const motor::IfaceSelection& isel){
    StackJob<Motors,MotorSpooler,
      const motor::IfaceSelection&,
      onlyParam<const motor::IfaceSelection&>,
	     &MotorSpooler::do_setIface,true> job(&ms,isel);
    ms.waitJob(&job,MotSpoolPriority::HIGH);
  }

  template<typename MotorSign>
  void MotorSpooler<MotorSign>::MotorInterfaceCell::connect(Cell::CellConnection& c){
    ms.interfaces.update.connect(c);
  }

  template<typename MotorSign>
  void MotorSpooler<MotorSign>::MotorInterfaceCell::disconnect(Cell::CellConnection& c){
    ms.interfaces.update.disconnect(c);
  }

  template<typename MotorSign>
  MotorSpooler<MotorSign>::MotorInterfaceCell::MotorInterfaceCell(MotorSpooler& pms):ms(pms){
  }
  
  template<typename MotorSign>
  std::string MotorSpooler<MotorSign>::MotorInterfaceListCell::getName() const{
    return "MotorInterfacesListCell";
  }

  template<typename MotorSign>
  void MotorSpooler<MotorSign>::MotorInterfaceListCell::getData(Array<motor::IfaceId>& a,const motor::MotorLevel& l){
    typedef std::pair<Array<motor::IfaceId>*,motor::MotorLevel> LParam;
    StackJob<Motors,IfaceManagerSet,
      LParam,
      onlyParam<LParam&>,
	     &IfaceManagerSet::do_listIfaces,true> job(&ms.interfaces,LParam(&a,l));
    ms.waitJob(&job,MotSpoolPriority::HIGH);
  }

  template<typename MotorSign>
  MotorSpooler<MotorSign>::MotorInterfaceListCell::MotorInterfaceListCell(MotorSpooler& pms):ms(pms){    
  }

  template<typename MotorSign>
  MotorSpooler<MotorSign>::MotorSpooler(int n,bool autoStart):Spooler<Motors>(m,MotSpoolPriority::PRIORITIES),
							      motorModes(n),posConns(n),modeCells(*this,n),multipleCell(*this),posCells(*this,n),eposCells(*this,n), ifaceCell(*this),ifaceListCell(*this),interfaces(*this){
    do_registerMode(&NOMOTORMODE);
    setAll(motorModes,NOMOTORMODE.getMotorModeId());      
    interfaces.do_registerAll(&motor::Raw<MotorSign>::NOHARDWARE);     
    if (autoStart){
      this->start();
    }
  }

  template<typename MotorSign>
  size_t  MotorSpooler<MotorSign>::getMotorNum(){
    return motorModes.size();
  }
  
  template<typename MotorSign>
  class DefDescr<MotorSpooler<MotorSign> >:public Description<MotorSpooler<MotorSign> > {  
  public:  
    DefDescr(MotorSpooler<MotorSign>& ms):Description<MotorSpooler<MotorSign> >(ms){
    }
    
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      hopname="motors";
      //std::cout<<"the ms is being MAPPED"<<std::endl;
      members.push_back(scalarMapping(this->obj.ifaceCell,"interfaces"));
      members.push_back(vectMapping(this->obj.modeCells,"motor","mode"));
      members.push_back(vectMapping(this->obj.posCells,"motor","position"));
      members.push_back(vectMapping(this->obj.eposCells,"motor",IDPath("position.exact",".")));
      members.push_back(scalarMapping(this->obj.multipleCell,"mode"));
    }
    
  };
  

  
  template<typename MotorSign> typename MotorSpooler<MotorSign>::NoMotorMode MotorSpooler<MotorSign>::NOMOTORMODE;
  

    /*void setWholeMotorMode(const Array<typename motor::ModeId>& mode){
      Job<Motors>* job= new AutoDeleteJob<Motors,
	MotorSpooler,
	const Array<typename MotorMode::ModeId>*,
	const Array<typename MotorMode::ModeId>*,
	&MotorSpooler::do_setWholeMotorMode> job(this,&mode);     
      this->addJob(&job,HIGH);
    
    }


 
    void setAndWaitWholeMotorMode(const Array<typename motor::ModeId>& mode){
      StackJob<Motors,
	MotorSpooler,
	const Array<typename MotorMode::ModeId>*,
	const Array<typename MotorMode::ModeId>*,
	&MotorSpooler::do_setWholeMotorMode> job(this,&mode);     
	this->waitJob(&job,HIGH);
    
	}*/

}
