#include <helium/simul/simulMotorAdapter.h>
#include <helium/robot/motorInterface.h>
namespace helium{

  const SimulMotorSign::Pos SimulMotorSign::INVALIDPOS=NAN;
  const SimulMotorSign::Pos SimulMotorSign::INVALIDPROP=-100;
  
  const SimulMotorSign::Pos SimulMotorSign::MOVINGPOS=-200;
  const SimulMotorSign::Pos SimulMotorSign::FREEPOS=-300;
  





  /* motor::Interface::Position SimulNullNormalizer::normPos(SimulMotorSign::Pos value,int joint){
  return value;
}
 
SimulMotorSign::Pos SimulNullNormalizer::unnormPos(motor::Interface::Position value,int joint){
  return value;
}



SimulNullNormalizer& SimulNullNormalizer::get(){
  static SimulNullNormalizer nn;
  return nn;
}


motor::Interface::Property SimulPropNormalizer::normProp( SimulMotorSign::Prop value,const motor::PropertyRequest& r){
	throw exc::InvalidOperation("Property "+toString(r)+" not available for simulated normalizer");
  }


SimulMotorSign::Prop SimulPropNormalizer::unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r){
  throw exc::InvalidOperation("Property "+toString(r)+" not available for simulated normalizer");
}

SimulPropNormalizer& SimulPropNormalizer::get(){
  static SimulPropNormalizer spn;
  return spn;
}

  */

}//ns helium
