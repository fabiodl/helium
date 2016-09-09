/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */


#ifndef HE_THREAD
#define HE_THREAD

#include <helium/core/exception.h>
#include <helium/thread/thread_public.h>
#include <helium/thread/mutex.h>
#include <helium/thread/semaphore.h>
#include <helium/signal/signal.h>
#include <helium/signal/tsSignal.h>


namespace helium{

  ThreadId getThreadId();

class Thread{
private:
  void finalize(bool destroy);

  Semaphore terminated;
protected:
  Mutex stateMutex; 
private:
  int runningExecutions;
  exc::ExceptionHandler* eh;
 private:
  int allowedExecutions;
  void handle(exc::Exception& e,bool destroyThread);
 protected:
  virtual void run()=0;
  void handle(exc::Exception& e); //call the best exception handler
  void stopCurrentThread();
  bool deconstructWithoutJoin;
 public:  
  void setExceptionHandler(exc::ExceptionHandler* eh);
  inline exc::ExceptionHandler* getExceptionHandler(){return eh;}
  static void setGlobalExceptionHandler(exc::ExceptionHandler* eh);
  static exc::ExceptionHandler* getGlobalExceptionHandler();

  void execute(bool destroyThread=false); //runs in the current thread
  void start();
  Signal<void> onStart,onStop;  
  Thread(int allowedExecutions=1); 
  virtual ~Thread(){
    //std::cout<<"calling join fom ~Thread"<<std::endl;
    if (!deconstructWithoutJoin){
      join();
    }
  }
  void join();
  int getRunningExecutions(){return runningExecutions;}

  static void run(Callback<void> *) ;

  static void run(const ClonableCallback<void> &c);//the callback is copied and denstructed internally


};

  template<typename C,void (C::*f)()=&C::run()>
  class Runner:public Thread{
    C& c;
    void run(){
      (c.*f)();
    }
  public:
    Runner(C& pc):c(pc){      
    }
    
    ~Runner(){
      join();
    }

  };


};//helium


#endif
