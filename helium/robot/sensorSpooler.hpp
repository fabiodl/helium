/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

namespace helium{

template<typename T>
SensorSpooler<T>::SensorSpooler(Sensor& psensor,bool autoStart):Spooler<sensor::Set<T> >(psensor,SensSpoolPriority::PRIORITIES){
      if (autoStart){
	this->start();
      }
    }           


template<typename T>
 size_t SensorSpooler<T>::size(){
  return this->getResource().size();
}

 template<typename T>
 void SensorNotifier<T>::handle(const HwIdList& bad,exc::Exception& e,const T* data){
   if (isAffected(bad)){
     eh.handle(e);
   }else{
     (*this)(data);
   }
 }

template<typename T>
SensorNotifier<T>::SensorNotifier(sensor::ReadType ptype,exc::ExceptionHandler& peh):ConnReference<const T*>(this),type(ptype),eh(peh){
}


template<typename T> 
void SensorNotifierSignal<T>::connect(SensorNotifier<T>& sn){ //ONLY this type of connection is allowed
  SignalType<const T*,true>::value::connect(sn);
}

template<typename T> 
void SensorNotifierSignal<T>::operator()(const T* d,const HwIdList& read){
      this->m.lock();
      try{
	for (typename std::list<Callback<const T*>*>::iterator i=
	       this->cs.begin();i!=this->cs.end();i++){
	  if (static_cast<SensorNotifier<T>*>(*i)->canSignal(read)){
	    (*(*i))(d); 
	  }
	}      
      }catch(...){
	this->m.unlock();
	throw;
      }
      this->m.unlock();
    }

template<typename T> 
void SensorNotifierSignal<T>::handle(const HwIdList& bad,exc::Exception& e,const T* data){

      exc::SensorGroupException<T> sge(e,data,bad);

      for (typename std::list<Callback<const T*>* >::iterator i=this->cs.begin();i!=this->cs.end();i++){
	static_cast<SensorNotifier<T>*>(*i)->handle(bad,sge,data);
      }
    }


template<typename T>
bool MultipleSensorNotifier<T>::isAffected(const HwIdList& bad){
  return intersects(bad.ids,what.ids);
}

template<typename T>
bool MultipleSensorNotifier<T>::canSignal(const HwIdList& read){
  return isIncluded(what.ids,read.ids);
}


template<typename T>
MultipleSensorNotifier<T>::MultipleSensorNotifier(sensor::ReadType ptype,const HwIdList& pwhat,exc::ExceptionHandler& eh):SensorNotifier<T>(ptype,eh),what(pwhat){
    }

template<typename T>
MultipleSensorNotifier<T>::MultipleSensorNotifier(sensor::ReadType ptype,int n,exc::ExceptionHandler& eh):SensorNotifier<T>(ptype,eh),what(n){
  what.fillConsecutive();
}


template<typename T>
void SingleSensorNotifier<T>::operator()(const T* t){
  (*cb)(&t[idx]);
}

template<typename T>
bool SingleSensorNotifier<T>::canSignal(const HwIdList& read){
  return contains(read.ids,idx);
}

template<typename T>
bool SingleSensorNotifier<T>::isAffected(const HwIdList& bad){
      for (size_t i=0;i<bad.ids.size();i++){
	if (bad.ids[i]==idx) return true;
      }
      return false;
    }

template<typename T>
SingleSensorNotifier<T>::SingleSensorNotifier(sensor::ReadType ptype,int pidx,Cell::CellCallback* pcb,exc::ExceptionHandler& eh):
      SensorNotifier<T>(ptype,eh),
      idx(pidx),cb(pcb){
    }

  
template<typename T>
void SingleSensorChangeNotifier<T>::operator()(const T* t){
  if (t[this->idx]!=prevVal||toUpdate){
    (*this->cb)(&t[this->idx]);
    toUpdate=false;
  }
  prevVal=t[this->idx];

}


template<typename T>
SingleSensorChangeNotifier<T>::SingleSensorChangeNotifier(sensor::ReadType ptype,int pidx,Cell::CellCallback* pcb,exc::ExceptionHandler& eh):
  SingleSensorNotifier<T>(ptype,pidx,pcb,eh),toUpdate(true){
}
  

template<typename T>
bool TypedSingleSensorNotifier<T>::canSignal(const HwIdList& read){
  return contains(read.ids,idx);
}

template<typename T>
void TypedSingleSensorNotifier<T>::operator()(const T* t){
  (*cb)(t[idx]);
}

template<typename T>
bool TypedSingleSensorNotifier<T>::isAffected(HwIdList& bad){
      for (size_t i=0;i<bad.ids.size();i++){
	if (bad.ids[i]==idx) return true;
      }
      return false;
    }

template<typename T>
TypedSingleSensorNotifier<T>::TypedSingleSensorNotifier(sensor::ReadType ptype,int pidx,Callback<T>* pcb,exc::ExceptionHandler& eh):
  SensorNotifier<T>(ptype,eh),
  idx(pidx),cb(pcb){
}

