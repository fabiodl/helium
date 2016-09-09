
#include <helium/hiddevice/hidSonyJog.h>

namespace helium{

HidSonyJog::HidSonyJog():
  AbsoluteSonyJog(NULL),
  dev(0x054c,0x000e),
  running(true)
{
};
  
  HidSonyJog::~HidSonyJog(){
    running=false;
    join();
  }

void HidSonyJog::run(){
  UsbMessage<HidSonyJog::KEYBPACKETLENGTH> c;
  Time prevt=getSystemMillis();   
  while(true){
    if (!running) break;
    int r=dev.recv(c);

    Time t=getSystemMillis();
    if (t-prevt>5){
      flush();
    }
    prevt=t;
    for (int i=0;i<r;i++){
      push(c[i]);
    }
  }
}


}
