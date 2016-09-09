/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.01.31
 * \version 0.2
 * Release_flags s
 */


#include <helium/core/storableType.h>

namespace helium{
  
  

  
  
 
  

  ////----------------------VPOPER------------------

  
  template<>
  template<typename VP> void vpOper<true>::FAILCOMPILE(VP* v){
    v->vDoesNotHaveThisFunction();
  }

  template<>
  template<typename VP> void vpOper<false>::FAILCOMPILE(VP* v){
  }



  template<>
  template<typename VP> void vpOper<true>::assureConnection(VP* v,const std::string& oper){
    if (!v->isConnected()){
      throw exc::OperationOnDisconnectedProxy(oper+" on a disconnected valueProxy");
    }
  }

  template<>
  template<typename VP> void vpOper<false>::assureConnection(VP* v,const std::string& oper){
  }


  
  template<>
  template<typename Return,typename VP>
   Return vpOper<true>::getCache(VP* v){
    return v->wCache;
  }
  
  template<>
  template<typename Return,typename VP>
  Return vpOper<false>::getCache(VP* v){
    typename VP::V::GT g;
    v->getData(&g); //this may throw
    v->template processValue<ONGET,0>(g);
    return g;
  }



  template<>
  template<typename VP,typename P>
  void vpOper<true>::connectSingleParCache(VP* v, const P& p){
    static_cast<GenericValueProxy*>(v)->connect
    (*vpOper<(ONREMOTE&VP::V::Access::parConnectFlags)!=0>::createSignal(v,p),
     &p);
}

  template<>
  template<typename VP,typename P>
  void vpOper<false>::connectSingleParCache(VP* v, const P& p){
  }



template<>
template<typename VP,typename P>
void vpOper<true>::retrieveSingleParCache(VP* v, const P& p){
  typename VP::V::GT g;
  v->getData(&g,&p);
  v->template parProcessValue<ONREMOTE,0>(g,p);
}

template<>
template<typename VP,typename P>
void vpOper<false>::retrieveSingleParCache(VP* v, const P& p){
}


  template<>
  template<typename Return, typename VP,typename P>
  Return vpOper<true>::getParCache(VP* v,const P& p){
    SharedUse use(v->acc);
    if (v->parCache.find(p)==v->parCache.end()&& 
	v->isConnected()){//calling functions should throw an exception if REMOTELESSPARGET is disabled
      
      vpOper<(ONREMOTE&VP::V::Access::parConnectFlags)!=0>::connectSingleParCache(v,p);
      //no need to check for onremoteandlistener,no one is listening if it was not in the cache
      vpOper<(ONREMOTE&(VP::V::Access::parWriteCacheFlags|
			VP::V::Access::parSignalFlags)
	      )!=0>::retrieveSingleParCache(v,p);
    }   
    return v->parCache[p];
  }
  
  template<>
  template<typename Return, typename VP,typename P>
  Return vpOper<false>::getParCache(VP* v,const P& p){
    typename VP::V::GT g;
    SharedUse use(v->acc);
    v->getData(&g,&p);
    v->template parProcessValue<ONPARGET,0>(g,p);
    return g;      
  }



  template<>
  template<typename VP,typename X> 
  void vpOper<true>::writeCache(VP* v,const X& x){
    SharedUse use(v->acc);
    v->wCache=x;
  }
  
  template<>
  template<typename VP,typename X> 
  void vpOper<false>::writeCache(VP* v,const X& x){}



  template<>
  template<typename VP,typename X,typename P> 
  void vpOper<true>::parWriteCache(VP* v,const X& x,const P& p){      
    SharedUse use(v->acc);
    v->parCache[p]=x;
  }

  template<>
  template<typename VP,typename X,typename P> 
  void vpOper<false>::parWriteCache(VP* v,const X& x,const P& p){}



  template<>
  template<typename VP,typename X> 
  void  vpOper<true>::sigW(VP* v,X& x){
    v->wSignal.sig(x);
  }

  template<>
  template<typename VP,typename X> 
  void  vpOper<false>::sigW(VP* v,X& x){}


 
  template<>
  template<typename VP,typename X> 
  void vpOper<true>::sigW(VP* v,X& x,const Callback<typename VP::V::RUT>* noEx){
    v->wSignal.sig(x,noEx);
  }

  template<>
  template<typename VP,typename X> 
  void vpOper<false>::sigW(VP* v,X& x,const Callback<typename VP::V::RUT>* noEx){}



  template<>
  template<typename VP,typename X,typename P> 
  void vpOper<true>::sigPar(VP* v,X& x,const P& p){
      SharedUse use(v->acc);
      typedef typename std::map<typename VP::V::UP,typename VP::ParSigData* >::iterator IT;   
      IT i=v->parSignal.find(p);
      if (i!=v->parSignal.end()){	
	i->second->sig(x);      
      }
    }

