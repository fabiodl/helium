namespace helium{
  namespace sensor{




    template<typename R,typename T>
     GenericFilter<R,T>::FilterSignal::FilterSignal(GenericFilter<R,T>& powner):owner(powner){
    }
    
    
    template<typename R,typename T>
    void GenericFilter<R,T>::FilterSignal::operator()(const R* r){
      //MutexLock lock(owner.tmpAccess); already locked by the info operator
      for (typename std::list<FilterConnInfo<R>*>::iterator it=conns.begin();
	   it!=conns.end();++it){
	(*(*it)->cb)(r);
      }    
      
    }

  template<typename R,typename T>
  void GenericFilter<R,T>::FilterSignal::handle(const HwIdList& bad,
						exc::Exception& e,const R* r){
    MutexLock lock(owner.tmpAccess);
    for (typename std::list<FilterConnInfo<R>*>::iterator it=conns.begin();
	 it!=conns.end();++it){
      (*it)->handle(bad,e,r);
    }//for    
    
  }//handle
  
  template<typename R,typename T>
  void GenericFilter<R,T>::FilterSignal::connect(FilterConnInfo<R>* c){
    owner.tmpAccess.lock();
    c->it=conns.insert(conns.end(),c);
    owner.tmpAccess.unlock();
  }

  template<typename R,typename T>
  void GenericFilter<R,T>::FilterSignal::disconnect(FilterConnInfo<R>* c){
    owner.tmpAccess.lock();
    conns.erase(c->it);
    owner.tmpAccess.unlock();
  }







  template<typename R,typename T>
  void GenericFilter<R,T>::ReadInfo::update(){ //locked by mark/unmark  
    bool wasConnected=srcList.ids.size()>0;
    resList.ids.clear();
    for (size_t i=0;i<rcnt.size();i++){
      if (rcnt[i]){
	resList.ids.push_back(i);
      }//if	  
    }//for i
    srcList.ids.clear();
    owner.comp.getRequired(srcList,resList);      
    if (wasConnected){
      //std::cout<<"disconnecting from src"<<std::endl;
      owner.src.disconnect(ref);
    }
    if (resList.ids.size()>0&&srcList.ids.size()>0){//the resList is for buggy computations than just return "read all"
      owner.src.connect(ref,srcList,type);
    }
  }//update


  template<typename R,typename T>
  GenericFilter<R,T>::ReadInfo::~ReadInfo(){
    owner.rinfoAccess.lock();
    if (srcList.ids.size()>0){
      owner.src.disconnect(ref);
    }
    owner.rinfoAccess.unlock();
  }


   template<typename R,typename T>
   GenericFilter<R,T>::ReadInfo::ReadInfo(GenericFilter& powner,sensor::ReadType ptype):owner(powner),type(ptype),ref(this,this), rcnt(owner.resSize()),toNotify(powner){
     zero(rcnt);
   }
  
   


  template<typename R,typename T>
  void GenericFilter<R,T>::ReadInfo::mark(const HwIdList& sl){
    MutexLock lock(owner.rinfoAccess);
    bool toUpdate=false;
    for (size_t i=0;i<sl.ids.size();i++){
      int idx=sl.ids[i];
      if (rcnt[idx]==0) toUpdate=true;
      rcnt[idx]++;
    }
    if (toUpdate){
      update();
    }
    
  }

  template<typename R,typename T>
  void GenericFilter<R,T>::ReadInfo::unmark(const HwIdList& sl){
    MutexLock lock(owner.rinfoAccess);
    bool toUpdate=false;
    //std::cout<<"Unmarking";
    //print(sl);
    for (size_t i=0;i<sl.ids.size();i++){
      int idx=sl.ids[i];	 
      rcnt[idx]--;
      if (rcnt[idx]==0) toUpdate=true;
    }
    if (toUpdate){
      //std::cout<<"update!"<<std::endl;
      update();
    }
  }

  template<typename R,typename T>
  void GenericFilter<R,T>::ReadInfo::operator()(const T* t){
    MutexLock lock(owner.tmpAccess);
    //std::cout<<"content "<<std::endl;
    //print(t,4,4)<<std::endl;
    owner.comp.compute(owner.res.data,t,resList);
    toNotify(owner.res.data);
  }



    template<typename R,typename T>
    void GenericFilter<R,T>::ReadInfo::connect(FilterConnInfo<R>* cinfo){      
      mark(cinfo->what);      
      toNotify.connect(cinfo);      
    }

    template<typename R,typename T>
    void GenericFilter<R,T>::ReadInfo::disconnect(FilterConnInfo<R>* dinfo){
      toNotify.disconnect(dinfo);
      unmark(dinfo->what);
  }



  template<typename R,typename T>
  void GenericFilter<R,T>::ReadInfo::handle(exc::Exception& e){
    try{
      e.rethrow();
    }catch(exc::SensorGroupException<T>& e){      
      HwIdList cango,badResults;
      owner.comp.getGenerated(badResults,e.bad);
      setDifference(cango.ids,resList.ids,badResults.ids);
      {
	MutexLock lock(owner.tmpAccess);
	owner.comp.compute(owner.res.data,e.data,cango);
	toNotify.handle(badResults,e,owner.res.data);
      }
    }    
    



  }



   
 

template<typename R>
FilterConnInfo<R>::FilterConnInfo(const HwIdList& pwhat,
							sensor::ReadType ptype,
							exc::ExceptionHandler& peh,
							Callback<const R*>* pcb):
  what(pwhat),type(ptype),eh(peh),cb(pcb){
}

  
template<typename R>
void FilterConnInfo<R>::handle(const HwIdList& bad,exc::Exception& e,const R* r){
  if (intersects(bad.ids,what.ids)){
    eh.handle(e);
  }else{
    (*cb)(r);
  }
}

  template<typename R>
  void GroupCellFilterConnInfo<R>::operator()(const R* r){
    for (size_t i=0;i<this->what.ids.size();++i){
      vals.vals[i]=r[this->what.ids[i]];
    }//for
    (*ccb)(&vals);
  }
    
    template<typename R>
    GroupCellFilterConnInfo<R>::GroupCellFilterConnInfo(const HwIdList &what,sensor::ReadType type,Cell::CellConnection& eh):
      FilterConnInfo<R>(what,type,eh,this),
      vals(what.ids.size()),ccb(eh.cb)
    {
      
    }
    
    
    template<typename R>
    void SingleCellFilterConnInfo<R>::operator()(const R* r){ 
      //std::cout<<"accessing element "<<this->what.ids[0]<<std::endl;
      (*ccb)(&r[this->what.ids[0]]);
    }
    
    
    
    template<typename R>
    SingleCellFilterConnInfo<R>::SingleCellFilterConnInfo(int id,sensor::ReadType type,Cell::CellConnection& eh):
      FilterConnInfo<R>(SingleHwIdList(id),type,eh,this),ccb(eh.cb)
    {
      
    }



        template<typename R>
    OnChangeGroupCellFilterConnInfo<R>::OnChangeGroupCellFilterConnInfo(const HwIdList &what,sensor::ReadType type,Cell::CellConnection& eh):
      FilterConnInfo<R>(what,type,eh,this),
      vals(what.ids.size()),ccb(eh.cb),
      toUpdate(true)
    {
      
    }
    
    


    template<typename R>
    void OnChangeGroupCellFilterConnInfo<R>::operator()(const R* r){
      for (size_t i=0;i<this->what.ids.size();++i){
	if (vals.vals[i]!=r[this->what.ids[i]]){
	  toUpdate=true;
	  vals.vals[i]=r[this->what.ids[i]];
	}	
      }//for
      if (toUpdate){
	(*ccb)(&vals);
      }
      toUpdate=false;//reset for the next time, as we want the first to be true (from the ctor)
    }

    
    template<typename R>
    OnChangeSingleCellFilterConnInfo<R>::OnChangeSingleCellFilterConnInfo(int id,sensor::ReadType type,Cell::CellConnection& eh):
       FilterConnInfo<R>(SingleHwIdList(id),type,eh,this),ccb(eh.cb)
    {
      
    }

    
    template<typename R>
    void OnChangeSingleCellFilterConnInfo<R>::operator()(const R* r){ 
      //std::cout<<"accessing element "<<this->what.ids[0]<<std::endl;
      if (r[this->what.ids[0]]!=val||toUpdate){	
	(*ccb)(r+this->what.ids[0]);	
      }
      toUpdate=false;
    }
    
    
    


  
  template<typename R,typename T>
  void GenericFilter<R,T>::updateRes(const HwIdList& toComp,const HwIdList& toRead){
    src.read(sdata.data,toRead);
    comp.compute(res.data,sdata.data,toComp);
  }
  
  
  
  template<typename R,typename T>
  std::string GenericFilter<R,T>:: getName() const{
    return "sensor filter cell";
  }
  
  template<typename R,typename T>
  void GenericFilter<R,T>::getData(SensorValList<R>& t){
    t.vals.resize(res.size());
    read(t.vals.data);
  }
  
  template<typename R,typename T>
  void GenericFilter<R,T>::getData(SensorValList<R>& t,const HwIdList& p){
    t.vals.resize(p.ids.size());
    read(t,p);
  }
  


  

  template<typename R,typename T>
  void GenericFilter<R,T>::connect(EhConnReference<const R*> &ref,const HwIdList& what, sensor::ReadType type){      
    FilterConnInfo<R>* finfo=new FilterConnInfo<R>(what,type,ref,ref.cb);
    rinfo[type].connect(finfo);
    ref.disc=finfo;
  }
  
  template<typename R,typename T>
  void GenericFilter<R,T>::connect(EhConnReference<const R*> &ref,const sensor::ReadType type){
    connect(ref,rAll,type);    
  }


  template<typename R,typename T>
  void GenericFilter<R,T>::connect(Cell::CellConnection& ref,const std::pair<HwIdList, sensor::ReadType>& p){
    OnChangeGroupCellFilterConnInfo<R>* cfinfo=new OnChangeGroupCellFilterConnInfo<R>(p.first,p.second,ref);
    rinfo[p.second].connect(cfinfo);
    ref.disc=cfinfo;      
  }


  template<typename R,typename T>
  void GenericFilter<R,T>::connect(Cell::CellConnection& ref){    
    OnChangeGroupCellFilterConnInfo<R>* cfinfo=new OnChangeGroupCellFilterConnInfo<R>(rAll,sensor::READ,ref);
    rinfo[sensor::READ].connect(cfinfo);
    ref.disc=cfinfo;
  }
    
    template<typename R,typename T>
    void GenericFilter<R,T>::connect(Cell::CellConnection& ref,sensor::ReadType type,int id){
      OnChangeSingleCellFilterConnInfo<R>* cfinfo=new OnChangeSingleCellFilterConnInfo<R>(id,type,ref);
      rinfo[type].connect(cfinfo);
      ref.disc=cfinfo;
    }


  template<typename R,typename T>
  void GenericFilter<R,T>::disconnect(EhConnReference<const R*> &ref){
    FilterConnInfo<R>* fci=trust_cast<FilterConnInfo<R>*>(ref.disc);
    rinfo[fci->type].disconnect(fci);
    ref.clearDisc();
  }



  template<typename R,typename T>
   void GenericFilter<R,T>::disconnect(Cell::CellConnection &ref){
    FilterConnInfo<R>* fci=trust_cast<FilterConnInfo<R>*>(ref.disc);
    rinfo[fci->type].disconnect(fci);
    ref.clearDisc();
  }
  





  
  template<typename R,typename T>
  R GenericFilter<R,T>::read(int idx){
    HwIdList toComp;
    toComp.ids.push_back(idx);
    HwIdList toRead;   
    comp.getRequired(toRead,toComp);
    R r;
    {
      MutexLock lock(tmpAccess);
      updateRes(toComp,toRead);
      r=res[idx];
    }
    //std::cout<<"read value @"<<idx<<" = "<<r<<std::endl;
    return r;
  }
  
   template<typename R,typename T>
   void GenericFilter<R,T>::read(R* r){
     MutexLock lock(tmpAccess);
     src.read(sdata.data,sAll);
     comp.compute(r,sdata.data,rAll);              
   }

  template<typename R,typename T>
   void GenericFilter<R,T>::read(R* r,const HwIdList& what){
	HwIdList toRead;   
	comp.getRequired(toRead,what);
	{
	  MutexLock lock(tmpAccess);
	  updateRes(toRead,what);
	  for (size_t i=0;i<what.ids.size();i++){
	    int idx=what.ids[i];
	    r[idx]=res[idx];
	  }	  
	}
  }
  

  template<typename R,typename T>
  void GenericFilter<R,T>::read(SensorValList<R>& v,const HwIdList& what){
    HwIdList toRead;   
    comp.getRequired(toRead,what);
    {
      MutexLock lock(tmpAccess);
      updateRes(toRead,what);
      for (size_t i=0;i<what.ids.size();i++){
	v.vals[i]=res[what.ids[i]];
      }	  
    }
  }


  template<typename R,typename T>
  GenericFilter<R,T>::GenericFilter(GenericFilterComputation<R,T>& pcomp,int rn,FilterInterface<T>& psrc,int sn):
    rinfoAccess("rinfoAccess"),tmpAccess("tmpAccess"),src(psrc),comp(pcomp),res(rn),sdata(sn),rAll(rn),rinfo(*this){	
    rAll.fillConsecutive();
    comp.getRequired(sAll,rAll);
  }	
  
    
    

  
    template<typename R,typename T>
    size_t GenericFilter<R,T>::srcSize() const{
      return sdata.size();
    }
    
    template<typename R,typename T>
    size_t GenericFilter<R,T>::resSize() const{
      return res.size();
    }


    
    



  }//sensor

}//helium





 /*template<typename R,typename T>
  void GenericFilter<R,T>::ReadInfo::mark(int x){
    MutexLock lock(owner.rinfoAccess);
    bool toUpdate=(rcnt[x]==0);
    rcnt[x]++;
    if (toUpdate){
      update();
    }
    
  }//mark
  
  template<typename R,typename T>
  void GenericFilter<R,T>::ReadInfo::unmark(int x){	
    MutexLock lock(owner.rinfoAccess);
    rcnt[x]--;
    if (rcnt[x]==0){
      update();
    }	
  }//unmark
    */
 /*template<typename R,typename T>
  GenericFilter<R,T>::DiscInfo::DiscInfo(int n,sensor::ReadType ptype):disc(NULL),what(n),type(ptype){	
    what.fillConsecutive();
  }

  template<typename R,typename T>
  GenericFilter<R,T>::DiscInfo::DiscInfo(const HwIdList& pwhat,sensor::ReadType ptype):disc(NULL),what(pwhat),type(ptype){	
  }

  template<typename R,typename T>
  GenericFilter<R,T>::DiscInfo::~DiscInfo(){
    if (disc!=NULL){
      delete disc;
    }
    }*/
  
  /*template<typename R,typename T>
  void GenericFilter<R,T>::WholeCBAdapter::operator()(const R* r){
    assign(data.vals,r);
    (*cb)(&data);
  }
  
  template<typename R,typename T>
  void GenericFilter<R,T>::WholeCBAdapter(int n,sensor::ReadType type,Cell::CellConnection& conn):
	  DiscInfo(n,type),
	  EhPtrConnReference<const R*>(this,&pcb),	
	  data(n),cb(conn.cb)
  {	  
  };
  
  template<typename R,typename T>
  void GenericFilter<R,T>::GroupCBAdapter::operator()(const R* r){
    for (size_t i=0;i<data.vals.size();i++){
      data.vals[i]=r[this->what.ids[i]];
    }
    (*cb)(&data);
  }

  template<typename R,typename T>
  void GenericFilter<R,T>::GroupCBAdapter::operator()(const R* r){
    for (size_t i=0;i<data.vals.size();i++){
      data.vals[i]=r[this->what.ids[i]];
    }
    (*cb)(&data);
  }
  
  template<typename R,typename T>
  void GenericFilter<R,T>::GroupCBAdapter::GroupCBAdapter(const HwIdList& what,sensor::ReadType type,Cell::CellConnection& conn):
    DiscInfo(what,type),
    EhPtrConnReference<const R*>(this,&pcb),
    data(what.ids.size()),cb(conn.cb)
  {	  
  };*/
  
