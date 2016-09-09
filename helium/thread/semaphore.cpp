/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.02.18
 * \version 0.2
 * Release_flags s
 */

#include <helium/thread/semaphore.h>
#include <helium/thread/thread_private.h>

//note: windows has no "unlimited count" semaphore
//      pthread has no "limited count" semaphore

namespace helium{




  Semaphore::Semaphore(int pmax,int pval):mutex("sem"),maxVal(pmax),value(pval){
#ifdef _WIN32
    changeHappened =CreateSemaphore(NULL,0,1,NULL);    
    if (changeHappened==NULL){ 
      throw exc::SemaphoreException(getLastErrorMsg()); //printLastError
    }
#else

#ifdef __APPLE__
    
    
    do{
	myId=rand() &0x0FFFFFFFFFFFFF ;
	char buffer[128];
	sprintf(buffer,"/%lx",myId);	
	changeHappened=sem_open(buffer,O_CREAT|O_EXCL,0644,0);
    }while((changeHappened==(sem_t*)SEM_FAILED)&&(errno==EEXIST));


    if (changeHappened==(sem_t*)SEM_FAILED){
#else
    if (sem_init(&changeHappened,0,0)==-1){
#endif


      switch(errno){
      case EACCES: //mac
	throw exc::SemaphoreException("The named semaphore exists and the permissions specified by oflag are denied, or the named semaphore does not exist and permission to create the named semaphore is denied.");
      case EEXIST: //mac
	throw exc::SemaphoreException("O_CREAT and O_EXCL are set and the named semaphore already exists.");
      case EINTR: //mac
	throw exc::SemaphoreException("The sem_open() operation was interrupted by a signal.");
      case EMFILE: //mac
	throw exc::SemaphoreException("Too many semaphore descriptors or file descriptors are currently in use by this process.");
      case ENAMETOOLONG: //mac
	throw exc::SemaphoreException("The length of the name string exceeds PATH_MAX, or a pathname component is longer than NAME_MAX while _POSIX_NO_TRUNC is in effect.");
      case ENFILE: //mac
	throw exc::SemaphoreException("Too many semaphores are currently open in the system.");

      case EINVAL:
	throw exc::SemaphoreException("The value argument exceeds SEM_VALUE_MAX.");	
      case ENOENT:
	throw exc::SemaphoreException("O_CREAT is not set and the named semaphore does not exist.");

      case ENOSPC:
	throw exc::SemaphoreException("A resource required to initialise the semaphore has been exhausted, or the limit on semaphores (SEM_NSEMS_MAX) has been reached.");
      case ENOSYS:
	throw exc::SemaphoreException("The function sem_init() is not supported by this implementation.");
      case EPERM:
	throw exc::SemaphoreException("The process lacks the appropriate privileges to initialise the semaphore.");
      default:
	throw exc::SemaphoreException(toString(errno));
      }
    }
#endif
  }


     Semaphore::~Semaphore(){
#ifdef __APPLE__
       char buffer[128];
       sprintf(buffer,"/%lx",myId);
       sem_unlink(buffer);
       sem_close(changeHappened);
#else

#ifdef _WIN32
       CloseHandle(changeHappened);
#else
       sem_destroy(&changeHappened);
#endif

#endif

     }


  void Semaphore::wait(){
    bool took=false;
    while (!took){
      mutex.lock();
      if (value>0){ 
	value--;
	took=true;
      }
      mutex.unlock();
      if (!took){ 
#ifdef _WIN32
	WaitForSingleObject(changeHappened,INFINITE);
#else
#ifdef __APPLE__
	sem_wait(changeHappened);	
#else
	sem_wait(&changeHappened);	
#endif
#endif
      }//if !took      
    }//while     
  }//wait
  

  
  void Semaphore::signal(){
    mutex.lock();
    if (value<maxVal||maxVal<0) value++;    
    mutex.unlock();
#ifdef _WIN32
    ReleaseSemaphore(changeHappened,1,NULL);
#else
#ifdef __APPLE__
    sem_post(changeHappened); 
#else
    sem_post(&changeHappened); 
#endif
#endif    
  }


  void Semaphore::signal(int n){
    mutex.lock();
    if (maxVal>0&&value+n>maxVal){
      n=maxVal-value;
    }
    value+=n;
    //std::cout<<"sem value "<<value<<std::endl;
    mutex.unlock();
    for (int i=0;i<n;i++){
#ifdef _WIN32
      ReleaseSemaphore(changeHappened,1,NULL);
#else
#ifdef __APPLE__
       sem_post(changeHappened); 
#else
      sem_post(&changeHappened); 
#endif
#endif    
    }


  }


};