  template<typename T>
  void TypedSingleSensorChangeNotifier<T>::operator()(const T* t){
    if (t[this->idx]!=prevVal||toUpdate){
      (*this->cb)(t[this->idx]);
      toUpdate=false;
    }
    prevVal=t[this->idx];
  }
  

  template<typename T>
TypedSingleSensorChangeNotifier<T>::TypedSingleSensorChangeNotifier(sensor::ReadType ptype,int pidx,Callback<T>* pcb,exc::ExceptionHandler& eh):
  TypedSingleSensorNotifier<T>(ptype,pidx,pcb,eh),toUpdate(true){
  }
  
  

 template<typename T>
 void GroupSensorNotifier<T>::operator()(const T* t){
   for (size_t i=0;i<this->what.ids.size();i++){
     g.vals[i]=t[this->what.ids[i]];
   }
   (*cb)(&g);
 }


template<typename T>
 GroupSensorNotifier<T>::GroupSensorNotifier(sensor::ReadType ptype,const HwIdList& pwhat,Cell::CellCallback* pcb,exc::ExceptionHandler& eh):
      MultipleSensorNotifier<T>(ptype,pwhat,eh),
      g(pwhat.ids.size()),cb(pcb){
      //std::cout<<"new notifier,"<< pwhat.ids.size()<<std::endl;
    }    



