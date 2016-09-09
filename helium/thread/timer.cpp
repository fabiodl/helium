/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#include <helium/thread/timer.h>

namespace helium{
  
  void PeriodicTimer::run(){
    stateLock.lock();
    if (state==READY){
      state=RUNNING;
    }
    stateLock.unlock();
   
    Time start,now;
    start=getSystemMillis();        
    while(state==RUNNING){
      now=getSystemMillis();     
      //std::cout<<t-prevt<<" vs "<<p<<std::endl;
      if (now-start<p){
	milliSleep((int)(p-(now-start)));
      }
      start=getSystemMillis();     
      timerRun();
    }//running    
  }

  PeriodicTimer::PeriodicTimer(Time pp,bool autoStart):p(pp),stateLock("trSL"),state(READY){//false becaue timerRun will likely use subclass' members
    if (autoStart) start();
  }

   PeriodicTimer::~PeriodicTimer(){
    std::cout<<"destroying timer"<<std::endl;
    join();
  }
  
  void PeriodicTimer::stop(){
    stateLock.lock();
    state=STOPPING;
    stateLock.unlock();
    join();
  }  

  void PosticipableTimer::run(){
    m.lock();
    while(running){
      if (signalTime==0){
	m.unlock();
	//std::cout<<"timer waiting for being armed"<<std::endl;
	hasWork.wait();
	m.lock();
      }else{
	Time toSleep=signalTime-getSystemMillis();
	if (toSleep>0){
	  m.unlock();
	  //std::cout<<"sleeping for"<<toSleep<<std::endl;
	  milliSleep((int)(toSleep));
	  //std::cout<<"woke up"<<std::endl;
	  m.lock();
	}else{
	  signalTime=0;
	  m.unlock();
	  //std::cout<<"no time to sleep!"<<std::endl;
	  signal();
	  m.lock();

	}	
      }
    }
    m.unlock();
  }

  void PosticipableTimer::rearm(Time timeout){
    m.lock();
    //std::cout<<"rearming, wait "<<timeout<<std::endl;
    signalTime=getSystemMillis()+timeout;      
    m.unlock();
    hasWork.signal();
  }

  void PosticipableTimer::cancel(){
    m.lock();
    signalTime=0;
    m.unlock();
  }

  PosticipableTimer::PosticipableTimer():running(true),signalTime(0),m("mPositicipableTimer"){   
    start();
  }


  PosticipableTimer::~PosticipableTimer(){
    m.lock();
    running=false;
    m.unlock();
    hasWork.signal();
    join();
  }


};
