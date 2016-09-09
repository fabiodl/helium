/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/util/mout.h>
#include <helium/debug/trace.h>
namespace helium{

  template<typename Res> 
  void Spooler<Res>::run(){
    std::cout<<"    Spooler "<<this<<" started"<<std::endl;
    //std::cout<<"**Spooler "<<this<<" started "<<std::endl;
    running=true;
    bool toDel=false;
    bool pHasJob=false;
    while(running){
      int p=(int)jobs.size()-1;
      while(p>=minPriority){
	jobs[p].m.lock();	      
	//std::cout<<"Spooler "<<this<<"queue "<<p<<" with "<<jobs[p].l.size()<<" jobs "<<std::endl;
	typename std::list<Job<Res>*>::iterator it=jobs[p].l.begin();	
	bool pHadJobs=jobs[p].l.size()>0;
	while (it!=jobs[p].l.end()){
	  Job<Res>* job=*it;
	  jobs[p].m.unlock();		
	  try{
	    //std::cout<<"."<<std::flush;
	    //std::cout<<"Spooler "<<this<<" job "<<job<<"@"<<p<<std::endl;
	    //int x=rand();
	    //std::cout<<"exc ["<<x<<std::endl;
	    toDel=(*job)(res);
	    //std::cout<<x<<"]"<<std::endl;
	    //std::cout<<" toDel="<<toDel<<std::endl;
	  }catch(exc::Exception& e){
	    try{
	      handle(e);	   
	    }catch(exc::Exception& f){	     
	      merr(msg::ERR)("The exception handler is not able to handle {"+toString(f)+"}");
	    }//catch f
	    toDel=job->isToCancel();
	  }//catch e
	  jobs[p].m.lock();
	  if (toDel){
	    jobs[p].l.erase(it++);		   
	  }else{
	    ++it;
	  }		
	}//while it	    
	pHasJob=jobs[p].l.size()>0;
	jobs[p].m.unlock();
	if (pHasJob ||  (
			 pHadJobs&& !jobs[p].queueManager->shouldDescend()
			 ) 
	    ){
	  break;
	}
	p--;
      }//while p
      
      if (p<minPriority){
	//std::cout<<"**Spooler "<<this<<" wating jobs "<<std::endl;
	hasJob.wait();
	//std::cout<<"**Spooler "<<this<<" livinf "<<std::endl;
      }
    }//while running 	   
    //std::cout<<"    Spooler "<<this<<" ended"<<std::endl;
  }//run

}
#include <helium/os/time.h>
namespace helium{


  template<typename Res> 
  void Spooler<Res>::addJob(Job<Res>* j,int p){
    //Time t=helium::getSystemMillis();
    //std::cout<<this<<"new job "<<j<<" pushed "<<p<<std::endl;
    jobs[p].m.lock();
    jobs[p].l.push_back(j);
    jobs[p].m.unlock();      
    hasJob.signal();
    newJob(p);
    /*std::cerr<<"the job"<<j<<" is "<<std::endl;
    printTrace();
    */


    //std::cout<<(helium::getSystemMillis()-t)<<std::flush;
    //std::cout<<this<<"new job pushed "<<p<<std::endl;
    //std::cout<<"Pushed "<<j<<", Queue"<<p<<"has now "<<jobs[p].l.size()<<" jobs"<<std::endl;    
  }

  

  template<typename Res> 
  void Spooler<Res>::waitJob(WaitableJob<Res>* j,int p ){
    addJob(j,p);
    if (p<minPriority){
      merr(msg::WARNING)("Waiting a job whose priority is lower than the minimum!: probable lock");
#ifdef TRACEDEBUG
      printTrace();
#endif
    }

    //std::cout<<"waiting job "<<p<<std::endl;
    j->wait();
    //std::cout<<"job done "<<p<<std::endl;
  }

  
  template<typename Res> 
   Spooler<Res>::Spooler(Res& pres,int priorities):res(pres),running(false),jobs(priorities),minPriority(0){}


  template<typename Res> 
  void Spooler<Res>:: setMinPriority(int lim){
    jobs.back().m.lock(); //setting the priority is a "job" at the highest priority
    minPriority=lim;
    std::cout<<"@@@ Spooler "<<this<<" priority is"<<lim<<std::endl;
    jobs.back().m.unlock();
    hasJob.signal();
  }


  template<typename Res> 
  int Spooler<Res>::getMinPriority(){
    jobs.back().m.lock();
    int r=minPriority;
    jobs.back().m.unlock();
    return r;
  }


   template<typename Res> 
   void Spooler<Res>::registerQueueManager(QueueManager* cb,int p){
     jobs[p].m.lock();
     jobs[p].queueManager=cb;
     jobs[p].m.unlock();

   }


  template<typename Res> 
  void Spooler<Res>::stop(){
    running=false;
    hasJob.signal();
    join();
  }

}
