/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_TSSIGNAL
#define HE_TSSIGNAL

#include <list>
#include <helium/signal/signal.h>
#include <helium/thread/mutex.h>


namespace helium{

  template <bool b>
  struct mutexAdapter{
  };

  template <> 
  struct mutexAdapter<false>{
    typedef Mutex MutexType;
    typedef const char* Param;
   static  const char* onNull(){
      return "tss";
    }
  };


  template <>
  struct mutexAdapter<true>{
    typedef Mutex& MutexType;    
    typedef Mutex Param;
    static Mutex& onNull(){
      throw exc::Bug("External Mutex Required");
    }
    
  };


  template <typename D,bool externalMutex=false>
  class TsSignal:public Callback<D>{
    TsSignal(const TsSignal& s);
    TsSignal& operator=(const TsSignal& s);
  protected:
    typename mutexAdapter<externalMutex>::MutexType m;
    std::list<Callback<D>*> cs;
    typedef typename std::list<Callback<D>*>::iterator IT;
  public:
        
    
    TsSignal(typename mutexAdapter<externalMutex>::Param* p=NULL):m(p==NULL?  //used to pass nothing
								    mutexAdapter<externalMutex>::onNull():
								    *p
								    ){
    }


    
    void connect(Callback<D>* cb){
      m.lock();
      cs.insert(cs.end(),cb);
      m.unlock();
    }
  
  void disconnect(Callback<D>* cb){
    m.lock();
    cs.remove(cb);
    m.unlock();
  }


  void connect(ConnReference<D>& r){
    m.lock();
#ifdef DEBUGCONNECTONCE
    if (r.disc!=NULL){
      m.unlock();
      throw exc::Bug("Connecting two times!");
    }
#endif

    r.disc=new IterDiscInfo<D>(cs.insert(cs.end(),r.cb));
    m.unlock();
  }
  
  void disconnect(ConnReference<D> &r){
    m.lock();
    cs.erase(trust_cast<IterDiscInfo<D>*>(r.disc)->it);
    delete r.disc;
    r.disc=NULL;
    m.unlock();
  }
  

    void connect(DisconnectInfo* &disc,Callback<D>* cb){
      m.lock();
      disc=new IterDiscInfo<D>(cs.insert(cs.end(),cb));
      m.unlock();
    }
    
    void disconnect(DisconnectInfo* &disc){
       m.lock();
      cs.erase(trust_cast<IterDiscInfo<D>*>(disc)->it);
      delete disc;
      disc=NULL;
      m.unlock();
    }




 
    //TODO: execute others when one fails, make an exception list
    void operator()(D d){
      m.lock();
      try{
	for (IT i=cs.begin();i!=cs.end();i++){
	  (*(*i))(d); 
	}      
      }catch(...){
	m.unlock();
	throw;
      }
      m.unlock();
    }


    void operator()(D d,const Callback<D>* noExec){
      m.lock();
      try{
	for (IT i=cs.begin();i!=cs.end();i++){
	  if ((*i)!=noExec){
	    (*(*i))(d);
	  }
	}
      }catch(...){
	m.unlock();
	throw;
      }
      m.unlock();
    }

    void operator()(D d,const ConnReference<D>& noExec){
      (*this)(d,noExec.cb);
    }


    void signalAndClear(D d){
      m.lock();
      try{
	for (IT i=cs.begin();i!=cs.end();){
	  (**i)(d);
	  cs.erase(i++);
	}
      }catch(...){
	m.unlock();
	throw;
      }
      m.unlock();
    }
    
    void clear(){
      m.lock();
      cs.clear();
      m.unlock();
    }


   inline int listenerNum(){
    return cs.size();
   } 
};


  template <bool externalMutex>
  class TsSignal<void,externalMutex>:public Callback<void>{
    typename mutexAdapter<externalMutex>::MutexType m;
    std::list<Callback<void>*> cs;
    typedef std::list<Callback<void>*>::iterator IT;
    TsSignal(const TsSignal& s);
    TsSignal& operator=(const TsSignal& s);
 public:

    template <typename M> TsSignal(M &pm):
      m(pm){
    }
    
     TsSignal(typename mutexAdapter<externalMutex>::Param p):
      m(p){
    }
    
    TsSignal(typename mutexAdapter<externalMutex>::Param* p=NULL):m(p==NULL?
								    mutexAdapter<externalMutex>::onNull():
								    *p
								    ){
    }


      

  void connect(ConnReference<void>& r){
    m.lock();
#ifdef DEBUGCONNECTONCE
    if (r.disc!=NULL){
      m.unlock();
      throw exc::Bug("Connecting two times!");
    }
#endif

    r.disc=new IterDiscInfo<void>(cs.insert(cs.end(),r.cb));
    m.unlock();
  }
  
  void disconnect(ConnReference<void> &r){
    m.lock();
    cs.erase(trust_cast<IterDiscInfo<void>*>(r.disc)->it);
    delete r.disc;
    r.disc=NULL;
    m.unlock();
  }


  void connect(Callback<void>* cb){
    m.lock();
    cs.insert(cs.end(),cb);
    m.unlock();
  }
  
  void disconnect(Callback<void>* cb){
    m.lock();
    cs.remove(cb);
    m.unlock();
  }


    void operator()(){
      m.lock();
      try{
	for (IT i=cs.begin();i!=cs.end();i++){
	  (**i)();
	}
      }catch(...){
	m.unlock();
	throw;
      }
      m.unlock();
    }



    void operator()(const Callback<void>* noExec){
      m.lock();
      try{
	for (IT i=cs.begin();i!=cs.end();i++){
	  if ((*i)!=noExec){
	    (*(*i))();
	  }
	}
      }catch(...){
	m.unlock();
	throw;
      }
      m.unlock();
    }


    void operator()(const ConnReference<void>& noExec){
      (*this)(noExec.cb);
    }
  

    void signalAndClear(){
      m.lock();
      try{
	for (IT i=cs.begin();i!=cs.end();){
	  (**i)();
	  cs.erase(i++);
	}
      }catch(...){
	m.unlock();
	throw;
      }
      m.unlock();
    }
    
    void clear(){
      m.lock();
      cs.clear();
      m.unlock();
    }
    
   inline int listenerNum(){
    return cs.size();
   } 
};



}
#endif
