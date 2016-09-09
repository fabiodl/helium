/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */


#include <helium/debug/debugOptions.h>
#include <helium/thread/thread_private.h>
#include <helium/thread/thread.h>
#include <helium/thread/mutex.h>

#ifdef MUTEXONDEBUG
namespace helium{
  bool isMutexOnDebug(Mutex& m){

    return m.name=="vp::remoteAccess";
  }

  bool isMutexOnDebug(CountMutex& m){

    return m.name=="vp::remoteAccess";
  }

}
#endif //ifdef MUTEXONDEBUG


namespace helium{



  inline void mutexInit(MutexObj& mutex,const std::string& name){
#ifdef _WIN32
    mutex = CreateMutex(NULL,FALSE,NULL);
    if (mutex==NULL) throw exc::MutexException(name,getLastErrorMsg());
#else
    if (pthread_mutex_init(&mutex,NULL)){
      switch(errno){
      case EAGAIN:
	throw exc::MutexException(name,"The system lacked the necessary resources (other than memory) to initialise another mutex");
      case ENOMEM:
	throw exc::MutexException(name,"Insufficient memory exists to initialise the mutex");
      case EPERM:
	throw exc::MutexException(name,"The caller does not have the privilege to perform the operation");
      case EBUSY:
	throw exc::MutexException(name,"The implementation has detected an attempt to re-initialise the object referenced by mutex, a previously initialised, but not yet destroyed, mutex.");
      case EINVAL:
	throw exc::MutexException(name,"The value specified by attr is invalid.");
      default:
	throw exc::MutexException(name,toString(errno));
      }
    }
#endif
  }//mutexInit


  inline void mutexDestroy(MutexObj& mutex){
#ifdef _WIN32
    CloseHandle(mutex);
#else
    pthread_mutex_destroy(&mutex);
#endif
    

  }


  Mutex::Mutex(const std::string& pname):name(pname),owner(NO_THREAD){
    mutexInit(mutex,name);
  }

  Mutex::~Mutex(){
    mutexDestroy(mutex);
  }

  void Mutex::lock(){

    ThreadId me=getThreadId();
    if (owner == me ){
      throw exc::MutexException(name,"Multiple lock on the same mutex");
    }    
    

#ifdef _WIN32
    WaitForSingleObject(mutex,INFINITE);
#else
    pthread_mutex_lock(&mutex);
#endif





#ifdef MUTEXONDEBUG
    if (isMutexOnDebug(*this)){
      std::cout<<"Thread "<<me<<" got the mutex"<<std::endl;
    }
#endif


    owner= me;    
  }


  void Mutex::unlock(){

#ifdef MUTEXONDEBUG
    if (isMutexOnDebug(*this)){
      std::cout<<"Thread "<<getThreadId()<<" unreleasing the mutex owned by"<<owner<<std::endl;
    }
#endif


    if (getThreadId()!=owner){
      throw exc::MutexException(name,"Unlocking unowned mutex");
    }

    owner=NO_THREAD;

#ifdef _WIN32
    ReleaseMutex(mutex);
#else
    pthread_mutex_unlock(&mutex);
#endif




  }


  CountMutex::CountMutex(const std::string& pname):name(pname),owner(NO_THREAD),requiredUnlocks(0){
    mutexInit(mutex,name);
  }

  CountMutex::~CountMutex(){
    mutexDestroy(mutex);
  }

  void CountMutex::lock(){
    ThreadId me=getThreadId();
    if (owner!=me){
#ifdef _WIN32
      WaitForSingleObject(mutex,INFINITE);
#else
      pthread_mutex_lock(&mutex);
#endif
      owner=me;
    }
#ifdef MUTEXONDEBUG
    if (isMutexOnDebug(*this)){
      std::cout<<this<<"at enter,value="<<requiredUnlocks<<std::endl;
    }
#endif

    requiredUnlocks++;
#ifdef MUTEXONDEBUG
    if (isMutexOnDebug(*this)){
      std::cout<<this<<"Thread "<<getThreadId()<<" inc requiredUnLocks to "<<requiredUnlocks<<std::endl;
    }
#endif
  }

  void CountMutex::unlock(){
    if (owner!= getThreadId()){
      throw exc::MutexException(name,"Unlocking unowned mutex, owner="+toString(owner)+" unlocker="+toString(getThreadId()));      
    }

    

#ifdef MUTEXONDEBUG
    if (isMutexOnDebug(*this)){
      std::cout<<this<<"value="<<requiredUnlocks<<std::endl;
    }
#endif


    requiredUnlocks--;
#ifdef MUTEXONDEBUG
    if (isMutexOnDebug(*this)){
      std::cout<<this<<"Thread "<<getThreadId()<<" dec requiredUnLocks to "<<requiredUnlocks<<std::endl;
    }
#endif
    if (requiredUnlocks==0){      
      owner=NO_THREAD;
#ifdef _WIN32
      ReleaseMutex(mutex);      
#else
      pthread_mutex_unlock(&mutex);
#endif
    }
#ifdef MUTEXONDEBUG
    if (isMutexOnDebug(*this)){
      std::cout<<this<<"at exit,value="<<requiredUnlocks<<std::endl;
    }
#endif

  }//unlock
  

}