  template<>
  template<typename VP,typename X,typename P> 
  void vpOper<false>::sigPar(VP* v,X& x,const P& p){}



  template<>
  template<typename VP,typename X,typename P> 
  void vpOper<true>::sigPar(VP* v,X& x,const P& p,const Callback<typename VP::V::RUT>* noEx){
    SharedUse use(v->acc);
    v->parSignal[p]->sig(x,noEx);
  }

  template<>
  template<typename VP,typename X,typename P> 
  void vpOper<false>::sigPar(VP* v,X& x,const P& p,const Callback<typename VP::V::RUT>* noEx){}


  
  template<>
  template<typename VP>  
  void vpOper<true>::signalCache(VP* v){
    sigW(v,v->wCache);
  }

  template<>
  template<typename VP>  
  void vpOper<false>::signalCache(VP* v){}
 

  template<>
  template<typename VP,typename P> 
  void vpOper<true>::signalParCache(VP* v,const P& p){
    typedef typename cacheType<VP,VP::Access::parCacheAttachment>::CacheType CacheType;
    sigPar(v,getParCache<const CacheType&>(v,p),p);
  }

  template<>
  template<typename VP,typename P> 
  void vpOper<false>::signalParCache(VP* v,const P& p){}
  


  template<>
  template<typename VP> 
  bool  vpOper<true>::isToSet(VP* v,const typename VP::V::ST& st){
    return v->get()!=st;
  }

  template<>
  template<typename VP> 
  bool vpOper<false>::isToSet(VP* v,const typename VP::V::ST& st){
    return true;
  }



  template<>
  template<typename VP> 
  void  vpOper<true>::wlinkOwner(VP* v){
    v->wSignal.work.owner=v;
  } 
  
  template<>
  template<typename VP> 
  void  vpOper<false>::wlinkOwner(VP* v){}
  

  
  template<>
  template<typename VP> 
  bool vpOper<true>::isConnected(VP* v){
    return (v->wSignal.sig.listenerNum()>0)||
      ((VP::V::Access::connectFlags&ONREMOTE) && (v->remote!=NULL));
  }

  //false version is not used. isConnected  is used by other methods in vpOper<true>



  template<>
  template<typename VP> 
  void vpOper<true>::connect(VP* v,ConnReference<typename VP::V::RUT>& c){     
    SharedUse use(v->acc);
    if (v->remote!=NULL&&!isConnected(v)){
      v->remote->connect(v->wSignal);
    }
    v->wSignal.sig.connect(c);
    wrapInfo<const typename VP::V::UP*>(c,NULL);
  }

  template<>
  template<typename VP>
  void vpOper<false>::connect(VP* v,ConnReference<typename VP::V::RUT>& c){}


  

  template<>
  template<typename VP> 
  void  vpOper<true>::connect(VP* v,Callback<typename VP::V::RUT>* c){     
    SharedUse use(v->acc);
    if (v->remote!=NULL&&!isConnected(v)){
      v->remote->connect(v->wSignal);
    }
    v->wSignal.sig.connect(c);
  }
  
  template<>
  template<typename VP> 
  void  vpOper<false>::connect(VP* v,Callback<typename VP::V::RUT>* c){}


  
  template<>
  template<typename VP> 
  typename vpOper<true>::SignalObjPtr<VP>::value
  vpOper<true>::createSignal(VP* v,const typename VP::V::UP& p){
    typedef typename std::map<typename VP::V::UP,typename VP::ParSigData* >::iterator IT;   
    v->acc.upToExclusive();
    std::pair<IT,bool> ni=v->parSignal.insert(std::make_pair(p,new typename VP::ParSigData()));
    IT i=ni.first;    
    i->second->work.owner=v;
    i->second->work.p=&i->first;
    v->acc.downToShared();
    if (v->remote!=NULL){	    
      v->remote->connect(*i->second,&p);
    }    

    return i->second;
  }
  
 template<>
 template<typename VP>
 typename vpOper<false>::SignalObjPtr<VP>::value
 vpOper<false>::createSignal(VP* v,const typename VP::V::UP& p){
 }



  

    
template<>
template<typename VP> 
typename vpOper<true>::SignalObjPtr<VP>::value
  vpOper<true>::assureSignal(VP* v,const typename VP::V::UP& p){
    typedef typename std::map<typename VP::V::UP,typename VP::ParSigData* >::iterator IT;   
    IT i=v->parSignal.find(p);
    if (i==v->parSignal.end()){	
      return createSignal(v,p);
    }else{
      return i->second;
    }
  }
  
  template<>
  template<typename VP>   
typename vpOper<false>::SignalObjPtr<VP>::value
  vpOper<false>::assureSignal(VP* v,const typename VP::V::UP& p){    
  }



