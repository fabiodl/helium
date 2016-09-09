/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_VSSV410CRCMD
#define HE_VSSV410CRCMD

#include <helium/vstone/vsCrcServo.h>
#include <helium/vstone/vsCrcSerial.h>
#include "busMotorConfig.h"


class VsSv410CrcMd{   
public:
  static const int TIMEOUT=300;
  static const int BAUDRATE=115200;
  typedef helium::VServoSign MotorSign;
  helium::BusMotorConfig<helium::VServoSign> config;
  helium::Serial srl;
  helium::VsCrcSerial<helium::Serial> vs;
  helium::VsCrcServo<helium::VsCrcSerial<helium::Serial> >  motors;

  VsSv410CrcMd(helium::Memory& m,const helium::IDPath& confPath,const helium::IDPath& servicePath):
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
