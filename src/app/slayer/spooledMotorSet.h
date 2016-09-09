/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_GENERICMOTORSET
#define HE_GENERICMOTORSET

#include <helium/quick/motorModeSet.h>
#include <helium/system/cell.h>
#include <helium/robot/motorExceptionHandler.h>
#include <helium/robot/motorPropertyManager.h>

namespace helium{
//MotorDevice 
//  - defines MotorSign
//  - has a constructor that takes memory, confPath and servicePath
//  -has a member motors that can be spooled


template<typename MotorDevice> class SpooledMotorSet{
public:
  typedef typename MotorDevice::MotorSign MotorSign;

  SpooledMotorSet(Memory& m,const std::string& name,const helium::IDPath& confPath,const IDPath& servicePath);  
  bool isTemporary();

private:
  MotorDevice mot;
  MotorSpooler<MotorSign> ms;
  RealTimeSource rts;
  MotorModeSet<MotorSign> motModeSet;
  MotorExceptionHandler<MotorSign>  meh;
  MotorPropertyManager<MotorSign> mpm;  
  void setCurrentState();

};

}

#include "spooledMotorSet.hpp"

#endif
