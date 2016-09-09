

#include <helium/robot/motorExceptionHandler.h>
#include <helium/robot/sensorExceptionHandler.h>
#include <helium/robot/sensorStructures.h>
#include <helium/vstone/neonySensorMapping.h>
#include <helium/vstone/V4Gstructure.h>
#include <helium/vstone/vServoSign.h>
#include <helium/graphics/glDraw.h>
#include <helium/system/keyboardCellServer.h>
#include <helium/wrap/ode/odeSimulMotorAdapter.h>
#include <helium/robot/motorPropertyManager.h>
#include <helium/spooling/spoolerControl.h>
#include <helium/spooling/interruptibleWaitingJob.h>
#include <helium/robot/gyroStructures.h>
#include <helium/thread/exitHandler.h>

#include "psimul.h"


using namespace helium;

class CombineFilterComp:public sensor::GenericFilterComputation<double,double>{
  MouseSimulatedTouchSensor& msts;
  
  double saturate(double x){
    return 1-exp(-2*x);
  }

  void compute(double* r,const double *s,const HwIdList& what){    
    for (size_t i=0;i<what.ids.size();i++){
      int idx=what.ids[i];
      r[idx]=saturate(s[idx]);
    }
    msts.addTo(r,what.ids);
  }

  void  getRequired(HwIdList& r,const HwIdList& s){
    r=s;
  }
  void getGenerated(HwIdList& gen,const HwIdList& id){
    gen=id;
  }

public:
  CombineFilterComp(MouseSimulatedTouchSensor& pmsts):msts(pmsts){
  }

};








class SensorDenormComp:public sensor::GenericFilterComputation<int,double>{
  sensor::Calib<int>& limit;
  
  void compute(int* r,const double *s,const HwIdList& what){    
    for (size_t i=0;i<what.ids.size();++i){
      int idx=what.ids[i];
      r[idx]=limit[idx].minv+s[idx]*(limit[idx].maxv-limit[idx].minv);
    }
  }

  void getRequired(HwIdList& req,const HwIdList& what){
    req=what;
  }
  
  void getGenerated(HwIdList& gen,const HwIdList& id){
    gen=id;
  }

public:
  SensorDenormComp(sensor::Calib<int>& plimit):limit(plimit){    
  }
  
};


/*class MotorDenormComp:public sensor::GenericFilterComputation<int,double>{
  motor::Calib<VservoSign>& calib;
  motor::LinearPosNormalizer<VservoSign> normal;
  
  void compute(int* r,const double *s,const HwIdList& what){    
    for (size_t i=0;i<what.ids.size();++i){
      int idx=what.ids[i];
      r[idx]=limit[idx].minv+s[idx]*(limit[idx].maxv-limit[idx].minv);
    }
  }

  void getRequired(HwIdList& req,const HwIdList& what){
    req=what;
  }
  
  void getGenerated(HwIdList& gen,const HwIdList& id){
    gen=id;
  }

public:
  SensorDenormComp(motor::Calib<VservoSign>& cal):calib(cal),normal({    
  }
  
  };*/

/*namespace helium{
template<typename RobType>
class SimulPropNormalizer<RobType,SimulMotorSign>:public motor::Raw<SimulMotorSign>::PropNormalizer{	
  typedef void* ParamPtr;
    motor::Interface::Position normPos(SimulMotorSign::Pos value,int joint);
    SimulMotorSign::Pos unnormPos(motor::Interface::Position value,int joint);
    SimulPosNormalizer(void*);
};  

}*/

class GyroDenormComp:public sensor::GenericFilterComputation<int,double>{
  const GyroCalib& g;


  void compute(int* r,const double *m,const HwIdList& what){  
    static const double magnitude=300;        
    /*std::cout<<"converting";
    print(m)<<std::endl<<" with ";
    std::cout<<g.xratio<<" "<<g.xoffset<<std::endl;
    std::cout<<g.yratio<<" "<<g.yoffset<<std::endl;
    std::cout<<" "<<g.zoffset<<std::endl;
    std::cout<<" to ";*/
    r[0]=-round(magnitude*m[8]*g.xratio-g.xoffset);
    r[1]=-round(magnitude*m[9]*g.yratio-g.yoffset);
    r[2]=-round(magnitude*m[10]*1-g.zoffset);
    r[3]=r[4]=0;  
    //print(r,5)<<std::endl;
   }

