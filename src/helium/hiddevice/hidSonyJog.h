#ifndef HE_HIDSONYJOG_
#define HE_HIDSONYJOG_

#include <helium/device/sonyJog.h>
#include <helium/os/usbHid.h>

namespace helium{

class HidSonyJog:public AbsoluteSonyJog{
  UsbHid dev;
  bool running;
public:


  HidSonyJog();
  ~HidSonyJog();
  void run();
  
};

}

#endif
