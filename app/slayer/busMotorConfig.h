/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_MOTORBASECONFIG
#define HE_MOTORBASECONFIG

#include <helium/robot/motorStructures.h>


namespace helium{
  template<typename MotorSign>
  class BusMotorConfig{
  public:
    IdMapper idMap;
    motor::Calib<MotorSign> calib;  
    std::string serialDevice;
    BusMotorConfig(Memory& mem,const IDPath& confPath){
      dimport(idMap,mem,confPath);
      dimport(calib,mem,confPath); 
      IDPath p=confPath;
      p.downPath(IDPath("motors.bus.device","."));  
      dimport(serialDevice,mem,p);
    }        
  };  
}

#endif
