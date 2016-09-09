/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_THREAD_PRIVATE
#define HE_THREAD_PRIVATE
#include <helium/core/exception.h>
#include <helium/util/ioUtil.h>
#include <helium/os/winError.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <errno.h>  //for errors at creation
#endif

#include <sstream>

namespace helium{

#ifdef _WIN32  
  typedef DWORD ThreadId;
#define ThreadReturn DWORD WINAPI
  typedef LPTHREAD_START_ROUTINE ThreadRoutine;
#else
  typedef pthread_t ThreadId; 
  typedef void * ThreadReturn;
  typedef  void* (*ThreadRoutine)(void *);
#endif



 


  
  inline void createThread(ThreadRoutine function,void* parameter){
#ifdef _WIN32
    if (CreateThread(NULL,0,function,parameter,0,NULL)==NULL){
      throw exc::ThreadException(getLastErrorMsg());
    }
#else
    pthread_t tid;
    if (pthread_create(&tid,NULL,function,parameter)){
      switch(errno){
      case EAGAIN:
	throw exc::ThreadException("The system lacked the necessary resources to create another thread, or the system-imposed limit on the total number of threads");	
      case EINVAL:
	throw exc::ThreadException("The attribute value is invalid");
      case EPERM:
	throw exc::ThreadException("The caller does not have appropriate permission to set the required scheduling parameters or scheduling policy");
      default:
	throw exc::ThreadException(toString(errno));
      break;
      }
    }
#endif
  }


  inline void closeThread(){
#ifdef _WIN32
    ExitThread(0);
#else
    pthread_detach(pthread_self());
    pthread_exit(NULL);
#endif
  }

}


#endif
