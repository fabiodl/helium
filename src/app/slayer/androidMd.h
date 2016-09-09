/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags s
 */

#ifndef _SLAYER_ANDROIDMOTOR
#define _SLAYER_ANDROIDMOTOR

#include "busMotorConfig.h"
#include <helium/android/androidMotor.h>

class AndroidMd{
public:

  typedef helium::AndroidMotor::Sign MotorSign;

  static const int TIMEOUT=-1;
  static const int BAUDRATE=500000;
  helium::BusMotorConfig<helium::AndroidMotor::Sign> config;
  helium::Serial srl;
  helium::AndroidMotor motors;

  AndroidMd(helium::Memory& m,const helium::IDPath& confPath,const helium::IDPath& servicePath):
    config(m,confPath),    
    srl(config.serialDevice.c_str(),BAUDRATE,TIMEOUT),
    motors(srl,config.idMap,config.calib)
  {
  }

};



#endif
