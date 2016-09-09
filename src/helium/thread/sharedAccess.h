/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SHAREDACCESS
#define HE_SHAREDACCESS

#include <helium/thread/semaphore.h>

namespace helium{
  namespace exc{
    class SharedAccessException:public Exception{
    public:
      SharedAccessException(const std::string& s):Exception("Shared access:"+s){
	
      }
      virtual void rethrow(){
	throw *this;
      }

      Exception* clone() const{
	return new SharedAccessException(*this);
      }
    };
		      
  };

  class SharedAccess{
    Mutex state;
    int users;
    Semaphore changeHappened;

    
  public:
    SharedAccess();
    void enterShared();
    void leaveShared();
    void enterExclusive();
    void leaveExclusive();
    void upToExclusive();
    void downToShared();

  };
  
  class SharedUse{
    SharedAccess& acc;
  public: 
    SharedUse(SharedAccess& pacc):acc(pacc){
      acc.enterShared();
    }
    ~SharedUse(){
      acc.leaveShared();
    }
  };


  class ExclusiveUse{
    SharedAccess& acc;
  public: 
    ExclusiveUse(SharedAccess& pacc):acc(pacc){
      acc.enterExclusive();
    }
    ~ExclusiveUse(){
      acc.leaveExclusive();
    }
  };
}

#endif
