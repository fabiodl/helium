#ifndef HE_HIDRAW
#define HE_HIDRAW

#include <helium/os/usbHid.h>

namespace helium{

  class HidRaw:public UsbHid{
    #ifndef _WIN32
    Timeout timeout;
    #endif
    HidRaw(int vendorId,int productId,Time timeout,const char *manufacturer=NULL, const char* product=NULL);  

    //returns true on data received
    //false on timeout
    template<int N,bool padRead,bool padWrite> bool recvAll(UsbMessage<N,padRead,padWrite>&);    
    
  };
  
  template<int N,bool padRead,bool padWrite> bool HidRaw::recvAll(UsbMessage<N,padRead,padWrite>& m){
      int toRead=padRead?m.completeSize():m.payloadSize();
      const char* dst=m.data+(padRead?0:1);
      while(toRead>0){
#ifndef _WIN32
	if (timeout.getState()==Timeout::TIMEOUT) return false;
#endif
	int r=recv(dst,toRead);
	toRead-=r;
	dst+=r;
      }
    }
    


}

#endif
