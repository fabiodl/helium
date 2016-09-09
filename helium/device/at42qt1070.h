#ifndef HE_AT42QT1070
#define HE_AT42QT1070



namespace helium{


  template<typename I2C>

  class AT42QT1070{
  public:
    enum AddrTable{
      DEVICEADDR=0x1B,
      CHIPID=0,
      FIRMWAREVER=1,
      DETECTIONSTATUS=2,
      KEYSTATUS=3,
      KEYSIGNAL=4, //2 bytes per key, MSB, LSB
      REFERENCEDATA=18, //2 bytes per key, MSB, LSB
      NEGATIVETHRESHOLD=32, //1 bytes per key
      AVERAGINGADIACENTKEY=39, //1 bytes per key
      DECTECTIONINTEGRATOR=46,//1 bytes per key
      FASTOUTMAXCALGUARDCHAN=53,
      LOWPOWERMODE=54,
      MAXONDURATION=55,
      CALIBRATE=56,
      RESET=57
    };


    AT42QT1070(I2C& );

    
    void reset();
    void calibrate();
    unsigned char getKeyStatus();
    void setThreshold(unsigned char ch,unsigned char val);
    void setDetectionIntegrator(unsigned char ch,unsigned char val);
    void setAveAks(unsigned char ch, unsigned char ave,unsigned char aks);
    void setLowPower(unsigned char steps);
  private:
        I2C& i2c;
    void set (unsigned char addr,unsigned char val);
    void set (unsigned char addr,unsigned char  ch,unsigned char val);
  };



}//ns helium


#include <helium/device/at42qt1070.hpp>

#endif
