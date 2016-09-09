/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_TOUCHSENSOREXCEPTION
#define HE_TOUCHSENSOREXCEPTION

#include <helium/robot/hardwareException.h>


namespace helium{

  namespace exc{


    template<HwState S=GENERICFAULTYHWSTATE>
    class TouchSensorException:public HardwareDeviceException<UNNAMEDDEVICE,S>{
    public:
      TouchSensorException(const std::string& details=""):HardwareDeviceException<UNNAMEDDEVICE,S>("Touch sensor",details){}
      
      virtual void rethrow(){
	throw *this;
      }
      
      virtual Exception* clone() const{
      return new TouchSensorException(*this);
    }
      
  };



  template<HwState S=GENERICFAULTYHWSTATE>
  class SingleTouchSensorException:public HardwareDeviceException<NAMEDDEVICE,S>{
  public:
    SingleTouchSensorException(int id,const std::string& details=""):HardwareDeviceException<NAMEDDEVICE,S>("Touch sensor",id,details){}

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new SingleTouchSensorException(*this);
    }
    
  };


  template<HwState S>
  class TouchSensorBoardException:public HardwareDeviceException<NAMEDMULTIPLEXINGDEVICE,S>{
  public:

    TouchSensorBoardException(int id,const HwIdList& affectedTouchsensor,const std::string& details=""):HardwareDeviceException<NAMEDMULTIPLEXINGDEVICE,S>("Touch sensor board",id,affectedTouchsensor,details){}

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new TouchSensorBoardException(*this);
    }
    
  };

  
  }

}



#endif