   void getRequired(HwIdList& req,const HwIdList& what){
     if (what.ids.size()){
       req.ids.resize(16);
       setConsecutive(req.ids);
     }else{
       req.ids.clear();
     }
   }

  void getGenerated(HwIdList& gen,const HwIdList& id){
    gen.ids.resize(5);
    setConsecutive(gen.ids);
  }

public:
  GyroDenormComp(const GyroCalib& pg):g(pg){
  }

};





class TSDescr:public Description<sensor::Calib<int> >{
public:  
  TSDescr(sensor::Calib<int>& sc):Description<sensor::Calib<int> >(sc){
  }

  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(vectMapping(vd(obj),IDPath("robot.sensors.sensor","."),"",IDPath("robot.sensors.num",".")  ));
  };

};


class TSRawsDescr:public Description<sensor::SingleCellAdapter<int,double> >{
public:
  TSRawsDescr(sensor::SingleCellAdapter<int,double>& a):Description<sensor::SingleCellAdapter<int,double> >(a){}

  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(vectMapping(vd(obj.cells),IDPath("robot.sensors.sensor","."),"value",IDPath("robot.sensors.num",".")));
  }
  

};

class PHWSimul:public PSimul{

  template<typename T>
  class IdleJob:public Job<T >{
    bool operator()(T&){
      milliSleep(500);
      return false;
    }

    bool isToCancel(){
      return false;
    }

  };


  template<typename T>
  class PrintIdleJob:public Job<T >{
    bool operator()(T&){
      //std::cout<<"The idle job is called"<<std::endl;
      milliSleep(500);
      return false;
    }

    bool isToCancel(){
      return false;
    }

  };

  void finalize(const Components& comp, std::auto_ptr<helium::MotorSpooler<helium::VServoSign> > &ms){
    ms->stop();
    comp.touchSs->stop();

    ms->join();
    comp.touchSs->join();
  }
  
