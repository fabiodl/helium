#ifndef HE_RESOURCELEASE
#define HE_RESOURCELEASE

#include <helium/thread/mutex.h>
#include <helium/thread/semaphore.h>

//the producer holds the resource, and periodically leases it
namespace helium{

  class ResourceLease{

    Mutex m;
    int waiters;
    int consumers;
    Semaphore dataAvailable;
    
    Semaphore consumerGone;
      

   
  public:
    void producerLease(){
      m.lock();
      if (!waiters){ //for efficiency
	m.unlock();
	return;
      }
      consumers=waiters;
      waiters=0;
      //std::cout<<"signalling "<<consumers<<std::endl;
      dataAvailable.signal(consumers);
      while(consumers>0){
	m.unlock();
	//std::cout<<"waiting "<<consumers<<std::endl;
	consumerGone.wait();
	m.lock();
      }
      m.unlock();
    }
    
    void consumerGet(){
      m.lock();
      waiters++;
      m.unlock();
      //std::cout<<"waiting data"<<std::endl;
      dataAvailable.wait();
      //std::cout<<"use data,boy"<<std::endl;
    }
           
    
    void consumerRemit(){
      m.lock();
      consumers--;
      m.unlock();
      consumerGone.signal();
    }
    
    
    ResourceLease():m("rl"),waiters(0),consumers(0),dataAvailable(-1),consumerGone(-1){
    }
    
  };//class ResourceLease
  
};




#endif
