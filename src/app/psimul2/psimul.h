#ifndef _PSIMUL_
#define _PSIMUL_

#include <helium/wrap/ode/odeStructure.h>
#include <helium/simul/simulMotorAdapter.h>
#include <helium/system/cellServer.h>

#include <helium/robot/motorSpooler.h>
#include <helium/robot/application.h>
#include <helium/robot/motorFramePlayMode.h>
#include <helium/robot/sensorSpooler.h>
#include <helium/simul/mouseSimulatedTouchSensor.h>
#include <helium/simul/simulTouchAdapter.h>
#include <helium/simul/simulTouchAdapter.h>
#include <helium/simul/simulRototraAdapter.h>

#include <helium/robot/timeSource.h>
#include <helium/robot/motorOffMode.h>
#include <helium/robot/motorNotifiedLinearMode.h>


template<typename MotorSign> struct MotorModes{
  std::auto_ptr<helium::MotorOffMode<MotorSign> > off;
  std::auto_ptr<helium::SecondRefTimeSource > tsrc;
  std::auto_ptr<helium::MotorLinearMode<MotorSign,helium::motor::IFACELEVEL> > mlm;
  std::auto_ptr<helium::MotorFramePlayMode<MotorSign> > mfp;
  std::auto_ptr<helium::MotorNotifiedLinearMode<MotorSign,helium::motor::RAWLEVEL> > rmlm;
};





class PSimul:public helium::Application{
  enum Flags{SHOWWINDOW=1<<0,
	     SHOWCAMERA=1<<2,	
	     USESIMULMOTORS=1<<3,
	     USEMOUSESENSORS=1<<4,
	     USETOUCHSENSOR=1<<5,	     
	     USEROTOTRASPOOLING=1<<6,
	     STARTSERVER=1<<7,	     	 	     
	     STARTSIMUL=1<<8

  };

  enum MotorModeType{
    OFFMOTORMODE=1<<0,
    LINEARMOTORMODE=1<<1,
    FRAMEPLAYMOTORMODE=1<<2,
    RAWLINEARMOTORMODE=1<<3
  };
  
 
  void assureDependency(int required,int when);
  void fixFlags();


  

protected:  
  int options;
  int motorModes;
  MotorModeType defaultMode;
  int serverPort;
  double dt;
  int port;
  int touchSampleDecimation;
  int gyroSampleDecimation;

  struct Components{
    helium::Array<double,helium::array::RESIZABLE,helium::array::LINK> mot;//motors
    helium::ODEWorld* w;  
    helium::MotorSpooler<helium::SimulMotorSign>* ms;
    helium::motor::Raw<helium::SimulMotorSign>::Hardware* miface;
    MotorModes<helium::SimulMotorSign>& mmodes;

    helium::MouseSimulatedTouchSensor* msts;

    helium::SimulTouchAdapter* touchHw;
    helium::SensorSpooler<double>* touchSs;
    helium::SensorProcessor<double>* touchSp;

    helium::SimulRototraAdapter* rototraHw;
    helium::SensorSpooler<double>* rototraSs;
    helium::SensorProcessor<double>* rototraSp;

    Components(double* motdata,size_t motsize,helium::ODEWorld* w,
	       helium::MotorSpooler<helium::SimulMotorSign>* ms,
	       helium::motor::Raw<helium::SimulMotorSign>::Hardware* miface,
	       MotorModes<helium::SimulMotorSign>& mmodes,
	       helium::MouseSimulatedTouchSensor* msts,
	       helium::SimulTouchAdapter* touchHw,
	       helium::SensorSpooler<double>* touchSs,
	       helium::SensorProcessor<double>* touchSp,
	       helium::SimulRototraAdapter* rototraHw,
	       helium::SensorSpooler<double>* rototraSs,
	       helium::SensorProcessor<double>* rototraSp
	       );
  };

  

  template<typename MotorSign> void addSimulatedMotSpooling( std::auto_ptr<helium::SimulMotorAdapter<helium::ODERobot,MotorSign> >& miface,
								     std::auto_ptr<helium::MotorSpooler<MotorSign> >& ms,
							     MotorModes<MotorSign>& mmodes,
							     helium::ODEWorld* w,
							     helium::ODERobot* r,
							     double* motData,
							     int n,
							     typename helium::SimulPropNormalizer<helium::ODERobot,MotorSign>::ParamPtr prop=NULL,
							     typename helium::SimulPosNormalizer<helium::ODERobot,MotorSign>::ParamPtr pos=NULL
							     );
								     
  
  virtual void run(const Components&);//default is GlDraw::join

  void set(int);
  bool isSet(int);
  bool  isMotorModeSet(int);


  void run();
public:


  PSimul(int options=SHOWWINDOW|USEMOUSESENSORS|USETOUCHSENSOR|USEROTOTRASPOOLING|STARTSERVER|STARTSIMUL,
	 int motorModes=OFFMOTORMODE|LINEARMOTORMODE|FRAMEPLAYMOTORMODE|RAWLINEARMOTORMODE,
	 MotorModeType defMode=LINEARMOTORMODE,
	 double dt=3.5E-3,
	 int port=helium::CELLPORT,
	 int touchSampleDecimation=37,
	 int gyroSampleDecimation=37 
	 );
  virtual ~PSimul();

};

#include "psimul.hpp"


#endif
