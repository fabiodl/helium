/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef XPLAYER_VSRC003GYRO
#define XPLAYER_VSRC003GYRO

#include <helium/vstone/usbVsRc003.h>

namespace helium{
class VsRc003Gyro{
  UsbHid hid;
  UsbVsRc003 rc003;
  
  
  SensorPollerInterface<int> polling;
public:
  VsRc003Gyro(Memory& mem,const HwComponentName& name,const IDPath& confPath,const IDPath& servicePath):
    hid(0x1962,0x1000),
    rc003(hid),
    polling(rc003.gyro)
  {
    dexport(mem,polling,servicePath);
  }

  bool isTemporary(){
    return false;
  }

};


}
#endif
