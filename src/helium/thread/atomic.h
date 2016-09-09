#ifndef _HE_ATOMIC_
#define _HE_ATOMIC_

#include <helium/thread/mutex.h>
namespace helium{

  template<typename T>
  class Atomic{
    T data;
    Mutex m;
  public:    
    Atomic(const Atomic& s):m("atomic"),data(s){
    }

    Atomic():m("atomic"){
    }    
    
    inline T get(){
      m.lock();
      T t=data;
      m.unlock();
      return t;
    }

    operator T(){      
      return get();
    }
    
    const T& operator=(const T& t){
      m.lock();
      data=t;
      m.unlock();
      return t;
    }




  };



}


#endif