 template<typename T>
 void GroupSensorChangeNotifier<T>::operator()(const T* t){   
   for (size_t i=0;i<this->what.ids.size();i++){
     if (g.vals[i]!=t[this->what.ids[i]]){
       g.vals[i]=t[this->what.ids[i]];
       toUpdate=true;
     }
   }
   
   if (toUpdate){
     (*cb)(&g);
     toUpdate=false;
   }   
 }


template<typename T>
GroupSensorChangeNotifier<T>::GroupSensorChangeNotifier(sensor::ReadType ptype,const HwIdList& pwhat,Cell::CellCallback* pcb,exc::ExceptionHandler& eh):
  MultipleSensorNotifier<T>(ptype,pwhat,eh), g(pwhat.ids.size()),cb(pcb),toUpdate(true){
}



template<typename T>
void WholeSensorNotifier<T>::operator()(const T* t){
  assign(g.vals,t);
  (*cb)(&g);
}

template<typename T>
bool WholeSensorNotifier<T>::isAffected(const HwIdList& bad){
  return true;
}


template<typename T>
WholeSensorNotifier<T>::WholeSensorNotifier(sensor::ReadType ptype,int n,Cell::CellCallback* pcb,exc::ExceptionHandler& eh):
  MultipleSensorNotifier<T>(ptype,n,eh),
  g(n),cb(pcb){
}    


template<typename T>
void WholeSensorChangeNotifier<T>::operator()(const T* t){
  for (size_t i=0;i<g.vals.size();i++){
    if (g.vals[i]!=t[i]){
      toUpdate=true;
      break;
    }
  }
  if (toUpdate){
    assign(g.vals,t);
    (*cb)(&g);
    toUpdate=false;
  }
}

template<typename T>
bool WholeSensorChangeNotifier<T>::isAffected(const HwIdList& bad){
  return true;
}

template<typename T>
WholeSensorChangeNotifier<T>::WholeSensorChangeNotifier(sensor::ReadType ptype,int n,Cell::CellCallback* pcb,exc::ExceptionHandler& eh):
  MultipleSensorNotifier<T>(ptype,n,eh),g(n),cb(pcb),toUpdate(true){
}    



template<typename T>
void TypedGroupSensorNotifier<T>::operator()(const T* t){
  (*cb)(t);
}

template<typename T>
TypedGroupSensorNotifier<T>::TypedGroupSensorNotifier(sensor::ReadType ptype,const HwIdList& pwhat,Callback<const T*> *pcb,exc::ExceptionHandler& eh):
  MultipleSensorNotifier<T>(ptype,pwhat,eh),
  cb(pcb){  
  }    



template<typename T>
void TypedGroupSensorChangeNotifier<T>::operator()(const T* t){  
  for (size_t i=0;i<this->what.ids.size();i++){
    int idx=this->what.ids[i];
    if (prevVal[i]!=t[idx]){
      prevVal[i]=t[idx];
      toUpdate=true;//we do not break,we want to store all the prevVals
    }
  }
  if (toUpdate){
    (*cb)(t);
    toUpdate=false;
  }
}

template<typename T>
TypedGroupSensorChangeNotifier<T>::TypedGroupSensorChangeNotifier(sensor::ReadType ptype,const HwIdList& pwhat,Callback<const T*> *pcb,exc::ExceptionHandler& eh):
  MultipleSensorNotifier<T>(ptype,pwhat,eh),
  cb(pcb),prevVal(pwhat.ids.size()),toUpdate(true){  
  }    



  /*
template<typename T>
void SensorProcessor<T>::ReadWork::firstRead(sensor::Set<T>& s){
  typename Pipe::Node* n=owner.pipe.getWriterNode();
  try{
    s.rawRead((**n).vals.data,readToken);
    (**n).readData.assure(toRead);
  }catch(exc::Exception& e){
    (**n).exc.handle(e);	 
    owner.pipe.remitWriterNode(n);
    throw;
  }
  owner.pipe.remitWriterNode(n);
}
  */

template<typename T>
bool SensorProcessor<T>::ReadWork::operator()(sensor::Set<T>& s){
  //std::cout<<"the set"<<std::endl;
  if (jobState.mustExit()){
    return true;
  }
  //std::cout<<"has work"<<std::endl;
  typename Pipe::Node* n=owner.pipe.getWriterNode();
  try{
    s.rawRead((**n).vals.data,readToken);
    (**n).readData.assure(toRead);
  }catch(exc::Exception& e){
    (**n).exc.handle(e);	 
    owner.pipe.remitWriterNode(n);
    throw;
  }
  owner.pipe.remitWriterNode(n);
  //std::cout<<"ok,let's return"<<std::endl;
  return false;
}

template<typename T>
bool SensorProcessor<T>::ReadWork::isToCancel(){
  return jobState.mustExit();
}





template<typename T>
void SensorProcessor<T>::ReadWork::update(sensor::Set<T>& s){
  toRead=new HwIdList();
  for(size_t i=0;i<listeners.size();i++){
    if (listeners[i]>0){
      toRead->ids.insert(toRead->ids.end(),i);
    }
  }
  bool hasWork=toRead->ids.size()>0;
  jobState.setRunning(owner.spool,
		      this,
		      priority,
		      hasWork);
  if (hasWork){
    s.getToken(readToken,*toRead);
  }
}

template<typename T>
void SensorProcessor<T>::ReadWork::mark(sensor::Set<T>& s,int idx){
  bool toUpdate=(listeners[idx]==0);
  listeners[idx]++;	
  if (toUpdate){
    update(s);
  }
}

template<typename T>
void SensorProcessor<T>::ReadWork::mark(sensor::Set<T>& s,std::vector<int>& ids){
	bool toUpdate=false;
	for (size_t i=0;i<ids.size();i++){
	  toUpdate=toUpdate||(listeners[ids[i]]==0);
	  listeners[ids[i]]++;
	}
	if (toUpdate){
	  update(s);
	}
      }
      

template<typename T>
      void SensorProcessor<T>::ReadWork::unmark(sensor::Set<T>& s,int idx){
	listeners[idx]--;
	if (listeners[idx]==0){
	  update(s);
	}
      }

template<typename T>
      void SensorProcessor<T>::ReadWork::unmark(sensor::Set<T>& s,std::vector<int>& ids){
	bool toUpdate=false;
	for (size_t i=0;i<ids.size();i++){
	  listeners[ids[i]]--;
	  if (listeners[ids[i]]==0){
	    toUpdate=true;
	  }
	}
	if (toUpdate) update(s);
      }

template<typename T>
      SensorProcessor<T>::ReadWork::ReadWork(SensorProcessor& powner,int n,typename SensSpoolPriority::Priority ppriority):
	owner(powner),priority(ppriority),
	listeners(n){
	zero(listeners);
      }

  
  
