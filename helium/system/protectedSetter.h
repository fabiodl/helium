/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_PROTECTEDSETTER
#define HE_PROTECTEDSETTER

#include <helium/signal/tsSignal.h>
//#include <helium/util/debug.h>

namespace helium{

  template<typename ST,
	   typename RUT,
	   typename Reason,
	   Reason remoteReason,typename V>

class ProtectedSetter:public ConnReference<RUT>,public Callback<RUT>{
    V& v;
    Mutex acc;
    Signal<std::pair<RUT,Reason> > onChange;

    bool enabled; //to switch off the signal when we set from here
    
    void operator()(RUT ut){
      acc.lock();
      if (enabled){
	acc.unlock();
	onChange(std::make_pair(ut,remoteReason));
      }else{
	acc.unlock();
      }
    }

    void setRemote(const ST& st){
      enabled=false;
      acc.unlock();
      v.set(st);
      acc.lock();
      enabled=true;      
    }

    void setRemoteExcept(const ST& st,const Callback<RUT>* cb){
      enabled=false;
      acc.unlock();
      v.setExcept(st,cb);
      acc.lock();
      enabled=true;      
    }




  public:  
    ProtectedSetter(V& pv):ConnReference<RUT>(this),
			   v(pv),acc("ps"),enabled(true){
    }
    
  void connect(ConnReference<std::pair<RUT,Reason> >& ref){
    if (onChange.listenerNum()==0){
      v.connect(*this);//the ConnReference
    }
    MutexLock lock(acc);
    onChange.connect(ref);
  }
    
    void disconnect(ConnReference<std::pair<RUT,Reason> >& ref){
      MutexLock lock(acc);
      v.onChange.disconnect(ref);
      if (onChange.listenerNum()==0){
	v.disconnect(*this);//the ConnReference
      }
    }

    template<Reason r>
    void set(const ST& st){
      MutexLock lock(acc);
      setRemote(st);
      onChange(std::make_pair(st,r));
    }

    template<Reason r>
    void assure(const ST& st){
      if (v.isToSet(st)){
	set<r>(st);
      }
    }
    
    template<Reason r>
    void setExcept(const ST& st,const Callback<std::pair<RUT,Reason> >* cb){
      MutexLock lock(acc);
      setRemote(st);
      onChange(std::make_pair(st,r),cb);
    }

    template<Reason r>
    void setExcept(const ST& st,const ConnReference<std::pair<RUT,Reason> >& ref){
      setExcept<r>(st,ref.cb);
    }


    template<Reason r>
    void setExcept(const ST& st,const Callback<RUT>* cb){
      MutexLock lock(acc);
      setRemoteExcept(st,cb);
      onChange(std::make_pair(st,r));
    }

    template<Reason r>
    void setExcept(const ST& st,const ConnReference<RUT>& ref){
      setExcept<r>(st,ref.cb);
    }



    template<Reason r,typename X>
    void assureExcept(const ST& st,const X& x){
      if (v.isToSet(st)){
	setExcept<r>(st,x);
      }
    }    
    

    

};





}
#endif
