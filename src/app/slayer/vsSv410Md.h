/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_VSSV410MD
#define HE_VSSV410MD

#include <helium/vstone/vServo.h>
#include "busMotorConfig.h"


class VsSv410Md{   
public:
  static const int TIMEOUT=30;
  static const int BAUDRATE=115200;
  typedef helium::VServoSign MotorSign;
  helium::BusMotorConfig<helium::VServoSign> config;
  helium::Serial srl;
  helium::VstoneSerial vs;
  helium::Vservo motors;

  VsSv410Md(helium::Memory& m,const helium::IDPath& confPath,const helium::IDPath& servicePath):
    config(m,confPath),    
    srl(config.serialDevice.c_str(),BAUDRATE,TIMEOUT),
    vs(srl),
    motors(vs,config.idMap,config.calib)
  {

  }

  bool isTemporary(){
    return false;
  }

};




#endif