   template<>
  template<typename VP> 
  void vpOper<true>::parConnect(VP* v,Callback<typename VP::V::RUT>* c,const typename VP::V::UP& p){  
    SharedUse use(v->acc);       
    assureSignal(v,p)->sig.connect(c);          

  }

  template<>
  template<typename VP> 
  void vpOper<false>::parConnect(VP* v,Callback<typename VP::V::RUT>* c,const typename VP::V::UP& p){}
  





  template<>
  template<typename VP> 
  void vpOper<true>::parConnect(VP* v,ConnReference<typename VP::V::RUT>& c,const typename VP::V::UP& p){
    SharedUse use(v->acc);
    typename VP::ParSigData* s=assureSignal(v,p);
    s->sig.connect(c);     
    wrapInfo<const typename VP::V::UP*>(c,s->work.p);    
  }

  template<>
  template<typename VP> 
  void vpOper<false>::parConnect(VP* v,ConnReference<typename VP::V::RUT>& c,const typename VP::V::UP& p){}
  
  

 

  
  //returns if this was without parameters (otherwise we need to use parDisconnect)
  template<>
  template<typename VP> 
  bool vpOper<true>::disconnect(VP* v,ConnReference<typename VP::V::RUT>& c){    
    const typename VP::V::UP* p=peekInfo<const typename VP::V::UP*>(c);
    if (p==NULL){	
      discardInfo<const typename VP::V::UP*>(c);
      v->wSignal.sig.disconnect(c);
      if (!isConnected(v)&&v->remote!=NULL){
	SharedUse use(v->acc);
	v->remote->disconnect(v->wSignal);
      }//!isConnected
      
    }//p!=NULL
    return p==NULL;
  }

  
  template<>
  template<typename VP> 
  bool vpOper<false>::disconnect(VP* v,ConnReference<typename VP::V::RUT>& c){
    return false;
  }


  
  template<>
  template<typename VP> 
  void vpOper<true>::parDisconnect(VP* v,ConnReference<typename VP::V::RUT>& c){
    const typename VP::V::UP* p=peekInfo<const typename VP::V::UP*>(c);
    discardInfo<const typename VP::V::UP*>(c);
    SharedUse use(v->acc);    
    typename std::map<typename VP::V::UP,typename VP::ParSigData* > ::iterator i=v->parSignal.find(*p);
    i->second->sig.disconnect(c);
    if (i->second->sig.listenerNum()==0){
      if (v->remote!=NULL){
	v->remote->disconnect(*i->second);
      }
      v->acc.upToExclusive();
      delete i->second;
      v->parSignal.erase(i);
      v->acc.downToShared();
    } 
  }//parDisconnect

  template<>
  template<typename VP> 
  void vpOper<false>::parDisconnect(VP* v,ConnReference<typename VP::V::RUT>& c){}
  
  
  
  template<>
  template<typename VP> 
  void vpOper<true>::remoteConnect(VP* v){
    static_cast<GenericValueProxy*>(v)->connect(v->wSignal);
  }

  template<>
  template<typename VP> 
  void vpOper<false>::remoteConnect(VP* v){}

  
  
  template<>
  template<typename VP> 
  void vpOper<true>::remoteConnectOnListeners(VP* v){
    if (v->wSignal.sig.listenerNum()>0)
      static_cast<GenericValueProxy*>(v)->connect(v->wSignal);
  }
  
  template<>
  template<typename VP> 
  void vpOper<false>::remoteConnectOnListeners(VP* v){}


  
  template<>
  template<typename VP> 
  void vpOper<true>::parRemoteConnect(VP* v){
    SharedUse use(v->acc);
    typedef typename std::map<typename VP::V::UP,typename VP::ParSigData* >::iterator IT;
    for (IT it=v->parSignal.begin();it!=v->parSignal.end();++it){
      static_cast<GenericValueProxy*>(v)->connect(*it->second,&it->first);
    }
    

    

  }

  template<>
  template<typename VP> 
  void vpOper<false>::parRemoteConnect(VP* v){}



  template<>
  template<typename VP>
  void vpOper<true>::parRemoteConnectOnListeners(VP* v){
    SharedUse use(v->acc);
    typedef typename std::map<typename VP::V::UP,typename VP::ParSigData* >::iterator IT;
    for (IT it=v->parSignal.begin();it!=v->parSignal.end();++it){
      if (it->second->sig.listenerNum()>0){
	static_cast<GenericValueProxy*>(v)->connect(*it->second,&it->first);
      }
    }
  }

  template<>
  template<typename VP>
  void vpOper<false>::parRemoteConnectOnListeners(VP* v){}


  
  template<>
  template<typename VP> 
  void vpOper<true>::clearConn(VP* v){
    v->wSignal.clear();
  }

