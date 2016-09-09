#ifndef HE_BUSPROTOCOL
#define HE_BUSPROTOCOL

#include <helium/os/serial.h>

namespace helium{
  
  class VstoneSerial{
    Serial& s;
  public:
    void send(int id,const unsigned char* data,int size);
    void recv(int id,const unsigned char* data,int size);
    void romWrite(int id,const unsigned char* data,int size);
    
  };

}


#endif
