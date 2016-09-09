#include <helium/core/enum.h>
#include <helium/system/cellMapping.h>
#include "limitFactory.h"

using namespace helium;


typedef enum {vssv410,NUM} MotorType;  

namespace helium{
template<>  struct EnumStrings<MotorType>{
  static const int NUM=1;
  static const char* strings[];
};
}

const char* EnumStrings<MotorType>::strings[]={"vs-sv410"};

LimitFactory::LimitFactory(Memory& mem):mc(NULL){
  MotorType type;
  recursiveMemToField(type,mem,IDPath("robot.motors.type","."));  
 
  switch(type){
  case vssv410:{        
    mc=new Sv410Conversion(mem);

    break;
  }
  default:
    break;
  }
}


LimitFactory::~LimitFactory(){
  if (mc!=NULL)
    delete mc;
}