  template<>
  template<typename VP> 
  void vpOper<false>::clearConn(VP* v){}


  
  template<>
  template<typename VP> 
  void vpOper<true>::parClearConn(VP* v){
    //SharedUse use(v->acc);  already inside exclusive use
    typedef typename std::map<typename VP::V::UP,typename VP::ParSigData* >::iterator IT;
    for (IT it=v->parSignal.begin();it!=v->parSignal.end();++it){
      it->second->clear();
    }
  }

  template<>
  template<typename VP> 
  void vpOper<false>::parClearConn(VP* v){}



  template<>
  template<typename VP,typename X> 
  void vpOper<true>::safeSet(VP* v,const X& st){
    v->safeSetData(&static_cast<const typename VP::V::ST&>(st));
  }

  template<>
  template<typename VP,typename X> 
  void vpOper<false>::safeSet(VP* v,const X& st){}


  
  template<>
  template<typename VP,typename X> 
  void  vpOper<true>::discardRetValSafeSet(VP* v,const X& st){
    typename VP::V::SP sp;
    v->safeSetData(&sp,&static_cast<const typename VP::V::ST&>(st));
  }

  template<>
  template<typename VP,typename X> 
  void  vpOper<false>::discardRetValSafeSet(VP* v,const X& st){}



  template<>
  template<typename VP> 
  void vpOper<true>::retrieveValue(VP* v){
    SharedUse use(v->acc);
    typename VP::V::GT g;
    v->getData(&g);
    v->template processValue<ONREMOTE,0>(g);
  }

  template<>
  template<typename VP> 
  void vpOper<false>::retrieveValue(VP* v){}


  
  template<>
  template<typename VP>  
  void vpOper<true>::parRetrieveValue(VP* v){
    SharedUse use(v->acc);
    RetrieveIdx<typename VP::V,VP> idx(v);
    const typename RetrieveIdx<typename VP::V,VP>::R* p;
    typename VP::V::GT g;
    while((p=idx.getNext())!=NULL){
      v->getData(&g,p);
      v->template parProcessValue<ONREMOTE,0>(g,*p);
    }      
  }

  template<>
  template<typename VP>  
  void vpOper<false>::parRetrieveValue(VP* v){}


  
  template<>
  template<typename VP> 
  void vpOper<true>::onRemoteAvailableSet(VP* v){
    v->setData(&v->wCache);
  }

  template<>
  template<typename VP> 
  void vpOper<false>::onRemoteAvailableSet(VP* v){}
  

  
  template<>
  template<typename VP> 
  void vpOper<true>::parOnRemoteAvailableSet(VP* v){
    SharedUse use(v->acc);
    typedef typename std::map<
      typename cacheType<typename VP::V,VP::V::Access::parCacheAttachment>::CacheParam,
      typename cacheType<typename VP::V,VP::V::Access::parCacheAttachment>::CacheType
      >::iterator IT;
  
    for (IT it=v->parCache.begin();it!=v->parCache.end();++it){
      v->setData(&it->second,&it->first);
    }
  }
  
  template<>
  template<typename VP> 
  void vpOper<false>::parOnRemoteAvailableSet(VP* v){}


  //---  VALUEPROCESSOR

  template<typename VP>  
  ValueProcessor<VP,false>::ValueProcessor(VP* vp):
    owner(vp)
  {    
  }


  template<typename VP>  
  template<typename Access,CellEvent e,int mask,typename X> 
  void ValueProcessor<VP,false>::processValue(X& x){
    vpOper<((Access::writeCacheFlags&e)!=0)       
	   &&! (mask&SKIPWRITECACHE) >::writeCache(owner,x);      
  vpOper<((Access::remoteSetFlags&e)!=0)
	 &&! (mask&SKIPREMOTESET)>::safeSet(owner,x);
  vpOper<((Access::parRemoteSetFlags&e)!=0)
       &&! (mask&SKIPPARREMOTESET)>::discardRetValSafeSet(owner,x);
  vpOper<((Access::signalFlags&e)!=0)  //only signal can throw, and it is the last
	 &&! (mask&SKIPSIGNAL) >::sigW(owner,x);
  }//valueProcessor<false>::processValue

  template<typename VP>  
  void  ValueProcessor<VP,false>::callSignalRemF(typename VP::V::SignalType& sig,const typename VP::V::UT& u){
    VP::V::onRemoteF(sig,u);
  }
  

  template<typename VP>  
  ValueProcessor<VP,true>::ValueProcessor(VP* vp):
    ValueProcessor<VP,false>(vp),
    m("valueProcessor"){        
  }


  template<typename VP>  
  void ValueProcessor<VP,true>::pushUpdate(Callback<void>* c){
    m.lock();
    bool wasFirst=pendingUpdates.size()==0;
    pendingUpdates.push_back(c);
    m.unlock();
    if (wasFirst){
      for (size_t i=0;i<pendingUpdates.size();++i){
	(*pendingUpdates[i])();
      }      
      m.lock();
      pendingUpdates.clear();
      m.unlock();
    }
  
  }
  