  template<typename T> template<bool change>
  std::string SensorProcessor<T>::SensorCell<change>:: getName() const {
    return stringBegin()<<"sensor "<<idx<<(change?"change":"")<<" cell"<<stringEnd();
  }
  
  template<typename T>  template<bool change>
  void SensorProcessor<T>::SensorCell<change>::do_read(Sensor& s,T& t){	
    t=s.rawRead(idx);
  }
  


  template<typename T>  template<bool change>
  void SensorProcessor<T>::SensorCell<change>::do_connect(sensor::Set<T>& s,SensorNotifier<T>* n){

    owner.toNotify.connect(*n);
    owner.reads[n->type]->mark(s,idx);
  }


template<typename T> template<bool change>
void SensorProcessor<T>::SensorCell<change>::do_disconnect(sensor::Set<T>& s,SensorNotifier<T>* n){
  
	owner.toNotify.disconnect(*n);
	owner.reads[n->type]->unmark(s,idx);	
      }
  
      
  template<typename T> template<bool change>
  void SensorProcessor<T>::SensorCell<change>::anyConnect(SensorNotifier<T>* n,DisconnectInfo* &disc){
    StackJob<Sensor,
	     SensorCell,
	     SensorNotifier<T>*,
	     SensorNotifier<T>*,
	     &SensorCell::do_connect,true> job(this,n);
  owner.spool.waitJob(&job,SensSpoolPriority::HIGH);
  disc=n;
}


template<typename T> template<bool change>
void SensorProcessor<T>::SensorCell<change>::anyDisconnect(DisconnectInfo* &disc){
	SensorNotifier<T>* n=trust_cast<SensorNotifier<T>*>(disc);
	StackJob<Sensor,
	  SensorCell,
	  SensorNotifier<T>*,
	  SensorNotifier<T>*,
		 &SensorCell::do_disconnect,true> job(this,n);
	owner.spool.waitJob(&job,SensSpoolPriority::HIGH);
	delete disc;
	disc=NULL;
      }


  //Cell
  template<typename T> template<bool change>
  void SensorProcessor<T>::SensorCell<change>::connect(Cell::CellConnection& ref,const sensor::ReadType& t){
    anyConnect(new typename selectFirstOnTrue<
		 SingleSensorChangeNotifier<T>,
		 SingleSensorNotifier<T>,
		 change>::value	       
	       (t,idx,ref.cb,ref),
	       ref.disc);
  }
  
  template<typename T> template<bool change>
  void SensorProcessor<T>::SensorCell<change>::connect(Cell::CellConnection& ref){
    connect(ref,sensor::READ);
  }
  
  
  template<typename T> template<bool change>
  void SensorProcessor<T>::SensorCell<change>::disconnect(Cell::CellConnection& ref){
    anyDisconnect(ref.disc);
  }

