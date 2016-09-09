/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_DYNAMIXELMX28MD
#define HE_DYNAMIXELMX28MD

#include <helium/dynamixel/dynamixelMotor.h>
#include <helium/dynamixel/dynamixelSerial.h>
#include "busMotorConfig.h"

class DynamixelMx28Md{   
public:
  static const int TIMEOUT=300;
  typedef helium::dynamixel::Mx28Sign MotorSign;
  helium::BusMotorConfig<helium::dynamixel::Mx28Sign> config;

  helium::Serial srl;
  helium::DynamixelSerial<helium::Serial> dsrl;
  helium::DynamixelMotor<helium::DynamixelSerial<helium::Serial>,helium::dynamixel::Mx28Sign> motors;

  int getBaudRate(helium::Memory& m,const helium::IDPath& confPath){
    int br;
    helium::IDPath p(confPath);
    p.downPath(helium::IDPath("motors.bus.baudrate","."));
    dimport(br,m,p);    
    return br;
  }


  bool isTemporary(){
    return false;
  }
  
  DynamixelMx28Md(helium::Memory& m,const helium::IDPath& confPath,const helium::IDPath& servicePath):
    config(m,confPath),    
    srl(config.serialDevice.c_str(),getBaudRate(m,confPath),TIMEOUT),
    dsrl(srl,getMax(config.idMap.rawId)),    
    motors(dsrl,config.idMap,config.calib)
  {

  }
};




#endif
