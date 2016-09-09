/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_THREADPUBLIC
#define HE_THREADPUBLIC

#ifdef _WIN32
typedef void* HANDLE;
typedef unsigned long DWORD;
#else
#include <semaphore.h>
#include <pthread.h>
#endif

namespace helium{
#ifdef _WIN32  
  typedef DWORD ThreadId; 
  static const ThreadId NO_THREAD=0;
#else
  typedef pthread_t ThreadId;
  static const ThreadId NO_THREAD=(ThreadId)-1;
#endif

#ifdef _WIN32
  typedef HANDLE MutexObj;
  typedef HANDLE SemaphoreObj;
#else
  typedef pthread_mutex_t MutexObj;

#ifdef __APPLE__
  typedef sem_t* SemaphoreObj;
#else
  typedef sem_t SemaphoreObj;
#endif
#endif


}

#endif
