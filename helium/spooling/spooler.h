/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SPOOLER
#define HE_SPOOLER

#include <helium/thread/thread.h>
#include <helium/thread/semaphore.h>
#include <helium/spooling/job.h>
#include <helium/vector/array.h>

namespace helium{
  

  class QueueManager{
  public:
    virtual bool shouldDescend()=0;
  };
  
  template <typename Res> class Spooler:public Thread{
    struct JobQueue:public QueueManager{
      std::list<Job<Res>*> l;
      Mutex m;
    
      QueueManager* queueManager;
      
      bool shouldDescend(){return true;}

      JobQueue():m("jobQueue"),queueManager(this){}


      
    };

    Res& res;
    bool running;    
    Array<JobQueue> jobs;
    Semaphore hasJob;  
    int minPriority;
    void run();
  protected:
    const Res& getResource(){
      return res;
    }

    
      
public:
    TsSignal<int> newJob;

    void registerQueueManager(QueueManager*,int p);
    void addJob(Job<Res>* j,int p);    
    void waitJob(WaitableJob<Res>* j,int p);
    void setMinPriority(int lim);
    int getMinPriority();
  
    Spooler(Res& pres,int priorities);

    void stop();
       
    virtual ~Spooler(){
      stop();
    }
    
  };//Spooler

    

}//helium

#include <helium/spooling/spooler.hpp>

#endif
