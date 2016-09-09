#ifndef HE_INTERRUPTIBLEWAITINGJOB
#define HE_INTERRUPTIBLEWAITINGJOB

#include <helium/spooling/spooler.h>
#include <helium/signal/staticConnReference.h>
#include <helium/thread/timer.h>

namespace helium{
  
  template<typename T>
  class InterruptibleWaitingJob:public QueueManager,public Callback<int>{
    Spooler<T>& spool;
    
    PosticipableTimer timer;
    Time period;
    Semaphore shouldReturn;
    bool nothingToDo;
    bool isWaiting;
    int myPriority;

    Mutex m;
    

  public:
    InterruptibleWaitingJob(Spooler<T>& pspool,int priority,Time pperiod):
      spool(pspool),
      period(pperiod),     
      isWaiting(false),
      myPriority(priority),
      m("iwj")
    {      
      timer.signal.connect(&shouldReturn);
      spool.registerQueueManager(this,priority);
      spool.newJob.connect(this);
    }

    bool shouldDescend(){
      //std::cout<<"should descend "<<myPriority<<"?"<<std::endl;
      m.lock();
      nothingToDo=true;
      isWaiting=true;
      m.unlock();
      //Time t=getSystemMillis();
      //std::cout<<"startWaiting "<<std::endl;
      timer.rearm(period);
      shouldReturn.wait();
      //std::cout<<"stopWaiting "<<(getSystemMillis()-t)<<std::endl;
      {
	MutexLock lock(m);
	//std::cout<<"returing "<<nothingToDo<<std::endl;
	isWaiting=false;
	return nothingToDo;
      }
    }

    void operator()(int p){
      m.lock();      
      if (isWaiting){
	isWaiting=false;
	timer.cancel();
	shouldReturn.signal();
	nothingToDo=(p<myPriority);
	//std::cout<<"new job "<<nothingToDo<<"("<<p<<"<"<<myPriority<<")"<<std::endl;
      }
      m.unlock();

    }


    ~InterruptibleWaitingJob(){
      //spool.join();
    }


  };


}


#endif
