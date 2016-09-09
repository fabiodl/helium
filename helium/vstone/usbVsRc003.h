/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_USBRC003
#define HE_USBRC003


#include <helium/os/usbHid.h>
#include <helium/robot/hwInterface.h>
#include <helium/robot/sensorInterface.h>

namespace helium{
  class UsbVsRc003{
   
    UsbHid& usb;
    
    struct Protocol{
      UsbMessage<64> readPacket;
      UsbMessage<64> writePacket;
      UsbMessage<64> tmp;
      Protocol();
    } protocol;

    inline int getLastTmp(int i){
      return ((int)protocol.tmp[6+2*i]&0xff)|(((char)protocol.tmp[6+2*i+1])<<8); 
    }
    
  public:
    static const int VENDOR_ID;
    static const int PRODUCT_ID;

    UsbVsRc003(UsbHid& usb);

    void request(int addr,int length);



    void set(int addr,const int* data,int length);//length is the num of ints
    
    class Gyro:public sensor::Set<int>{
      UsbVsRc003& owner;
    public:
      int rawRead(int idx);
      void rawRead(int*);
      void rawRead(int*, const HwIdList&);
      void rawRead(SensorValList<int>& , const HwIdList&);
      void getToken(Token& t, const HwIdList& toRead);
      void rawRead(int*,const Token& t);

      size_t size() const;
      Gyro(UsbVsRc003& powner);
      HwState isWorking(int id);
      void isWorking(Array<HwState>& ,const HwIdList&);
      void getWorkingOptimizedSplit(Array <WorkingToken>& tk,const HwIdList &) const;
      void isWorking(Callback<const HwStateNotification&>*,const WorkingToken&);

    };
    friend class Gyro;

    Gyro gyro;


    void synchyInit();
    void setMainPower(bool);
    void rotate(int *);//expects 64 ints

  };


}


#endif
