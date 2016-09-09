#include "psimul.h"









#include <helium/graphics/glDraw.h>


#include <helium/wrap/ode/simulGraphics.h>
#include <helium/vstone/V4Gstructure.h>
#include <memory>
using std::auto_ptr;
using namespace helium;






void PSimul::set(int x){
  options|=x;
}

bool PSimul::isSet(int x){
  return (options&x)!=0;
}

bool PSimul::isMotorModeSet(int x){
  return (motorModes&x)!=0;
}

void PSimul::assureDependency(int required,int when){
  if (!isSet(required)&&isSet(when)){
    set(required);
  }
}

void  PSimul::fixFlags(){
  assureDependency(SHOWWINDOW,USEMOUSESENSORS);
}



PSimul::Components::Components(
			       double* motData,
			       size_t motSize,
			       helium::ODEWorld* pw,
			       helium::MotorSpooler<helium::SimulMotorSign>* pms,
			       helium::motor::Raw<SimulMotorSign>::Hardware* pmiface,
			       MotorModes<helium::SimulMotorSign>& pmmodes,
			       helium::MouseSimulatedTouchSensor* pmsts,
			       helium::SimulTouchAdapter* ptouchHw,
			       helium::SensorSpooler<double>* ptouchSs,
			       helium::SensorProcessor<double>* ptouchSp,
			       helium::SimulRototraAdapter* prototraHw,
			       helium::SensorSpooler<double>* prototraSs,
			       helium::SensorProcessor<double>* prototraSp
			       ):
  mot(motData,motSize),w(pw),ms(pms),miface(pmiface),mmodes(pmmodes),
  msts(pmsts),touchHw(ptouchHw),touchSs(ptouchSs),touchSp(ptouchSp),
  rototraHw(prototraHw),rototraSs(prototraSs),rototraSp(prototraSp)
{

}


void PSimul::run(){ 
  fixFlags();

  static const int n=22;   
  ODEWorld world;    
  Array<double,n> mots;
  zero(mots);
  Array<double,V4Gstructure::BOXESNUM*6> displaySensor;    
  V4Gstructure v4s(mots.data,displaySensor.data);     
  ODERobot* r=world.addRobot(&v4s);    
  
  auto_ptr<SimulWin> window;
  if (isSet(SHOWWINDOW)){
    window.reset(new SimulWin(&world));
    GlDraw::push_back(window.get());    
  }

  auto_ptr<MouseSimulatedTouchSensor> mouseSensor;

  if (isSet(USEMOUSESENSORS)){
    mouseSensor.reset(new MouseSimulatedTouchSensor(displaySensor.data,displaySensor.size(),&v4s));
    window.get()->addKeyboardInteraction(mouseSensor.get());
    window.get()->addMouseSensorListener(mouseSensor.get());
  }
  
  std::auto_ptr<SimulMotorAdapter<ODERobot,SimulMotorSign> > motorIface;
  std::auto_ptr<MotorSpooler<SimulMotorSign> > ms;

  MotorModes<SimulMotorSign> mmodes;
  if (isSet(USESIMULMOTORS)){
    addSimulatedMotSpooling(motorIface,ms,mmodes,&world,r,mots.data,n);
  }


  
  
  
  auto_ptr<SimulTouchAdapter> touch;
  auto_ptr<SensorSpooler<double> > touchSs;
  auto_ptr< SensorProcessor<double> > touchSp;



  if (isSet(USETOUCHSENSOR)){
    touch.reset(new SimulTouchAdapter(v4s.forces,V4Gstructure::BOXESNUM*6,touchSampleDecimation));
    world.addCallback(touch.get());
    touchSs.reset(new SensorSpooler<double>(*touch.get()));
    touchSp.reset(new SensorProcessor<double>(*touchSs.get()));  
  }


  auto_ptr<SimulRototraAdapter> rototra;
  auto_ptr<SensorSpooler<double> > rototraSs;
  auto_ptr< SensorProcessor<double> > rototraSp;
  
  if (isSet(USEROTOTRASPOOLING)){
    rototra.reset(new SimulRototraAdapter(r,gyroSampleDecimation));
    world.addCallback(rototra.get());
    rototraSs.reset(new SensorSpooler<double>(*rototra.get()));
    rototraSp.reset(new SensorProcessor<double>(*rototraSs.get()));  
  }
  


  

  
  
  auto_ptr<CellServer> tcp;
  if (isSet(STARTSERVER)){
    
    tcp.reset(new CellServer(port,mem));
    
  }//if  startserver				       
  else{
    //std::cout<<" no server started"<<std::endl;
  }
   
  
  if (isSet(SHOWWINDOW)||isSet(SHOWCAMERA)){
    GlDraw::start(argc,argv);
  }

 
  if (isSet(STARTSIMUL)){
    world.run(dt);    
  }

  GlDraw::Stopper graphics;
  
  std::cout<<"passing the componenents"<<std::endl;
  run(Components(mots.data,
		 mots.size(),
		 &world,
		 ms.get(),
		 motorIface.get(),
		 mmodes,
		 mouseSensor.get(),
		 touch.get(),
		 touchSs.get(),
		 touchSp.get(),
		 rototra.get(),
		 rototraSs.get(),
		 rototraSp.get()
		 )
      );

  graphics.stop();
  world.stop();
  

 
}


PSimul::PSimul(int poptions,int pmotorModes,MotorModeType pdefaultMode,double pdt,int pport,
	       int ptouchDecimation,int pgyroDecimation):
  options(poptions),
  motorModes(pmotorModes),
  defaultMode(pdefaultMode),
  dt(pdt),
  port(pport),
  touchSampleDecimation(ptouchDecimation),
  gyroSampleDecimation(pgyroDecimation)
{
  }

PSimul::~PSimul(){
  join();
}


void PSimul::run(const Components&){
  GlDraw::join();
}
