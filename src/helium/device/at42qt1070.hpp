#include <helium/device/at42qt1070.h>
#include <helium/util/byteConversion.h>
#include <helium/os/time.h>
namespace helium{

  template<typename I2C>
  AT42QT1070<I2C>::AT42QT1070(I2C& pi2c):
    i2c(pi2c){
  }


  template<typename I2C>
  void AT42QT1070<I2C>::set(unsigned char addr,unsigned char val){
    unsigned char buff[2];
    buff[0]=addr;
    buff[1]=val;
    i2c.write(buff,2);
  }

  template<typename I2C>
  void AT42QT1070<I2C>::set(unsigned char addr,unsigned char ch,unsigned char val){
    unsigned char buff[2];
    buff[0]=addr+ch;
    buff[1]=val;
    i2c.write(buff,2);
  }


  template<typename I2C>
  void AT42QT1070<I2C>::reset(){
    set(RESET,1);
    milliSleep(250);//the device takes 125ms
  }

  template<typename I2C>
  void AT42QT1070<I2C>::calibrate(){
    set(CALIBRATE,1);
  }

  
  template<typename I2C>
  unsigned char AT42QT1070<I2C>::getKeyStatus(){
    unsigned char r=KEYSTATUS;
    i2c.write(&r,1);
    i2c.read(&r,1);
    return r;
  }

  template<typename I2C>
  void AT42QT1070<I2C>::setAveAks(unsigned char ch, unsigned char ave,unsigned char aks){
    set(AVERAGINGADIACENTKEY,ch,(ave<<2)|aks);
  }

  template<typename I2C>
  void  AT42QT1070<I2C>::setThreshold(unsigned char ch,unsigned char val){
    set(NEGATIVETHRESHOLD,ch,val);
  }
  
  template<typename I2C>
  void  AT42QT1070<I2C>::setDetectionIntegrator(unsigned char ch,unsigned char val){
    set(DECTECTIONINTEGRATOR,ch,val);
    
  }
  

  template<typename I2C>
  void  AT42QT1070<I2C>::setLowPower(unsigned char steps){
    set(LOWPOWERMODE,steps);
    
  }


  

};
