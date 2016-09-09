/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_HIDBBTOUCHSENSOR
#define HE_HIDBBTOUCHSENSOR

#include <helium/signal/signal.h>
#include <helium/vector/array.h>
#include <helium/thread/thread.h>
#include <helium/os/usbHid.h>
#include <fstream>



namespace helium{

  class HidBusyBitTouchSensor:public Thread,public UsbHid{

  public:
    HidBusyBitTouchSensor(int dev,int addr=0x00,int n=8,bool autoStart=true);
    Signal<const unsigned char*> onSensorUpdate;
    Signal<exc::Exception*> onError;
    ~HidBusyBitTouchSensor();
    size_t size();//number of sensors
  private:
    int n,dev,addr,length;
    volatile bool running;
    UsbMessage<array::RESIZABLE> inbuffer,outbuffer;
    void run();
    void requestRead(int offset);
    unsigned char implicitCrc;
    unsigned char getImplicitCrc();
  };
  
};




#endif
