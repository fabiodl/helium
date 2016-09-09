#include <helium/parallel/parallel.h>

namespace helium{


ThreadPool::Worker::~Worker(){
  running=false;
  
  join();
}


void ThreadPool::Worker::run(){
  running=true;
  while(running){
    owner.doJob();
  }
}


  void ThreadPool::Worker::stop(){
    running=false;
  }

  ThreadPool::Worker:: Worker(ThreadPool& powner):
    owner(powner)
  {
    start();
  }


ThreadPool:: ThreadPool(size_t cnt):
  m("threadPool"),
  hasWork(-1,0),
  workers(*this,cnt)
{
}



  ThreadPool::~ThreadPool(){
    for (size_t i=0;i<workers.size();++i){
      workers[i].stop();
    }
    class FakeJob:public JobBlock,public Job{
      void operator()(){}
    public:
      FakeJob(int n){
	for (int i=0;i<n;++i){
	  push_back(this);
	}
      }
    };
    
    FakeJob fake(workers.size());
    run(fake);

  }

    

    
 


ThreadPool::JobBlock::JobBlock()  
{
}


void ThreadPool::JobBlock::init(){
  idx=0;
  toGo=jobs.size();
}


bool ThreadPool::JobBlock::getJob(Job* &j){
  j=jobs[idx++];
  return idx==jobs.size();  
}


void ThreadPool::JobBlock::incFinished(){
  toGo--;
  if (!toGo){
    s.signal();
  }
}
  
  void ThreadPool::doJob(){
    while(true){
      m.lock();
      if (blocks.size()) break;   
      m.unlock();
      hasWork.wait();
    }
    JobBlock* jb=blocks.front();
    Job* j;
    if (jb->getJob(j)){
      blocks.pop_front();
    }
    m.unlock();
    
    (*j)();

    m.lock();
    jb->incFinished();
    m.unlock();
  }


void ThreadPool::JobBlock::waitCompletion(){
  s.wait();
}

void ThreadPool::JobBlock::push_back(Job* j){
  jobs.push_back(j);
}

void ThreadPool::run(JobBlock& jb){
  jb.init();
  m.lock();
  blocks.push_back(&jb);
  m.unlock();
  hasWork.signal(jb.jobs.size());
  jb.waitCompletion();
}


}
