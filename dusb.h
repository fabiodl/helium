#ifndef _MEGANKRO_H_
#define _MEGANKRO_H_

#include <helium/os/usbHid.h>
#include <stdint.h>
class Dusb:public helium::UsbHid{
public:
  static const size_t NUMKEYS=24;

  Dusb();
  
  void set(uint8_t* keys);
  
};

#endif
