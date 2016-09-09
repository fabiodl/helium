#ifndef HE_ODESIMULMOTORADAPTER
#define HE_ODESIMULMOTORADAPTER

#include <helium/simul/simulMotorAdapter.h>
#include <helium/vstone/vServoSign.h>
#include <helium/wrap/ode/odeStructure.h>
#include <helium/math/tAbs.h>
namespace helium{
  
  struct FakeTemp:public Array<double>{
    FakeTemp(size_t n):Array<double>(n){
      setAll(*this,20);
    }
  };

  template<>
  class SimulPropNormalizer<ODERobot,VServoSign>:public motor::Raw<VServoSign>::PropNormalizer{
    SimulMotorAdapter<ODERobot,VServoSign>& owner;
  public:

    typedef  void* ParamPtr;



    motor::Interface::Property normProp( VServoSign::Prop value,const motor::PropertyRequest& req){
      switch(req.id){
      case motor::TEMP:
      case motor::POWER:
	return value;	  
	  break;
      case motor::POTENTIOPOSITION:
      case motor::TARGETPOSITION:
	//std::cout<<"normalizing here"<<std::endl;
	return owner.convPos().normPos(value,req.joint);
      default:
	throw exc::InvalidOperation("No known conversion for"+toString(req.id));
      }
    }


    VServoSign::Prop  unnormProp(motor::Interface::Property value,const motor::PropertyRequest& req){
      switch(req.id){
      case motor::TEMP:
      case motor::POWER:
	return value;	  
	  break;
      case motor::POTENTIOPOSITION:
      case motor::TARGETPOSITION:
	return owner.convPos().unnormPos(value,req.joint);
      default:
	throw exc::InvalidOperation("No known conversion for"+toString(req.id));
      }
    }

    ParamPtr check(ParamPtr p){
      if (p==NULL) throw exc::Bug("not initializing SimulPropNormalizer<ODERobot,VServoSign>");
    }
    
    SimulPropNormalizer( SimulMotorAdapter<ODERobot,VServoSign>& powner,ParamPtr):
      owner(powner){
    }
    
  };

  

  template<>
  void SimulMotorAdapter<ODERobot,VServoSign>::rawGetProperty(VServoSign::Prop& o, const motor::PropertyRequest & req){
    static FakeTemp prevTemp(changed.size());
    static const double alpha=5E-1;
    switch(req.id){
      case motor::TEMP:
	o=prevTemp[req.joint]*(1-alpha)+
	  alpha*(20+10/0.41*tAbs(r->getMotorAxisTorque(req.joint)));
      prevTemp[req.joint]=o;
      break;
    case motor::TARGETPOSITION:     
      o=posNorm.unnormPos(r->getMotorTarget(req.joint),req.joint);
      break;
    case motor::POTENTIOPOSITION:
      //std::cout<<"unnormalizing"<<r->getPosition(req.joint)<<std::endl;     
      o=posNorm.unnormPos(r->getPosition(req.joint),req.joint);
      //std::cout<<"unnormalized to"<<o<<std::endl;    
      break;
    case motor::POWER:
      o=r->getPowerState(req.joint);
      break;      
    default:
      throw exc::InvalidOperation("ODE simulation does not support property "+toString(req.id));
    }

  }
  

};




#endif
