#ifndef HE_GYROEXCEPTION
#define HE_GYROEXCEPTION

#include <helium/robot/hardwareException.h>


namespace helium{

  namespace exc{

  template<HwState S=GENERICFAULTYHWSTATE>
  class GyroException:public HardwareDeviceException<UNNAMEDDEVICE,S>{
  public:
    GyroException(const std::string& details=""):HardwareDeviceException<UNNAMEDDEVICE,S>("Gyro",details){}

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new GyroException(*this);
    }
    
  };


  template<HwState S=GENERICFAULTYHWSTATE>
  class SingleGyroException:public HardwareDeviceException<NAMEDDEVICE,S>{
  public:
    SingleGyroException(int id,const std::string& details=""):HardwareDeviceException<UNNAMEDDEVICE,S>("Gyro",id,details){}

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new SingleGyroException(*this);
    }
    
  };


  template<HwState S=GENERICFAULTYHWSTATE>
  class GyroDeviceException:public HardwareDeviceException<NAMEDMULTIPLEXINGDEVICE,S>{
  public:

    GyroDeviceException(int id,const HwIdList& affectedSensor,const std::string& details=""):
      HardwareDeviceException<NAMEDMULTIPLEXINGDEVICE,S>("Gyro",id,affectedSensor,details){}

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new GyroDeviceException(*this);
    }
    
  };

  
  }

}



#endif
