/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

namespace helium{



  template<typename MotorSign>
  class PowerRetriever:public helium::exc::ExceptionHandler{
  public:
    enum State{OFF,ON,UNRETRIEVABLE}; 
    helium::Array<State> powerCondition;
    void handle(helium::exc::Exception& e);
    void retrieve(helium::motor::MotorAccess<MotorSign>& m); //used as job for the spooler
    PowerRetriever(MotorSpooler<MotorSign>& ms);
  };


  
  template<typename MotorSign>
  void PowerRetriever<MotorSign>::handle(exc::Exception& e){
    try{
      e.rethrow();      
    }
    catch(exc::HardwareDeviceException<NAMEDDEVICE>& s){
    powerCondition[s.id]=UNRETRIEVABLE;
    }
    catch(exc::HardwareDeviceException<UNNAMEDDEVICE>& ){
      setAll(powerCondition,UNRETRIEVABLE);	
    }
    catch(exc::MultipleException& me){
    me.handleUsing(this);
    }  
  }


template<typename MotorSign>
void PowerRetriever<MotorSign>::retrieve(motor::MotorAccess<MotorSign>& m){
  Array<bool> powerState(powerCondition.size());
  setAll(powerState,false);
  try{
    m.hw->getPowerState(powerState.data);
    for (size_t j=0;j<powerCondition.size();j++){
      powerCondition[j]=powerState[j]?ON:OFF;
    }
  }
  catch(exc::MultipleException& me){
    me.handleUsing(this);
  }
  catch(exc::Exception& e){
    handle(e);
  }
}



template<typename MotorSign>
PowerRetriever<MotorSign>::PowerRetriever(MotorSpooler<MotorSign>& ms):
  powerCondition(ms.getMotorNum()){   
}










template<typename MotorDevice>
void SpooledMotorSet<MotorDevice>::setCurrentState(){
  PowerRetriever<MotorSign> pr(ms);
  StackJob<motor::MotorAccess<MotorSign>,
	   PowerRetriever<MotorSign>,
	   void,
	   void,
	   &PowerRetriever<MotorSign>::retrieve,true> job(&pr);    
  ms.waitJob(&job, MotSpoolPriority::HIGH);
      for (size_t j=0;j<ms.getMotorNum();j++){
	switch (pr.powerCondition[j]){
	case PowerRetriever<MotorSign>::OFF:
	  ms.setMotorMode(motModeSet.off.getMotorModeId(),j);
	  break;
	case PowerRetriever<MotorSign>::ON:
	  ms.setMotorMode(motModeSet.mlm.getMotorModeId(),j);
	  break;
	case PowerRetriever<MotorSign>::UNRETRIEVABLE:
	  std::cerr<<"The powerstate of "<<j<<" cannot be retrieved"<<std::endl;
	  std::cout<<"Select the action"<<std::endl;
	  std::cout<<" 1) off"<<std::endl;
	  std::cout<<" 2) on"<<std::endl;
	  std::cout<<" 3) no action"<<std::endl;	  
	  {
	    int x=3;
	    //std::cin>>x;
	    switch(x){
	    case 1:
	      ms.setMotorMode(motModeSet.off.getMotorModeId(),j);
	      break;
	    case 2:
	      ms.setMotorMode(motModeSet.mlm.getMotorModeId(),j);
	      break;
	    default:
	      mout(msg::PLAIN)("No action selected");
	      break;
	    }
	  }	  
	  break;
	}//switch
	
      }//for
}



template<typename MotorDevice>
SpooledMotorSet<MotorDevice>::SpooledMotorSet(Memory& mem,const std::string& name,const IDPath& confPath,const IDPath& servicePath):
  mot(mem,confPath,servicePath),
  ms(mot.motors.getMotorNum(),true),
  motModeSet(ms,rts),
  meh(ms,mot.motors),
  mpm(ms)
{
  ms.interfaces.registerAll(&mot.motors);    
  ms.setInterface(mot.motors.getIfaceId());    
  ms.setExceptionHandler(&meh);
  dexport(mem,ms,servicePath);
  dexport(mem,motModeSet,servicePath);
  dexport(mem,meh,servicePath);
  dexport(mem,mpm,servicePath);
  setCurrentState();

}

template<typename MotorDevice>
bool SpooledMotorSet<MotorDevice>::isTemporary(){
  return false;
}



}