  //Typed
  
  template<typename T> template<bool change>
  void SensorProcessor<T>::SensorCell<change>::getData(T& t){	
    StackJob<Sensor,
	     SensorCell,
	     T&,
	     T&,
	     &SensorCell::do_read, //we use this instead of the sp one because we do not need to pass the int either
	     true,
	     T&> job(this,t);       
    owner.spool.waitJob(&job,SensSpoolPriority::HIGH);
  }
  
  template<typename T> template<bool change>
  void SensorProcessor<T>::SensorCell<change>::connect(EhConnReference<T>& ref,const sensor::ReadType& t){       	
    anyConnect(new 
	       typename selectFirstOnTrue<
	       TypedSingleSensorChangeNotifier<T>,
	       TypedSingleSensorNotifier<T>,
	       change>::value
	       (t,idx,ref.cb,ref),
	       ref.disc);
  }
  
  template<typename T> template<bool change>
  void SensorProcessor<T>::SensorCell<change>::disconnect(EhConnReference<T>& ref){
    anyDisconnect(ref.disc);
  }
  
  template<typename T> template<bool change>
  SensorProcessor<T>::SensorCell<change>::SensorCell(SensorProcessor& powner,int pidx):owner(powner),idx(pidx){
  }
  


  template<typename T> template<bool change>
  std::string  SensorProcessor<T>::SensorGroupCell<change>::getName() const {
    return change?"sensor group change cell":"sensor group cell";
  }
  



  template<typename T> template<bool change>
  void SensorProcessor<T>::SensorGroupCell<change>::getData(SensorValList<T>& g){ 
    g.vals.resize(owner.spool.size());
    owner.read(g.vals.data);
}

template<typename T> template<bool change>
void SensorProcessor<T>::SensorGroupCell<change>::getData(T* t){  
  owner.read(t);
}


template<typename T>  template<bool change>
void SensorProcessor<T>::SensorGroupCell<change>::getData(SensorValList<T>& g,const HwIdList& l){
  owner.read(g,l);
}

template<typename T> template<bool change>
void SensorProcessor<T>::SensorGroupCell<change>::getData(T* g,const HwIdList& l){
  owner.read(g,l);
}


template<typename T> template<bool change>
void SensorProcessor<T>::SensorGroupCell<change>::connect(Cell::CellConnection& ref,const std::pair<HwIdList,sensor::ReadType>& t){
  owner.anyConnect(new 
		   typename selectFirstOnTrue<
		   GroupSensorChangeNotifier<T>,
		   GroupSensorNotifier<T>,
		   change>::value		   		   
		   (t.second,t.first,ref.cb,ref),
		   ref.disc);
}
  
  template<typename T> template<bool change>
  void SensorProcessor<T>::SensorGroupCell<change>::connect(Cell::CellConnection& ref){
    owner.anyConnect(new 
		     typename selectFirstOnTrue<
		     WholeSensorChangeNotifier<T>,
		     WholeSensorNotifier<T>,
		     change>::value		     
		     (sensor::READ,owner.spool.size(),ref.cb,ref),ref.disc);
  }
  
  template<typename T> template<bool change>
  void SensorProcessor<T>::SensorGroupCell<change>::disconnect(Cell::CellConnection& ref){
    owner.anyDisconnect(ref.disc);
  }
  
  
template<typename T> template<bool change>
      void SensorProcessor<T>::SensorGroupCell<change>::connect(EhConnReference<const T*>& ref,const HwIdList& what, sensor::ReadType type){
  owner.anyConnect(new TypedGroupSensorNotifier<T>(type,what,ref.cb,ref),
		   ref.disc);
}
  
  template<typename T> template<bool change>
  void SensorProcessor<T>::SensorGroupCell<change>::disconnect(EhConnReference<const T*>& ref){
    owner.anyDisconnect(ref.disc);
  }

  template<typename T> template<bool change>
  SensorProcessor<T>::SensorGroupCell<change>::SensorGroupCell(SensorProcessor& powner):owner(powner){
  }


  
  
