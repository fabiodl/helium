#ifndef HE_CRCEXCEPTION
#define HE_CRCEXCEPTION

#include <helium/core/exception.h>


namespace helium{
  
  namespace exc{
    
    
    class CRCException:public Exception{
    public:
      
      CRCException(unsigned char expected,unsigned char got);      
      CRCException(const unsigned char* msg,int size);//size includes the crc,which is the last char

      
      virtual void rethrow();
      
      virtual Exception* clone() const;
      
      
  };

  }//ns exc

}//ns he

#endif
