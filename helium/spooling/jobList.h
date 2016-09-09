#ifndef HE_JOBLIST
#define HE_JOBLIST

#include <helium/spooling/job.h>

namespace helium{
  

  template<typename T>
  class JobList; //to implement 


  template<>
  class JobList<void>{
    Mutex m;
    std::list<Job<void>* > toExecute;



  public:


    JobList():m("joblist"){
    }

    virtual void onAddJob(){}

    void addJob(Job<void>* job){
      m.lock();
      toExecute.push_back(job);
      m.unlock();
      onAddJob();
    }
    
    void waitJob(WaitableJob<void> * w){
      addJob(w);
      w->wait();
    }


    void operator()(){
      m.lock();
      for (std::list<Job<void>* >::iterator it=toExecute.begin();
	   it!=toExecute.end();){	
	try{
	  if ((**it)()){
	    toExecute.erase(it++);
	  }else{
	    ++it;
	  }
	}catch(exc::Exception& e){
	  if ((*it)->isToCancel()){
	    toExecute.erase(it++);
	  }else{
	    ++it;
	  }
	}//catch	
      }//for std
      m.unlock();
    }//operator
  };//JobList


}//ns helium




#endif
