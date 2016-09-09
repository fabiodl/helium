
/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */


#ifndef HE_MUTEX
#define HE_MUTEX

#include <string>
#include <helium/thread/thread_public.h>

namespace helium{



  class Mutex{    
    friend bool isMutexOnDebug(helium::Mutex& m);
    const std::string name;
    ThreadId owner;
    MutexObj mutex;
    Mutex(const Mutex&);
  public:
    Mutex(const std::string& name);
    ~Mutex();
    void lock();
    void unlock();
  };

  
  class CountMutex{    
    friend bool isMutexOnDebug(helium::CountMutex& m);
    std::string name;
    ThreadId owner;
    MutexObj mutex;
    int requiredUnlocks;
    CountMutex(const CountMutex&);
  public:
    CountMutex(const std::string& name);
    ~CountMutex();
    void lock();
    void unlock();
  };


 

  class MutexLock{
    Mutex &m;
    MutexLock(const MutexLock&);
  public:  
    MutexLock(Mutex &pm):m(pm){
      m.lock();
    }
    
    ~MutexLock(){
      m.unlock();
    }
  };

  class CountMutexLock{
    CountMutex &m;
    CountMutexLock(const CountMutexLock&);
  public:  
    CountMutexLock(CountMutex &pm):m(pm){
      m.lock();
    }
    
    ~CountMutexLock(){
      m.unlock();
    }
  };


};
#endif
