/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_EXITHANDLER
#define HE_EXITHANDLER

#include <helium/signal/destroyableSignal.h>
#include <helium/thread/semaphore.h>

namespace helium{



  class ExitHandler{
    DestroyableSignal<void>  onGlobalExitRequest;
    DestroyableSignal <void> onGlobalExitExecution;
    friend class ExitHandlerCreator;            
    volatile bool systemRunning;    
  public:    
    template<typename T> void exitRequestConnect(T& t){onGlobalExitRequest.connect(t);}
    template<typename T> void exitExecutionConnect(T& t){onGlobalExitExecution.connect(t);}
    template<typename T> void exitRequestDisconnect(T& t){onGlobalExitRequest.disconnect(t);}
    template<typename T> void exitExecutionDisconnect(T& t){onGlobalExitExecution.disconnect(t);}
    //static bool isSystemRunning();
    static ExitHandler& get();
    
  private:    
    //do not use
    ~ExitHandler();
    ExitHandler();
  };


  class Junction;
  //Create an object of this type to wait its deconstruction before exiting
  class OnExitWait:public Callback<void>{    
    Semaphore s;
    void operator()();
    DestroyableSignal<void>::SignalReference cr;

  public:
    OnExitWait();
    ~OnExitWait();
    void signalCompletion();//usually not required.for special cases.     
  };
  
}//helium



#endif
