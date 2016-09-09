#ifndef HE_I2C
#define HE_I2C

#include <helium/core/exception.h>

namespace helium{

  namespace exc{
    
    class I2CException:public Exception{
      
    public:
      I2CException(const std::string& serialPort,const std::string& msg):Exception("I2C communication, device "+serialPort+":"+msg){
      }
      
      virtual void rethrow();
      Exception* clone() const;
     
    };//I2CException
    
    
    
  }//ns exc  
  


}//ns helium


#endif

