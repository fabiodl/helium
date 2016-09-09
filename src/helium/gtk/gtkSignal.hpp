/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

namespace helium{
  





  /*template<typename D,typename DS,bool lastValue,bool safe>
    void GtkSignal::notifyValue(std::pair<DS,const Callback<D>*>& it);*/


  

  template<typename D,typename DS,bool lastValue,bool safe>
  void GtkSignal<D,DS,lastValue,safe>::notifyValue(typename SignalledElement<D,DS>::Type& it){


    	if (it.second==NULL){
	  for (typename std::list<Callback<D>*>::iterator cit=cs.begin();cit!=cs.end();++cit){
	    m.unlock();
	    try{
	      (**cit)(it.first);	
	    }catch(exc::Exception& e){
	      GtkExceptionHandling::getDefaultHandler()->handle(e);
	    }
	    m.lock();
	  }
	}else{
	  for (typename std::list<Callback<D>*>::iterator cit=cs.begin();cit!=cs.end();++cit){
	    m.unlock();
	    if (*cit!=it.second){
	      try{
		(**cit)(it.first);	
	      }catch(exc::Exception& e){
		GtkExceptionHandling::getDefaultHandler()->handle(e);
	      }
	    }
	    m.lock();
	  }
	}

    
  }


template<typename D,typename DS,typename Signal>
void DataList<D,DS,Signal,false>::sendUpdates(Signal* s){


  for (typename std::list<typename SignalledElement<D,DS>::Type >::iterator it=vals.begin();it!=vals.end();++it){
    s->notifyValue(*it);
  }
  vals.clear();    
}

  
  template<typename D,typename DS,typename Signal>
  void DataList<D,DS,Signal,true>::sendUpdates(Signal* s){  
    s->notifyValue(val);
  }


  template<typename D,typename DS,bool lastValue,bool safe>
  void  GtkSignal<D,DS,lastValue,safe>::operator()(){
    m.lock();
    vals.sendUpdates(this);
    m.unlock();
  }


  template<typename D,typename DS,bool lastValue,bool safe>
  void GtkSignal<D,DS,lastValue,safe>::onRemote(D d){
    m.lock();    
    vals.push_back(typename SignalledElement<D,DS>::Type(d,(const Callback<D>*)NULL));    
    m.unlock();    
    GtkSignalling::signalling->requestProcessing(this);
  }

   

  template<typename D,typename DS,bool lastValue,bool safe>
  void GtkSignal<D,DS,lastValue,safe>::operator()(D d){
    if (safe){
      onRemote(d);
    }else{
      m.lock();
      vals.push_back(typename SignalledElement<D,DS>::Type(d,(const Callback<D>*)NULL));    
      m.unlock();      
      (*this)();
    }
  }
  


template<typename D,typename DS,bool lastValue,bool safe>
void GtkSignal<D,DS,lastValue,safe>::operator()(D d,const Callback<D>* noEx){
  m.lock();
  vals.push_back(typename SignalledElement<D,DS>::Type(d,noEx));
  m.unlock();
  if (safe){
     GtkSignalling::signalling->requestProcessing(this);    
  }else{
    (*this)();
  }
}

template<typename D,typename DS,bool lastValue,bool safe>
void GtkSignal<D,DS,lastValue,safe>::operator()(D d,const ConnReference<D>& noEx){
  (*this)(d,noEx.cb);
}


template<typename D,typename DS,bool lastValue,bool safe>
void GtkSignal<D,DS,lastValue,safe>::connect(Callback<D>* cb){
  m.lock();
  cs.insert(cs.end(),cb);
  m.unlock();
}

template<typename D,typename DS,bool lastValue,bool safe>
void GtkSignal<D,DS,lastValue,safe>::connect(ConnReference<D> & c){
      m.lock();
      c.disc=new IterDiscInfo<D>(cs.insert(cs.end(),c.cb));
      m.unlock();
    }


template<typename D,typename DS,bool lastValue,bool safe>
void GtkSignal<D,DS,lastValue,safe>::disconnect(ConnReference<D> &r){
      m.lock();
      cs.erase(trust_cast<IterDiscInfo<D>*>(r.disc)->it);
      r.clearDisc();
      m.unlock();
    }


template<typename D,typename DS,bool lastValue,bool safe>
size_t GtkSignal<D,DS,lastValue,safe>::listenerNum(){
  MutexLock lock(m);
  return cs.size();
}

template<typename D,typename DS,bool lastValue,bool safe>
GtkSignal<D,DS,lastValue,safe>::GtkSignal():m("gtkSignalM"){
 
}
  
template<typename D,typename DS,bool lastValue,bool safe>
GtkSignal<D,DS,lastValue,safe>::~GtkSignal(){
  GtkSignalling::signalling->cancelProcessing(this);
}
};
