/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_JOBSHOULDRUN
#define HE_JOBSHOULDRUN

#include <helium/spooling/spooler.h>

namespace helium{
  template<typename T> 
  class  JobShouldRun{
    bool isRunning,shouldRun;

  public:
    JobShouldRun():isRunning(false),shouldRun(false){
    }

    //use either these 2

    inline void setShouldRun(bool hasWork){
      shouldRun=hasWork;
    }

    void startIfNecessary(Spooler<T>& spool,Job<T>* job,int priority){
      if (shouldRun&&!isRunning){
	spool.addJob(job,priority);
	isRunning=true;
      }
    }


    //or this
    
    inline void setRunning(Spooler<T>& spool,Job<T>* job,int priority,bool hasWork){
      setShouldRun(hasWork);
      startIfNecessary(spool,job,priority);      
    }





   inline  bool mustExit(){
      if (!shouldRun){
	isRunning=false;
      }
      return !shouldRun;
    }

  };

  }//ns helium



#endif
