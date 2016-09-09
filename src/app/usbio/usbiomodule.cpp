#include "./usbiomodule.h"
#include <iostream>
#include<new>
#include <helium/hiddevice/hidUSBIO.h>

namespace fake{
namespace helium{
  namespace avrPort{
    enum Name{A};
  }

  class HidUSBIO{
  public:
    HidUSBIO(int p){
      std::cout<<"new usbio with "<<p<<" ports thr"<<std::endl;      
    }

    void setDirection(avrPort::Name p,unsigned char mask){
      std::cout<<"set direction of "<<p<<" to "<<(int)mask<<std::endl;
    }

    void setOutput(avrPort::Name p,unsigned char mask){
      std::cout<<"set output of "<<p<<" to "<<(int)mask<<std::endl;
      throw 4;
    }
    


  };

}
}//ns fake

//using namespace fake;

void* new_usbio(){  
  try{
    return new(std::nothrow) helium::HidUSBIO(4);
  }catch(...){
    return NULL;
  }
    
}

void delete_usbio(void* vu){
  operator delete(vu,std::nothrow);
}


int setDirection(void* vu,uint8_t port,uint8_t value){
  try{
    helium::HidUSBIO* u=reinterpret_cast<helium::HidUSBIO*>(vu);

    u->setDirection
      (
       static_cast<helium::avrPort::Name>(port),
       value
       );
  }catch(...){
    return -1;
  }
  return 0;
}



int setOutput(void* vu,uint8_t port,uint8_t value){
  try{
    helium::HidUSBIO* u=reinterpret_cast<helium::HidUSBIO*>(vu);

    u->setOutput
      (
       static_cast<helium::avrPort::Name>(port),
       value
       );
  }catch(...){
    return -1;
  }
  return 0;
}
