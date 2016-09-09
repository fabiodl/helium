/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */


#include <helium/thread/thread.h>
#include <helium/thread/thread_private.h>
#include <helium/util/ioUtil.h>
#include <helium/util/mout.h>


namespace helium{

  
  ThreadId getThreadId(){
#ifdef _WIN32
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
  }

  


  
  static exc::ExceptionHandler* globalEh=NULL;
  
  exc::ExceptionHandler* Thread::getGlobalExceptionHandler(){
    return globalEh;
  }

  void Thread::finalize(bool destroy){
  onStop();  
  stateMutex.lock();
  runningExecutions--;
  stateMutex.unlock();
  //std::cout<<"thread body end"<<std::endl;
  terminated.signal();



  /*std::cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<std::endl;
  printTrace();
  std::cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<std::endl;*/


#ifdef PRINTTHREADEVENTS
  mout(msg::NINFO)(stringBegin()<<"Thread "<<this<<" gone"<<stringEnd());
#endif
  if (destroy){
    closeThread();
  }
  }


  void Thread::handle(exc::Exception& e){
    exc::ExceptionHandler* beh=eh;
    if (beh==NULL){
      //std::cout<<"global exception handler"<<std::endl;
      beh=globalEh;
    }else{
      std::cout<<"custom exception handler"<<std::endl;
    }
    if (beh!=NULL){
      beh->handle(e);
    }else{
      std::cout<<"no exception handler"<<std::endl;
      throw;
    }
  }

  void Thread::handle(exc::Exception& e,bool destroy){
    try{
      handle(e);
      finalize(destroy);
    }catch(exc::Exception& ){
      finalize(destroy);
      throw;
    }
  }

  void Thread::stopCurrentThread(){
    finalize(true);
  }

  void Thread::setExceptionHandler(exc::ExceptionHandler* peh){
    eh=peh;
  }

  void Thread::setGlobalExceptionHandler(exc::ExceptionHandler* eh){
    globalEh=eh;
  }
  
  void Thread::execute(bool destroyThread){  
    stateMutex.lock();
    if (runningExecutions==allowedExecutions){
      stateMutex.unlock();
      throw exc::InvalidOperation("Thread allows executions of "+toString(allowedExecutions)+" instances");
    }
    runningExecutions++;
    stateMutex.unlock();
    onStart();
    try{
      run();
      finalize(destroyThread);
    }catch(exc::Exception& e){
      handle(e,destroyThread);    
    }
  }



  ThreadReturn threadHelper(void* arg){
    Thread* t=static_cast<Thread*>(arg);    
    t->execute(true);
    return 0;
  }
  
  
  void Thread::start(){  
    createThread(threadHelper,this);    
  }

  

  




  Thread::Thread(int pallowedExecutions):stateMutex("stateMutex"),runningExecutions(0),eh(NULL),allowedExecutions(pallowedExecutions),deconstructWithoutJoin(false){  
  }

void Thread::join(){
  //  int i=0;
  stateMutex.lock();
  do{
    //std::cout<<"locked"<<++i<<"by "<<getThreadID()<<std::endl;
    if (runningExecutions>0){
      stateMutex.unlock();
      //std::cout<<"unlocked"<<i<<"by "<<getThreadID()<<std::endl;
      terminated.wait();
      stateMutex.lock();
    }
  }while(runningExecutions>0);
  stateMutex.unlock();
}




ThreadReturn runHelper(void* arg){
  Callback<void> *c=trust_cast<Callback<void> *>( arg);
  try{   
    (*c)();
  }catch(...){
  }
  delete c;   
  closeThread();  
  return 0;
}

void Thread::run(const ClonableCallback<void> &c){
  createThread(runHelper,c.clone());    
}




ThreadReturn runCallback(void* arg){
  Callback<void> *c=trust_cast<Callback<void> *>( arg);
  try{   
    (*c)();
  }catch(...){
  }
  closeThread();  
  return 0;
}


  void Thread::run(Callback<void> * c ){
      createThread(runCallback,c);    
  }


};