  template<typename VP>  
  template<typename Access,CellEvent e,int mask,typename X> 
  void ValueProcessor<VP,true>::processValue(X& x){
    class Updater:public Callback<void>{
      ValueProcessor<VP,false>& valPro;
      typename storableType<X>::value x;
    public:
      Updater(ValueProcessor<VP,true>&pvalPro,const X& px):
	valPro(pvalPro),x(px)
      {	
      }
      void operator()(){
	valPro.processValue<Access,e,mask>(x);
      };
    };//class Updater
    
    pushUpdate(new Updater(*this,x));
  }//ValueProcessor<true>::processValue


  template<typename VP>  
  void  ValueProcessor<VP,true>::callSignalRemF(typename VP::V::SignalType& sig,
						const typename VP::V::UT& u){
    class Updater:public Callback<void>{
      typename VP::V::SignalType& sig;
      typename storableType<typename VP::V::UT>::value u;      
      void operator()(){
	VP::V::onRemoteF(sig,u);
      }
      
    public:
      Updater(typename VP::V::SignalType& psig,const typename VP::V::UT& pu):sig(psig),u(pu){	
      }
      
    };//class Updater
    pushUpdate(new Updater(sig,u));

  }



  ////-------------------VALUEPROXY





  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  template<CellEvent e,int mask,typename X>   
  
  void ValueProxy<PGT,Acc,SigType,remF>::processValue(X& x){
    valueProcessor.processValue<typename V::Access,e,mask>(x);
  }


  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>  
  template<CellEvent e,int mask,typename X> 
  void ValueProxy<PGT,Acc,SigType,remF>::processValue(X& x,const Callback<typename ValueProxy<PGT,Acc,SigType,remF>::V::RUT>* noEx){
      vpOper<((V::Access::writeCacheFlags&e)!=0)
	&&! (mask&SKIPWRITECACHE) >::writeCache(this,x);
      vpOper<((V::Access::remoteSetFlags&e)!=0)
	&&! (mask&SKIPREMOTESET)>::safeSet(this,x);
      vpOper<((V::Access::parRemoteSetFlags&e)!=0)
	&&! (mask&SKIPPARREMOTESET)>::discardRetValSafeSet(this,x);
      vpOper<((V::Access::signalFlags&e)!=0)
	&&! (mask&SKIPSIGNAL) >::sigW(this,x,noEx);
    }


  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  template<CellEvent e,int mask,typename X,typename P> 
  void ValueProxy<PGT,Acc,SigType,remF>::parProcessValue(const X& x,const P& p){
      processValue<e,mask>(x);
      vpOper<((V::Access::parWriteCacheFlags&e)!=0)
	&&! (mask&SKIPPARWRITECACHE)>::parWriteCache(this,x,p);   
      vpOper<((V::Access::parSignalFlags&e)!=0)
	&&! (mask&SKIPPARSIGNAL)>::sigPar(this,x,p);
  }
  

  
  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  
  template<CellEvent e,int mask,typename X,typename P> 
  void ValueProxy<PGT,Acc,SigType,remF>::parProcessValue(const X& x,const P& p,const Callback<typename ValueProxy<PGT,Acc,SigType,remF>::V::RUT>* noEx){
    processValue<e,mask>(x,noEx);
    vpOper<((V::Access::parWriteCacheFlags&e)!=0)
	   &&! (mask&SKIPPARWRITECACHE)>::parWriteCache(this,x,p);   
  vpOper<((V::Access::parSignalFlags&e)!=0)
	 &&! (mask&SKIPPARSIGNAL)>::sigPar(this,x,p,noEx);
  }


  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::remoteAvailable(){    
    exc::MultipleExceptionHolder err;
    try{
      this->onRemoteAvailable();
      }catch(exc::Exception &e){
      err.handle(e);
    }
    
      //first connect,then get, so we do not loose updates


      try{
	vpOper<((ONREMOTE&V::Access::connectFlags)!=0)  &&  ((ONREMOTEANDHASLISTENER&V::Access::connectFlags)==0) >::remoteConnect(this); 
	vpOper<(ONREMOTEANDHASLISTENER&V::Access::connectFlags)!=0>::remoteConnectOnListeners(this);

      }catch(exc::Exception &e){
	err.handle(e);
      }

      try{
	vpOper<((ONREMOTE&V::Access::parConnectFlags)!=0) && ((ONREMOTEANDHASLISTENER&V::Access::parConnectFlags)==0)>::parRemoteConnect(this);
	vpOper<(ONREMOTEANDHASLISTENER&V::Access::parConnectFlags)!=0>::parRemoteConnectOnListeners(this);
      }catch(exc::Exception &e){
	err.handle(e);
      }
      



      try{
	vpOper<(ONREMOTE&(V::Access::writeCacheFlags|
		       V::Access::signalFlags)
		)!=0>::retrieveValue(this);
      }catch(exc::Exception &e){
	err.handle(e);
      }

      try{
	vpOper<(ONREMOTE&(V::Access::parWriteCacheFlags|
		       V::Access::parSignalFlags)
		)!=0>::parRetrieveValue(this);
      }catch(exc::Exception &e){
	err.handle(e);
      }

      try{
	vpOper<(ONREMOTE&V::Access::remoteSetFlags)!=0>::onRemoteAvailableSet(this);
      }catch(exc::Exception &e){
	err.handle(e);
      }

      try{
	vpOper<(ONREMOTE&V::Access::parRemoteSetFlags)!=0>::parOnRemoteAvailableSet(this);
      }catch(exc::Exception &e){
	err.handle(e);
      }
      
      err.check();
      
    }
  


  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::remoteUnset(){//already inside mutex
    this->onRemoteUnset();
      this->acc.upToExclusive();
      vpOper<vpDataSelector<V,ValueProxy>::hasSignal>::clearConn(this);
      vpOper<vpDataSelector<V,ValueProxy>::hasParSignal>::parClearConn(this);
      this->acc.downToShared();
      //vpOper<V::Access::onAvailableFlags&CONNECTONAVAILABLE>::remoteDisconnect(this);
  }


  
  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::notify(const typename ValueProxy<PGT,Acc,SigType,remF>::V::UT& ut){
      processValue<ONUPDATE,SKIPSIGNAL>(ut);
  }
  
  
   template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::parNotify(const typename ValueProxy<PGT,Acc,SigType,remF>::V::UT& ut,
						   const typename ValueProxy<PGT,Acc,SigType,remF>::V::UP& p){
      parProcessValue<ONPARUPDATE,SKIPPARSIGNAL>(ut,p);
   }

  

