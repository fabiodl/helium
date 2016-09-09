/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */


#ifndef _WIN32
#include <sys/time.h>
#include <unistd.h>
#else
#include <windows.h>
#endif
#include <helium/os/time.h>
#include <helium/core/exception.h>
#include <cstdio>
namespace helium{
  void milliSleep(int t){
#ifdef _WIN32
    Sleep(t);
#else
    usleep(1000*t);
#endif
    
  }

 Time getSystemMillis(){
#ifdef _WIN32
  LARGE_INTEGER lpFrequency,lpPerformanceCount;
  if (!QueryPerformanceFrequency(&lpFrequency)){
    throw exc::InvalidOperation("Precision timer not supported");
  }else{
    QueryPerformanceCounter(&lpPerformanceCount);      
  }
  return (long long)(lpPerformanceCount.QuadPart*1000/lpFrequency.QuadPart);
#else  
  struct timeval tv;
  gettimeofday(&tv ,NULL);
  return static_cast<Time>(tv.tv_sec*1000+(double)tv.tv_usec/1000);    
#endif
}


Time getElapsedMillis(Time t){
#ifdef _WIN32
  LARGE_INTEGER lpFrequency,lpPerformanceCount;
  if (!QueryPerformanceFrequency(&lpFrequency)){
    throw exc::InvalidOperation("Precision timer not supported");
  }else{
    QueryPerformanceCounter(&lpPerformanceCount);      
  }
  return (Time)(lpPerformanceCount.QuadPart*1000/lpFrequency.QuadPart);
#else
  struct timeval tv;
  gettimeofday(&tv ,NULL);
  return static_cast<Time>(tv.tv_sec*1000+(double)tv.tv_usec/1000)-t;    
#endif
}

 std::string getAsciiDateTime(){
    char buffer[256];
#ifdef _WIN32
    SYSTEMTIME t;  
    GetLocalTime(&t);
    sprintf(buffer,"%04d%02d%02d_%02d%02d%02d",
	    t.wYear,t.wMonth,t.wDay, 
	    t.wHour,t.wMinute,t.wSecond);
#else
    time_t t;//in secs
    time(&t);
    strftime(buffer,256,"%Y%m%d_%H%M%S",localtime(&t));
#endif
    return buffer;
  }

}
