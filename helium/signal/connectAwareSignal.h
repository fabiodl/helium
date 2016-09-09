#include "signal.h"
#include <helium/thread/mutex.h>

namespace helium{
  
  
  template <typename D,typename N> 
   class ConnectAwareSignal:public Signal<D>{
    Mutex m;
  public:
    
    class Reference{
      typename Signal<D>::Reference ref;
      friend class ConnectAwareSignal;
    public:
      N n;     
    Reference(const typename Signal<D>::Reference &pref,const N & pn):ref(pref),n(pn){
      }
      bool operator ==(const Reference& b) const{
	return ref==b.ref;
      }
    };
    
    
    Signal<const Reference&> connectSignal;
    Signal<const Reference&> disconnectSignal;
    



    Reference connect(Callback<D> *cb,const N &n){               
      m.lock();
      Reference r(Signal<D>::connect(cb),n);
      connectSignal(r);
      m.unlock();
      return r;
    }

    
    Reference connect(Callback<D> *cb,const N &n){               
      m.lock();
      Reference r(Signal<D>::connect(cb),n);
      connectSignal(r);
      m.unlock();
      return r;
    }
    
    Reference connect(const Callback<D> &cb,const N &n){               
      m.lock();
      Reference r(Signal<D>::connect(cb),n);
      connectSignal(r);
      m.unlock();
      return r;
    }

   Reference disconnect(const Reference& r){               
     m.lock();
     disconnectSignal(r);
     Signal<D>::disconnect(r.ref);
     m.unlock();
     return r;
    }

   void operator()(D d){
     m.lock();
     Signal<D>::operator()(d);
     m.unlock();
   }

  ConnectAwareSignal():m("connAwareS"){}

  };//ConnectedAwareSignal<D,N>



 template <typename N> 
   class ConnectAwareSignal<void,N>:public Signal<void>{
   Mutex m;
 public:
   class Reference{
     typename Signal<void>::Reference ref;
      friend class ConnectAwareSignal;
   public:
     N n;     
     Reference(const typename Signal<void>::Reference &pref,const N & pn):ref(pref),n(pn){
     }
     bool operator ==(const Reference& b) const{
      return ref==b.ref;
    }
   };


   Signal<Reference&> connectSignal;
   Signal<Reference&> disconnectSignal;

   Reference connect(Callback<void> *cb,const N &n){               
     m.lock();
     Reference r(Signal<void>::connect(cb),n);
     connectSignal(r);
     m.unlock();
     return r;
    }

   Reference connect(const Callback<void> &cb,const N &n){               
     m.lock();
     Reference r(Signal<void>::connect(cb),n);
     connectSignal(r);
     m.unlock();
     return r;
    }

   Reference disconnect(const Reference& r){               
     m.lock();
     disconnectSignal(r);
     Signal<void>::disconnect(r.ref);
     m.unlock();
     return r;
    }

   void operator()(){
     m.lock();
     Signal<void>::operator()();
     m.unlock();
   }

 ConnectAwareSignal():m("connAwareS"){}
   
 };// ConnectedAwareSignal<void,N>



 template<typename D,typename C>
   void connectSig(Signal<D> &s,C* c,void (C::*m)()){
   IgnoreParamClassCallback<D,C> cb(c,m);
   s.connect(cb);
 }

}//helium