   template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
     ValueProxy<PGT,Acc,SigType,remF>::ValueProxy(const Void&):
     valueProcessor(this)
     {
     vpOper<vpDataSelector<V,ValueProxy>::hasSignal>::wlinkOwner(this);
   }

  
  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  template<typename T> ValueProxy<PGT,Acc,SigType,remF>::ValueProxy(const T& t):
    valueProcessor(this)
  {
      setCache(t);
      vpOper<vpDataSelector<V,ValueProxy>::hasSignal>::wlinkOwner(this);
    }



   template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
   ValueProxy<PGT,Acc,SigType,remF>::~ValueProxy(){
     SharedUse use(this->acc);
     if (this->remote!=NULL){
       this->remote->onDelete.remove(this);
     }
   }


 template<typename PGT,typename Acc,
	   typename SigType,
	  typename signalFuncType<PGT,Acc,SigType>::value remF>
 typename ValueProxy<PGT,Acc,SigType,remF>::CacheReturn ValueProxy<PGT,Acc,SigType,remF>::get(){          
   vpOper<(V::Access::allowedRemoteless&REMOTELESSGET)==0>::assureConnection(this,"get");      
   return vpOper<V::Access::cacheAttachment!=NOCELLTYPE>::template getCache<CacheReturn>(this); 	
 }


  template<typename PGT,typename Acc,
	   typename SigType,
	  typename signalFuncType<PGT,Acc,SigType>::value remF>
  typename ValueProxy<PGT,Acc,SigType,remF>::ParCacheReturn 
  ValueProxy<PGT,Acc,SigType,remF>::parGet(const typename cacheType<typename ValueProxy<PGT,Acc,SigType,remF>::V,
								    ValueProxy<PGT,Acc,SigType,remF>::V::Access::parCacheAttachment
								    >::CacheParam& p){  
      
      vpOper<(V::Access::allowedRemoteless&REMOTELESSPARGET)==0>::assureConnection(this,"parGet");
      return vpOper<V::Access::parCacheAttachment!=NOCELLTYPE>::template getParCache<ParCacheReturn>(this,p);     
	
    }
  
   

  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::get(typename ValueProxy<PGT,Acc,SigType,remF>::V::GT& g){ //non cached,typed cell style signature  
    this->getData(&g);//this throws
    this->template processValue<ONGET,0>(g);
  }
  

  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::get(typename ValueProxy<PGT,Acc,SigType,remF>::V::GT& g,
					     const typename ValueProxy<PGT,Acc,SigType,remF>::V::GP &p){ //non cached,typed cell style signature     
	this->getData(&g,&p);
	this->template parProcessValue<ONPARGET,0>(g,p);
    }


