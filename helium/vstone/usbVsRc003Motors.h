#ifndef HE_USBVSRC003MOTORS
#define HE_USBVSRC003MOTORS

#include <helium/vstone/usbVsRc003.h>
#include <helium/robot/motorInterface.h>

namespace helium{

  struct VsUsbMotorSign{
    typedef int Prop;
    typedef int Pos;
    
    static const int INVALIDPROP=0xFFFF;
    static const int INVALIDPOS=0xFFFF;
    static const int FREEPOS=0xFFFE;
    static const int MOVINGPOS=0xFFFD;
    
  };


  class UsbVsRc003Motors:public motors::Raw<VsUsbMotorSign>::Hardware{
    UsbVsRc003& vs;


    void rawRotate(int j,typename Raw::Position val){
      vs.set()
    }

  };



}


#endif
