/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_TIMER
#define HE_TIMER
#include <helium/os/time.h>
#include <helium/thread/thread.h>


namespace helium{
class PeriodicTimer:public Thread{
  Time p;
  Mutex stateLock;
  enum State{RUNNING,STOPPING,READY} state;

  


  

  virtual void timerRun()=0;

  void run();

public:

  PeriodicTimer(Time pp,bool autoStart=false);
  ~PeriodicTimer();
  void stop();
};

class PosticipableTimer:public Thread{

  bool running;
  Time signalTime;
  Mutex m;
  Semaphore hasWork;

  
  void run();

public:

  PosticipableTimer();
  ~PosticipableTimer();
  
  void rearm(Time timeout);
  void cancel();


  Signal<void> signal;

};




}
#endif