   template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::
   setCache(const typename cacheType<typename ValueProxy<PGT,Acc,SigType,remF>::V,
				     ValueProxy<PGT,Acc,SigType,remF>::V::Access::cacheAttachment
				     >::CacheType& s){
      vpOper<vpDataSelector<V,ValueProxy>::hasWork>::writeCache(this,s);    
    }

  
 template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
 void ValueProxy<PGT,Acc,SigType,remF>::
 setCache(const typename cacheType<typename ValueProxy<PGT,Acc,SigType,remF>::V,
				   ValueProxy<PGT,Acc,SigType,remF>::V::Access::parCacheAttachment
				   >::CacheType& s,
	  const typename cacheType<typename ValueProxy<PGT,Acc,SigType,remF>::V,
				   ValueProxy<PGT,Acc,SigType,remF>::V::Access::parCacheAttachment
				   >::CacheParam& p)
    {
      
	vpOper<vpDataSelector<V,ValueProxy>::hasParWork>::parWriteCache(this,s,p);      


    }



 template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::retrieveAndSignal(){
   typename V::GT g;
   get(g);
   vpOper<
     vpDataSelector<V,ValueProxy>::hasWork&&
     isConvertible<typename V::UT,typename cacheType<V,V::Access::cacheAttachment>::CacheType>::value
     >::writeCache(this,
		   g
		   );
 }

  

   template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
   void ValueProxy<PGT,Acc,SigType,remF>::signalCache(){ 
      vpOper<isConvertible<typename V::UT,typename cacheType<V,V::Access::cacheAttachment>::CacheType>::value>::signalCache(this);
   }


  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
   void ValueProxy<PGT,Acc,SigType,remF>::signalParCache(const typename ValueProxy<PGT,Acc,SigType,remF>::V::GP &p){
      vpOper<isConvertible<typename V::UT,typename cacheType<V,V::Access::parCacheAttachment>::CacheType>::value>::signalParCache(p);
    }
  

  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
   void ValueProxy<PGT,Acc,SigType,remF>::signalData(const typename ValueProxy<PGT,Acc,SigType,remF>::V::UT& ut){      
      processValue<ONUPDATE,0>(ut);
    }


   template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
   void ValueProxy<PGT,Acc,SigType,remF>::signalData(const typename ValueProxy<PGT,Acc,SigType,remF>::V::UT& ut,
						     const typename ValueProxy<PGT,Acc,SigType,remF>::V::UP& p){
     parProcessValue<ONPARUPDATE,0>(ut,p);
   }

  
  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
   void ValueProxy<PGT,Acc,SigType,remF>::set(const typename ValueProxy<PGT,Acc,SigType,remF>::V::ST& s){
	vpOper<(V::Access::allowedRemoteless&REMOTELESSSET)==0>::assureConnection(this,"set");
	processValue<ONSET,0>(s);
    }


  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::set(typename ValueProxy<PGT,Acc,SigType,remF>::V::SP& sp,
					     const typename ValueProxy<PGT,Acc,SigType,remF>::V::ST& st){
    vpOper<(V::Access::allowedRemoteless&REMOTELESSPARSET)==0>::assureConnection(this,"parSet");
    processValue<ONPARSET,SKIPPARREMOTESET>(st);//the par is the one of get
    this->safeSetData(&sp,&st); //we ask, with SKIPPARREMOTESET,  not to set it remotely (ignoring the return value), and ask it directly here
    //already done in processvalue 
  }


 template<typename PGT,typename Acc,
	   typename SigType,
	  typename signalFuncType<PGT,Acc,SigType>::value remF>
 const typename  ValueProxy<PGT,Acc,SigType,remF>::V::ST& 
 ValueProxy<PGT,Acc,SigType,remF>::operator=(const typename  ValueProxy<PGT,Acc,SigType,remF>::V::ST& s){     
   set(s);
   return s;
 }


  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::setExcept(
						   const typename ValueProxy<PGT,Acc,SigType,remF>::V::ST& s,
						   const Callback<typename ValueProxy<PGT,Acc,SigType,remF>::V::RUT>* noEx){
    vpOper<(V::Access::allowedRemoteless&REMOTELESSSET)==0>::assureConnection(this,"setExcept");
    processValue<ONSET,0>(s,noEx);
      //this->safeSetData(&s); already done in processvalue
  }
  
  
  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
   void ValueProxy<PGT,Acc,SigType,remF>::setExcept(
						    const typename ValueProxy<PGT,Acc,SigType,remF>::V::ST& s,
						    const ConnReference<typename ValueProxy<PGT,Acc,SigType,remF>::V::RUT>& noEx){            
      setExcept(s,noEx.cb);    
    }

  
   template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
   bool ValueProxy<PGT,Acc,SigType,remF>::isToSet(const typename ValueProxy<PGT,Acc,SigType,remF>::V::ST& st){
     return vpOper<V::Access::assureUsesGet>::isToSet(this,st);
   }


  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::assure(const typename ValueProxy<PGT,Acc,SigType,remF>::V::ST& st){
    if (isToSet(st)){
      set(st);	
    }
  }



