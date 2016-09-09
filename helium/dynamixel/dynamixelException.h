/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_DYNAMIXELEXCEPTION
#define HE_DYNAMIXELEXCEPTION

#include <helium/robot/hardwareException.h>

namespace helium{
  
  extern std::string describePacket(const unsigned char* packet);

  namespace exc{



    class DynamixelException:public HardwareException{ 
    public:
      DynamixelException(const unsigned char* packet,const std::string& d);      
      void rethrow();      
      Exception* clone();
    };


    class DynamixelCsumException:public DynamixelException{
    public:
      DynamixelCsumException(const unsigned char* packet,unsigned char expected,unsigned char got); 
      void rethrow();      
      Exception* clone();
    };


    class DynamixelStatusError:public DynamixelException{
    public:
      DynamixelStatusError(const unsigned char* packet,unsigned char errorflags); 
      void rethrow();      
      Exception* clone();
    };

  }//exc

}//helium




#endif
