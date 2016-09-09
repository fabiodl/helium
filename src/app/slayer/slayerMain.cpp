/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include "slayer.h"
#include "spooledMotorSet.h"
#include "serialProbe.h"

#include "vsSv410Md.h"
#include "vsSv410CrcMd.h"
#include "vsSs110Crc.h"
#include "vsRc003gyro.h"
#include "vsCheck.h"

#include "dynamixelMx28Md.h"
#include "dynamixelCheck.h"

#include "bbTouchSensor.h"

#include "androidMd.h"
#include "androidCheck.h"



int main(int argc,char** argv){  
  Slayer slayer;  
  slayer.addComponentType<helium::SpooledMotorSet<VsSv410Md> > ("vs-sv410");
  slayer.addComponentType<helium::SpooledMotorSet<VsSv410CrcMd> > ("vs-sv410crc");
  slayer.addComponentType<helium::VsSs110Crc>("vs-ss110crc");
  slayer.addComponentType<helium::VsRc003Gyro>("vs-rc003gyro");
  slayer.addComponentType<helium::SerialProbe<helium::VsCheck<false> > >("vsProbe");
  slayer.addComponentType<helium::SerialProbe<helium::VsCheck<true> > >("vsCrcProbe");

  slayer.addComponentType<helium::SpooledMotorSet<DynamixelMx28Md> > ("dynamixel-mx28");
  slayer.addComponentType<helium::SerialProbe<helium::DynamixelCheck> > ("dynamixelProbe");


  slayer.addComponentType<helium::SpooledMotorSet<AndroidMd> > ("alabMotor");
  slayer.addComponentType<helium::SerialProbe<AndroidCheck> > ("alabProbe");

  slayer.addComponentType<helium::BusyBitTouchSensor>("bbTouchSensor");


  slayer.launch(argc,argv,"conf/slayer.xml",helium::Application::AS_RUN);
}