  template<typename T> 
  template<typename C,void (C::*f)()> void SensorProcessor<T>::execAndHandle(C* c,T* data){
    try{
      (c->*f)();
    }catch(exc::HardwareDeviceException<UNNAMEDDEVICE>& e){
      HwIdList bad(spool.size());
      bad.fillConsecutive();
      toNotify.handle(bad,e,data);	
    }catch(exc::HardwareDeviceException<NAMEDDEVICE>& e){
      HwIdList bad;
      bad.ids.push_back(e.id);
      toNotify.handle(bad,e,data);
    }catch(exc::HardwareDeviceException<MULTIPLEXINGDEVICE>& e){
      toNotify.handle(e.affected,e,data);
    }catch(exc::HardwareDeviceException<NAMEDMULTIPLEXINGDEVICE>& e){
      toNotify.handle(e.affected,e,data);
    }catch(exc::MultipleException& e){	
      exc::MultipleExceptionIterator mei(e);	
      for (;mei<e.size();++mei){
	execAndHandle<exc::MultipleExceptionIterator,&exc::MultipleExceptionIterator::operator()>(&mei,data);
      }	
    }catch(exc::Exception& e){ //not sure what happened, tell everyone
	HwIdList bad(spool.size());
	bad.fillConsecutive();
	toNotify.handle(bad,e,data);
    }
  }

  template<typename T>
  void  SensorProcessor<T>::CheckAndNotify::operator()(){
    (**n).exc.check();
    toNotify((**n).vals.data,*(**n).readData);	
  }
  
  template<typename T>
  SensorProcessor<T>::CheckAndNotify:: CheckAndNotify(SensorNotifierSignal<T>& ptoNotify):toNotify(ptoNotify){}
  
  template<typename T>
  void SensorProcessor<T>::run(){
    running=true;
    CheckAndNotify can(toNotify);
    
    while(true){
      can.n=pipe.getReaderNode();
      //std::cout<<" got data from the pipe"<<std::endl;
      if (pipe.isSentinel(can.n)){
	if (!running) return;
      }else{
	execAndHandle<CheckAndNotify,&CheckAndNotify::operator()>(&can,(**can.n).vals.data); //if anything is ok nothing happens here,
	//otherwise handle() of everyone is called
	//unaffected listeners will call the operaotor() as usually	  
	pipe.remitReaderNode(can.n);
      }
      
    }
    //std::cout<<"ret"<<std::endl;
  }


  template<typename T>
  void SensorProcessor<T>::do_read(Sensor& s,T& t,int idx){
    t=s.rawRead(idx);
  }

  
  template<typename T>
  T SensorProcessor<T>::read(int idx){
    T r;
    typedef PairPasser<T&,int> Pair;
    StackJob<Sensor,
	     SensorProcessor,
	     const Pair&,
	     Pair,
	     &SensorProcessor::do_read,
	     true> job(this,Pair(r,idx));   
    spool.waitJob(&job,SensSpoolPriority::HIGH);
    return r;
  }
  
  template<typename T>
  void SensorProcessor<T>::do_readAll(Sensor& s,T* t){
    s.rawRead(t);
  }

  
  template<typename T>
    void SensorProcessor<T>::read(T* t){
    StackJob<Sensor,
	     SensorProcessor,
	     T*,
	     T*,
	     &SensorProcessor<T>::do_readAll,
	     true,
	     T*
	     > job(this,t);       
    spool.waitJob(&job,SensSpoolPriority::HIGH);
  }


  template<typename T>
  void SensorProcessor<T>::do_readGroup(Sensor& s,SensorValList<T>& v,const HwIdList& what){
    s.rawRead(v,what);
  }
  
  template<typename T>
  void SensorProcessor<T>::do_vreadGroup(Sensor& s,T* v,const HwIdList& what){
    s.rawRead(v,what);
  }
  