   template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
   void ValueProxy<PGT,Acc,SigType,remF>::assureExcept(const typename ValueProxy<PGT,Acc,SigType,remF>::V::ST& st,
						       const Callback<typename ValueProxy<PGT,Acc,SigType,remF>::V::RUT>* noEx){
      if (isToSet(st)){
	setExcept(st,noEx);
      }
    }


  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::assureExcept(const typename ValueProxy<PGT,Acc,SigType,remF>::V::ST& st,
						      const ConnReference<typename ValueProxy<PGT,Acc,SigType,remF>::V::RUT>& noEx){
      assureExcept(st,noEx.cb);
    }
  

  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::connect(ConnReference<typename ValueProxy<PGT,Acc,SigType,remF>::V::RUT>& c){   
    vpOper<(V::Access::allowedRemoteless&REMOTELESSCONNECT)==0>::assureConnection(this,"connect");
    vpOper<V::Access::signalFlags!=0>::connect(this,c);
  }


 template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
 void ValueProxy<PGT,Acc,SigType,remF>::connect(Callback<typename ValueProxy<PGT,Acc,SigType,remF>::V::RUT>* c){      
	 vpOper<(V::Access::allowedRemoteless&REMOTELESSCONNECT)==0>::assureConnection(this,"connect");
	 vpOper<V::Access::signalFlags>::connect(this,c);
 }


template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
 void ValueProxy<PGT,Acc,SigType,remF>::connect(ConnReference<typename ValueProxy<PGT,Acc,SigType,remF>::V::RUT>& c,
						const typename ValueProxy<PGT,Acc,SigType,remF>::V::UP& p){

	vpOper<(V::Access::allowedRemoteless&REMOTELESSPARCONNECT)==0>::assureConnection(this,"parConnect");

	vpOper<V::Access::parSignalFlags>::parConnect(this,c,p);

    }


template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
 void ValueProxy<PGT,Acc,SigType,remF>::connect(Callback<typename ValueProxy<PGT,Acc,SigType,remF>::V::RUT>* c,
						const typename ValueProxy<PGT,Acc,SigType,remF>::V::UP& p){

	vpOper<(V::Access::allowedRemoteless&REMOTELESSPARCONNECT)==0>::assureConnection(this,"parConnect");
	vpOper<V::Access::parSignalFlags>::parConnect(this,c,p);

    }



template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
 void ValueProxy<PGT,Acc,SigType,remF>::disconnect(ConnReference<typename ValueProxy<PGT,Acc,SigType,remF>::V::RUT>& c){

	vpOper<(V::Access::allowedRemoteless&REMOTELESSDISCONNECT)==0>::assureConnection(this,"disconnect");
	if (!vpOper<V::Access::signalFlags!=0>::disconnect(this,c)){
	  vpOper<V::Access::parSignalFlags!=0>::parDisconnect(this,c);//some structures may not exist,need to separate
	}

    }



  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  std::string ValueProxy<PGT,Acc,SigType,remF>::getName() const{
    return Typename<typename V::GT>::name+" Value Proxy";
  }

  
  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::handleRemote(exc::Exception& e){
    throw;
  }

  
  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  void ValueProxy<PGT,Acc,SigType,remF>::parHandleRemote(exc::Exception& e,
							 const typename ValueProxy<PGT,Acc,SigType,remF>::V::UP* p){
    throw;
  }


template<typename PGT,typename Acc,
	   typename SigType,
	 typename signalFuncType<PGT,Acc,SigType>::value remF>
  Cell* ValueProxy<PGT,Acc,SigType,remF>::getTypedRemote(CellClient* c,const std::string& cellname){            
  return  new RemoteCell<typename V::Signature>(c,cellname);
}

  
template<typename PGT,typename Acc,
	 typename SigType,
	 typename signalFuncType<PGT,Acc,SigType>::value remF>
int ValueProxy<PGT,Acc,SigType,remF>::getCapabilities() const{
  return TypedCapabilities<
    typename V::GT,typename V::GP,
    typename V::ST,typename V::SP,
    typename V::UT,typename V::UP,
    V::getRequiresParam,
    V::setRequiresParam,
    V::connectRequiresParam
    >::value;	
}

template<typename PGT,typename Acc,
	 typename SigType,
	 typename signalFuncType<PGT,Acc,SigType>::value remF>
const CellProperties* ValueProxy<PGT,Acc,SigType,remF>::getProperties() const{
  return this;
}

  
  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF>
  int ValueProxy<PGT,Acc,SigType,remF>::getCellFlags()const{
      return VALUEPROXY;
    }

}
