#ifndef HE_JUNCTION
#define HE_JUNCTION

#include <helium/thread/mutex.h>




template<typename Pass,typename Act,typename PP,void (Act::*invokef)(PP),typename RP,void (Act::*connectf)(RP),typename DP,void (Act::*disconnectf)(DP)> class Junction{
  Pass* pass;
  Act* act;
  Mutex m;
  friend class JunctionActiveSide;

public:

  Junction(Act& pact):pass(NULL),act(NULL),m("Junction"){
  }



  
  void connect(Pass& ppass,Act& pact,RP& rp){
    pass=&ppass;
    act=&pact;
    (pact.*registerf)(rp);
  }
  
  void disconnectPass(){
    m.lock();
    pass=NULL;
    m.unlock();
  }
  

  void disconnectAct(){
    m.lock();
    act=NULL;
    m.unlock();
  }

};



#endif
