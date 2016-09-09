/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */


#include <helium/robot/knownPosNormalizer.h>
#include <helium/robot/genericMotorSign.h>
#include  <helium/vstone/vServo.h>
#include <helium/dynamixel/dynamixelSign.h>
#include <helium/android/androidMotorSign.h>





namespace helium{

  template<> struct EnumStrings<KnownPosNormalizer::MotorType>{
   static const int NUM=3;
   static const char* strings[NUM];       
};

  const char* EnumStrings<KnownPosNormalizer::MotorType>::strings[EnumStrings<KnownPosNormalizer::MotorType>::NUM]={"vs-sv410","dynamixel-mx28","alab-android"};


template<typename MotorSign> 
class StaticLinearJointPosConverter:public  JointPosConverter{    
  RelMotorSign::Pos getRelPos(AbsMotorSign::Pos p,AbsMotorSign::Pos zero){
    return MotorSign::TONORM*(p-zero);
  }
  

  virtual double getAbsVelocity(double relVelocity){
    return MotorSign::TORAW*relVelocity;
  }
};





  KnownPosNormalizer::KnownPosNormalizer(Memory& mem){  
    dimport(type,mem,IDPath("robot.motors.type",".")); 
  }
  
  JointPosConverter* KnownPosNormalizer::getNewJointPosConverter(){
    switch(type){
    case VSSV410:
      return new StaticLinearJointPosConverter<VServoSign>();
    case DYNAMIXELMX28:
      return new StaticLinearJointPosConverter<dynamixel::Mx28Sign>();
    case ALABANDROID:
      return new StaticLinearJointPosConverter<android::MotorSign>();

    }
    return NULL;
  }  

  
  


}
