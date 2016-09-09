/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.02.18
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SEMAPHORE
#define HE_SEMAPHORE

#include <helium/signal/callback.h>
#include <helium/thread/thread_public.h>
#include <helium/thread/mutex.h>
#include <helium/core/exception.h>

namespace helium{



  class Semaphore:public Callback<void>{
#ifdef __APPLE__
      long unsigned int myId;
#endif
  protected:
    SemaphoreObj changeHappened;
    Mutex mutex;    
    int maxVal;
    int value;
  public:
    Semaphore(int maxval=1,int val=0);
    ~Semaphore();
    void operator()(){
      signal();
    }
    void wait();
    void signal();
    void signal(int n);
    
  };

  




  class EhSemaphore:public Semaphore,public exc::ExceptionHandler{ 
    exc::ExceptionHolder eh;
  public:
    EhSemaphore(int maxval=1,int val=0):Semaphore(maxval,val){
    }
    void handle(exc::Exception& e){
      eh.handle(e);
      signal();
    }
    
    void wait(){
      Semaphore::wait();
      eh.check();
    }

  };

  template<bool EH> struct SemaphoreType{
  };


  template<> struct SemaphoreType<false>{
    typedef Semaphore value;
  };

  template<> struct SemaphoreType<true>{
    typedef EhSemaphore value;
  };

}
#endif