  template<typename T>
  void SensorProcessor<T>::read(SensorValList<T>& g,const HwIdList& l){
    g.vals.resize(l.ids.size());
    typedef PairPasser<SensorValList<T>&,const HwIdList&> Pair;
    StackJob<Sensor,
	     SensorProcessor,
	     const Pair&,
	     Pair,
	     &SensorProcessor<T>::do_readGroup,
	     true
	     > job(this,Pair(g,l));       
    spool.waitJob(&job,SensSpoolPriority::HIGH);
  }

  template<typename T>
    void SensorProcessor<T>::read(T* t,const HwIdList& what){
    typedef PairPasser<T*,const HwIdList&> Pair;
    StackJob<Sensor,
	     SensorProcessor,
	     const Pair&,
	     Pair,
	     &SensorProcessor<T>::do_vreadGroup,
	     true
	     > job(this,Pair(t,what));       
    spool.waitJob(&job,SensSpoolPriority::HIGH);
  }

  template<typename T>  
   void SensorProcessor<T>::do_connect(sensor::Set<T>& s,MultipleSensorNotifier<T>* n){
    toNotify.connect(*n);
    reads[n->type]->mark(s,n->what.ids);
  }
  
  template<typename T>
  void SensorProcessor<T>::do_disconnect(sensor::Set<T>& s,MultipleSensorNotifier<T>* n){
    toNotify.disconnect(*n);
    reads[n->type]->unmark(s,n->what.ids);
  }
  
   template<typename T>
  void SensorProcessor<T>::anyConnect(MultipleSensorNotifier<T>* n,DisconnectInfo* &disc){
    StackJob<Sensor,
	     SensorProcessor,
	     MultipleSensorNotifier<T>*,
	     MultipleSensorNotifier<T>*,
	     &SensorProcessor::do_connect,true> job(this,n);
  spool.waitJob(&job,SensSpoolPriority::HIGH);
  disc=n;
}

template<typename T>
void SensorProcessor<T>::anyDisconnect(DisconnectInfo* &disc){
  MultipleSensorNotifier<T>* n=trust_cast<MultipleSensorNotifier<T>*>(disc);
  StackJob<Sensor,
	   SensorProcessor,
	   MultipleSensorNotifier<T>*,
	   MultipleSensorNotifier<T>*,
	   &SensorProcessor::do_disconnect,true> job(this,n);
  spool.waitJob(&job,SensSpoolPriority::HIGH);
  delete disc;
  disc=NULL;
}
  

  template<typename T>
  void SensorProcessor<T>::connect(EhConnReference<const T*>& ref,const HwIdList& what, sensor::ReadType type){
    anyConnect(new TypedGroupSensorNotifier<T>(type,what,ref.cb,ref),ref.disc);
  }
    
  template<typename T>
    void SensorProcessor<T>::disconnect(EhConnReference<const T*>& ref){
    anyDisconnect(ref.disc);
  }


  template<typename T>    
  SensorProcessor<T>::SensorProcessor(Spool& pspool,bool autoStart):spool(pspool),pipe(spool.size()),running(false)
  {      
      reads[sensor::IDLE]=new ReadWork(*this,spool.size(),SensSpoolPriority::IDLE);
      reads[sensor::READ]=new ReadWork(*this,spool.size(),SensSpoolPriority::NORMAL);
      if (autoStart){
	start();
      }
    }

    template<typename T>    
    SensorProcessor<T>::~SensorProcessor(){
      running=false;
      pipe.sendSentinel(); //fake data
      join();
      delete reads[sensor::IDLE];
      delete reads[sensor::READ];
    }

template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType> 
ExportableSensorProcessor<T,SingleType,GroupType>::ExportableSensorProcessor(SensorSpooler<T>& pspool,bool autoStart):
  SensorProcessor<T>(pspool,autoStart),sensorCells(*this,pspool.size()),sensorGroups(*this)
  {
    
  }
  


template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType> 
  void DefDescr<ExportableSensorProcessor<T,SingleType,GroupType> >::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      //std::cout<<"EXPORTING "<<std::endl;
  if (SingleType!= sensorProcessing::NO_CELL){
    members.push_back(scalarMapping(this->obj.sensorGroups,IDPath("group.value",".")));
  }
  