  class SingleGyroDescription:public helium::Description<helium::sensor::SingleCellAdapter<int,double> >{
  public:
    SingleGyroDescription(helium::sensor::SingleCellAdapter<int,double>& s):helium::Description<helium::sensor::SingleCellAdapter<int,double> >(s){
    }    

    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(vectMapping(this->obj.cells,"sensor","value","num"));
    }
    
    
  };


  void run(const Components& comp){
   
      
      std::auto_ptr<helium::SimulMotorAdapter<ODERobot,VServoSign> > miface;
      std::auto_ptr<helium::MotorSpooler<helium::VServoSign> > ms;
      MotorModes<helium::VServoSign> mmodes;
      
      motor::Calib<VServoSign> calib;






      try{
	dimport(calib,mem,"robot");
      addSimulatedMotSpooling(miface,ms,mmodes,comp.w,comp.w->robots[0],comp.mot.data,comp.mot.size(),NULL,&calib);

      for (size_t i=0;i<22;i++){
	std::cout<<std::setprecision(10)<<(calib.data[i].maxv-calib.data[i].zero) *VServoSign::TONORM<<std::endl;
      }


      //SpoolerControl<motor::MotorAccess<helium::VServoSign> > msControl(*ms.get());
      //dexport(mem,msControl,IDPath("robot.motors.spooler.minpriority","."));
    IdleJob<motor::MotorAccess<helium::VServoSign> > idleJob;
    ms->addJob(&idleJob,MotSpoolPriority::IDLE);
    InterruptibleWaitingJob<motor::MotorAccess<helium::VServoSign> > normalIdle(*ms,MotSpoolPriority::NORMAL,200);
    InterruptibleWaitingJob<motor::MotorAccess<helium::VServoSign> > highIdle(*ms,MotSpoolPriority::HIGH,200);


    //SpoolerControl<sensor::Set<double> > tsControl(*comp.touchSs);
    //dexport(mem,tsControl,IDPath("robot.sensors.spooler.minpriority","."));

    PrintIdleJob<sensor::Set<double> > sensIdleJob;
    comp.touchSs->addJob(&sensIdleJob,SensSpoolPriority::NORMAL);
    
    InterruptibleWaitingJob<sensor::Set<double> > sensNormalIdle(*comp.touchSs,SensSpoolPriority::NORMAL,200);
    InterruptibleWaitingJob<sensor::Set<double> > sensHighIdle(*comp.touchSs,SensSpoolPriority::HIGH,300);

    MotorExceptionHandler<helium::VServoSign> meh(*ms,*miface);
    dexport(mem,meh,"robot");
    MotorPropertyManager<helium::VServoSign> motPropMan(*ms);
    dexport(mem,motPropMan,"robot");
    CombineFilterComp combfc(*comp.msts);
    NeonySensorMapping sortfc;
    sensor::Calib<int> limit;
    cimport<TSDescr>(limit,mem);
    SensorDenormComp denormfc(limit);
        
    sensor::GenericFilter<double,double>  combineFilter(combfc,Ssensor::SIMULATEDSENSORNUM, *comp.touchSp,Rsensor::REALSENSORNUM);
    sensor::GenericFilter<double,double> sortFilter(sortfc,Rsensor::REALSENSORNUM,combineFilter,Ssensor::SIMULATEDSENSORNUM);    
    sensor::GenericFilter<int,double> denormFilter(denormfc,Rsensor::REALSENSORNUM,sortFilter,Rsensor::REALSENSORNUM);    


    sensor::SingleGenFilterHardware<double,double> sortHw(*comp.touchHw,sortfc);
    SensorExceptionHandler<double> touchErrorHandler(*comp.touchSs,sortHw,Rsensor::REALSENSORNUM);
    dexport(mem,touchErrorHandler,IDPath("robot.sensors","."));
    
    
    sensor::SingleCellAdapter<int,double> touchRaws(denormFilter);
    cexport<TSRawsDescr>(mem,touchRaws);
    
    dexport(mem,denormFilter,IDPath("robot.sensors.group.value","."));


    GyroCalib gyroCalib;
    dimport(gyroCalib,mem,IDPath("robot.gyro","."));
    
    GyroDenormComp gyroDenormfc(gyroCalib);
    
    sensor::GenericFilter<int,double> gyroDenormFilter(gyroDenormfc,5,*comp.rototraSp,16);
    
    sensor::SingleCellAdapter<int,double> singleGyros(gyroDenormFilter);

    //sensor::GenericFilter<double,int>  accellDenorm(combfc,Ssensor::SIMULATEDSENSORNUM, *comp.touchSp,Rsensor::REALSENSORNUM);

   
    dexport(mem,gyroDenormFilter,IDPath("robot.gyro.group.value","."));
    cexport<SingleGyroDescription>(mem,singleGyros,IDPath("robot.gyro","."));

    sensor::SingleGenFilterHardware<int,double> denormHw(*comp.rototraHw,gyroDenormfc);
    SensorExceptionHandler<double> gyroErrorHandler(*comp.rototraSs,denormHw,5);
    dexport(mem,gyroErrorHandler,IDPath("robot.gyro","."));

    KeyboardCellServer keyb(mem);

    OnExitWait thisThread;


    helium::StaticConnReference<void,KeyboardCellServer,void,&KeyboardCellServer::stopReading> stopReq(&keyb);

    DestroyableSignal<void>::SignalReference sref(stopReq.cb);
    ExitHandler::get().exitRequestConnect(sref);

    mout(msg::OK)("Starting keyboard server");
    keyb.execute();
    std::cout<<"cin stream ended"<<std::endl;
    
    finalize(comp,ms);
    
    }catch(exc::Exception&){
      finalize(comp,ms);
      throw;
    }

  }



};




using namespace helium;

int main(int argc,char** argv){
  PHWSimul simul;
  simul.launch(argc,argv,"conf/psimul2.xml");
}
