#ifndef HE_SEMAPHORENOTIFIER
#define HE_SEMAPHORENOTIFIER


#include <list>
#include <helium/callback/callback.h>
#include <helium/thread/semaphore.h>
namespace helium{

  template<typename T>
  class SemaphoreNotifier:public Callback<T>{
    std::list<Semaphore*> sems;
    void operator()(T){	
      for (std::list<Semaphore*>::iterator it=sems.begin();it!=sems.end();){
	(*it)->signal();
	sems.erase(it++);
      }
    }//operator    
  public:
    void push_back(Semaphore* s){
      sems.push_back(s);
    }
  }; //SemaphoreNotifier


  template<>
  class SemaphoreNotifier<void>:public Callback<void>{
    std::list<Semaphore*> sems;
    void operator()(){	
      for (std::list<Semaphore*>::iterator it=sems.begin();it!=sems.end();){
	(*it)->signal();
	sems.erase(it++);
      }
    }//operator    
  public:
    void push_back(Semaphore* s){
      sems.push_back(s);
    }
  }; //SemaphoreNotifier



}//helium

#endif