  if (GroupType!= sensorProcessing::NO_CELL){
    members.push_back(vectMapping(this->obj.sensorCells,"sensor","value","num"));
  }
}

}//helium

  /*template<typename T>
  void SensorProcessor<T>::SensorGroupCell::readAll(Sensor& s,T* t){
    s.rawRead(t);
  }
 


  template<typename T>
  void SensorProcessor<T>::SensorGroupCell::readGroup(Sensor& s,const ReadGroupParam& p){
  s.rawRead(*p.first,*p.second);	
  }
  
  template<typename T>
  void SensorProcessor<T>::SensorGroupCell::vreadGroup(Sensor& s,const VReadGroupParam& p){
  s.rawRead(p.first,*p.second);	
  }
  */

  /*
template<typename T>
      void SensorProcessor<T>::SensorGroupCell::do_connect(sensor::Set<T>& s,MultipleSensorNotifier<T>* n){
	//std::cout<<"SensorGroupCell::do_connect"<<std::endl;
  owner.toNotify.connect(*n);
  owner.reads[n->type]->mark(s,n->what.ids);
  //std::cout<<"} SensorGroupCell::do_connect"<<std::endl;
}

template<typename T>
      void SensorProcessor<T>::SensorGroupCell::do_disconnect(sensor::Set<T>& s,MultipleSensorNotifier<T>* n){
	//std::cout<<"SensorGroupCell::do_disconnect"<<std::endl;
	owner.toNotify.disconnect(*n);
	owner.reads[n->type]->unmark(s,n->what.ids);
	//std::cout<<"} SensorGroupCell::do_disconnect"<<std::endl;
      }
  */


  /*     template<typename T>
      void SensorProcessor<T>::SensorGroupCell::anyConnect(MultipleSensorNotifier<T>* n,DisconnectInfo* &disc){
	//std::cout<<"SensorGroupCell::anyConnect"<<std::endl;
	StackJob<Sensor,
	  SensorGroupCell,
	  MultipleSensorNotifier<T>*,
	  MultipleSensorNotifier<T>*,
		 &SensorGroupCell::do_connect,true> job(this,n);
	owner.spool.waitJob(&job,SensSpoolPriority::HIGH);
	disc=n;
      }

template<typename T>
      void SensorProcessor<T>::SensorGroupCell::anyDisconnect(DisconnectInfo* &disc){
	MultipleSensorNotifier<T>* n=trust_cast<MultipleSensorNotifier<T>*>(disc);
	StackJob<Sensor,
	  SensorGroupCell,
	  MultipleSensorNotifier<T>*,
	  MultipleSensorNotifier<T>*,
		 &SensorGroupCell::do_disconnect,true> job(this,n);
	owner.spool.waitJob(&job,SensSpoolPriority::HIGH);
	delete disc;
	disc=NULL;
      }
  */

   /*g.vals.resize(owner.sensorCells.size());
      StackJob<Sensor,
		 SensorGroupCell,
	   T*,
	   T*,
	   &SensorGroupCell::readAll,
	   true,
	   T*
		 > job(this,g.vals.data);       
		 owner.spool.waitJob(&job,SensSpoolPriority::HIGH);*/

/*g.vals.resize(l.ids.size());
	ReadGroupParam p(&g,&l);
	StackJob<Sensor,
	  SensorGroupCell,
	  const ReadGroupParam&,
	  const ReadGroupParam&,
	  &SensorGroupCell::readGroup,true> job(this,p);       */
  //owner.spool.waitJob(&job,SensSpoolPriority::HIGH);

  /*VReadGroupParam p(g,&l);
  StackJob<Sensor,
	   SensorGroupCell,
	   const VReadGroupParam&,
	   const VReadGroupParam&,
	   &SensorGroupCell::vreadGroup,true> job(this,p);       
	   owner.spool.waitJob(&job,SensSpoolPriority::HIGH);*/
