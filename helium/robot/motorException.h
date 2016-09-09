/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_MOTOREXCEPTION
#define HE_MOTOREXCEPTION

#include <helium/robot/hardwareException.h>

namespace helium{

  namespace exc{
  template<HwState S=GENERICFAULTYHWSTATE>
  class MotorException:public HardwareDeviceException<UNNAMEDDEVICE,S>{
  public:
    MotorException(const std::string& details=""):
      HardwareDeviceException<UNNAMEDDEVICE,S>("Motor",details){}

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new MotorException(*this);
    }
    
  };




  template<HwState S=GENERICFAULTYHWSTATE>
  class SingleMotorException:public HardwareDeviceException<NAMEDDEVICE,S>{
  public:
    SingleMotorException(int id,const std::string& details=""):HardwareDeviceException<NAMEDDEVICE,S>("Motor",id,details){}

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new SingleMotorException(*this);
    }
    
  };



    template<HwState S=GENERICFAULTYHWSTATE>
    class MultipleMotorException:public HardwareDeviceException<MULTIPLEXINGDEVICE,S>{
      HwIdList list;
    public:
     MultipleMotorException(const HwIdList& plist,const std::string& details=""):HardwareDeviceException<MULTIPLEXINGDEVICE,S>("Motor",list,details),list(plist){}
      
      virtual void rethrow(){
	throw *this;
      }
      
      virtual Exception* clone() const{
	return new MultipleMotorException(*this);
      }
      
      ~MultipleMotorException() throw(){}
      
  };




  }


}


#endif
