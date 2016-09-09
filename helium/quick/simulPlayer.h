#ifndef HE_SIMULPLAYER
#define HE_SIMULPLAYER

#include <helium/wrap/ode/odeStructure.h>
#include <helium/simul/simulMotorAdapter.h>
#include <helium/simul/simulTouchAdapter.h>
#include <helium/simul/simulRototraAdapter.h>
#include <helium/simul/mouseSimulatedTouchSensor.h>

#include <helium/spooling/interruptibleWaitingJob.h>

#include <helium/robot/motorSpooler.h>
#include <helium/robot/motorStructures.h>
#include <helium/robot/motorExceptionHandler.h>
#include <helium/robot/motorPropertyManager.h>

#include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorStructures.h>
#include <helium/robot/genericFilter.h>
#include <helium/robot/sensorExceptionHandler.h>
#include <helium/robot/sensorListMaxComputation.h>

#include <helium/robot/accelerometerDenormComputation.h>

#include <helium/robot/gyroStructures.h>

#include <helium/quick/motorModeSet.h>
#include <helium/quick/simulSensorComputation.h>
#include <helium/wrap/ode/odeSimulMotorAdapter.h>
#include <helium/wrap/ode/simulGraphics.h>
namespace helium{

  

  template<typename MotHwSign >
  struct SimulPlayerMotors{
    struct Options{
      typename SimulPropNormalizer<ODERobot,MotHwSign>::ParamPtr prop;
      motorModeSet::ModeSetId initMode;
      Options();
    };
    
    typename motor::Raw<MotHwSign>::Hardware& motorIface;
    MotorSpooler<MotHwSign> ms;
    SecondRefTimeSource timesrc;
    MotorModeSet<MotHwSign> modes;
    MotorExceptionHandler<MotHwSign> meh;
    MotorPropertyManager<MotHwSign> motPropMan;
    InterruptibleWaitingJob<motor::MotorAccess<MotHwSign> > motNormalIdle,motHighIdle;   

    SimulPlayerMotors(ODERobot* r,	      
		      typename motor::Raw<MotHwSign>::Hardware& motors,
		      const Options& opts=Options());

 
  };





  template<typename MotHwSign >
  struct SimulPlayer:public SimulPlayerMotors<MotHwSign>{
    
    struct Options{
      typename SimulPlayerMotors<MotHwSign>::Options motoropts;

      int touchEveryN,rototraEveryN;
      Options();
    };


    MouseSimulatedTouchSensor mouseSimulSensor;
    SimulTouchAdapter touchIface;
    SensorSpooler<double> touchSs;
    SensorProcessor<double> touchSp;



    CombineFilterComp combfc;
    SensorDenormComp denormfc;    
    sensor::GenericFilter<double,double> touchCombineFilter,touchSortFilter;
    sensor::GenericFilter<int,double> touchDenormFilter;
    sensor::SingleGenFilterHardware<double,double> touchFilteredHw;
    
    SensorExceptionHandler<double> touchErrorHandler;
    sensor::SingleCellAdapter<int,double> touchSingleRaws;
    InterruptibleWaitingJob<sensor::Set<double> > touchNormalIdle,touchHighIdle;
    
    SimulRototraAdapter rototraIface;
    SensorSpooler<double> rototraSs;
    SensorProcessor<double> rototraSp;
    AccelerometerDenormComp gyrodenormfc;
    sensor::GenericFilter<int,double> gyroDenormFilter;
    sensor::SingleCellAdapter<int,double> singleGyros;
    
    SimulPlayer(ODERobot* r,		
		typename motor::Raw<MotHwSign>::Hardware& motors,
		SimulWin& win,
		const Array<double,array::RESIZABLE,array::LINK>& mouseSensor,	      		
		const Array<double,array::RESIZABLE,array::LINK>& forceScalar,		
		sensor::Calib<int>& touchCalib,
		SensorListMaxComputation& touchMapper,
		GyroCalib& gyroCalib,
		const Options& opt=Options());   
};



  template<typename HwSign> 
  class DefDescr<SimulPlayerMotors<HwSign> >: public Description<SimulPlayerMotors<HwSign> >{
  public:
    DefDescr(SimulPlayerMotors<HwSign> & obj):Description<SimulPlayerMotors<HwSign> >(obj){}

    void  getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);    
  };
  



  template<typename HwSign> 
  class DefDescr<SimulPlayer<HwSign> >: public Description<SimulPlayer<HwSign> >{
  public:
    DefDescr(SimulPlayer<HwSign> & obj):Description<SimulPlayer<HwSign> >(obj){}

    void  getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);    
  };
  

}
#include "simulPlayer.hpp"
#endif
