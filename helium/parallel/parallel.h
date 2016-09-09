#ifndef _HE_PARALLEL
#define _HE_PARALLEL

#include <helium/thread/thread.h>
#include <helium/signal/signal.h>
#include <helium/vector/array.h>

namespace helium{

  class ThreadPool{

    typedef Callback<void> Job;

    class Worker:public Thread{
      ThreadPool& owner;
      volatile bool running;
      void run();
    public:
      Worker(ThreadPool&);
      void stop();
      ~Worker();
    };





  public:    
    

    class JobBlock{
    public:
      JobBlock();
      void push_back(Job* j);
    private:
      void waitCompletion();
      void init();
      friend class ThreadPool;
      bool getJob(Job* &j);//returns if it's the last
      void incFinished();      
      size_t idx,toGo;
      std::vector<Job*> jobs;
      Semaphore s;
    };

    
    ThreadPool(size_t cnt);
    ~ThreadPool();
   
    void run(JobBlock& );


    void doJob();//called by the threads
    
  private:
    
    Mutex m;
    std::list<JobBlock*> blocks;
    Semaphore hasWork;
    Array<Worker,
	  array::RESIZABLE,
	  array::InternalAllocator<ParamAllocator<Worker,ThreadPool&> >
	  > workers;



  };






}




#endif
