#include <helium/simul/mouseSimulatedTouchSensor.h>
using namespace helium;

static const char PUSHKEY=' ';
static const double pushingSpeed=0.1;

void MouseSimulatedTouchSensor::timerRun(){      
      access.lock();
      //std::cout<<"hey"<<getSystemMillis()<<std::endl;
      if (enabled&&clickingSensor>=0){
	switch(pushingMode){      
	case EDITPATTERN:
	  //std::cout<<"was "<<touchPattern[clickingSensor]<<std::endl;
	  touchPattern[clickingSensor]=
	    touchPattern[clickingSensor]*(1-pushingSpeed)+1*pushingSpeed;      
	  //std::cout<<"now "<<touchPattern[clickingSensor]<<std::endl;
	  displaySensors[clickingSensor]=touchPattern[clickingSensor];
	  break;
	case DIRECTAPPLY:
	  sensors[clickingSensor]=
	    sensors[clickingSensor]*(1-pushingSpeed)+1*pushingSpeed;      
	  displaySensors[clickingSensor]=sensors[clickingSensor];
	break;
	}	
      }
      access.unlock();
    }

MouseSimulatedTouchSensor::MouseSimulatedTouchSensor(double* pdisplaySensor,int n,Robot* targetRobot):
  PeriodicTimer(50,false),
  access("msts"),
  sensors(n),touchPattern(n),
  displaySensors(pdisplaySensor),thisRobot(targetRobot),
  pushingMode(MouseSimulatedTouchSensor::EDITPATTERN),
  clickingSensor(-1),enabled(false)
{
  zeron(displaySensors,n);
  zero(touchPattern);
  zero(sensors);
  start();
}

MouseSimulatedTouchSensor::~MouseSimulatedTouchSensor(){
  stop();
}


void MouseSimulatedTouchSensor::onClickOccurred(PrimitiveMouseInteraction::MouseButton button, bool down,int selId,const Robot* selRobot){
  access.lock();
  if (enabled){
    if (down){
      switch(button){
      case PrimitiveMouseInteraction::LEFTBUTTON:
	pushingMode=EDITPATTERN;
	clickingSensor=selRobot==thisRobot?selId:-1;
	break;
      case PrimitiveMouseInteraction::MIDDLEBUTTON:	  
      case PrimitiveMouseInteraction::LEFTANDRIGHTBUTTON:
	pushingMode=DIRECTAPPLY;
	zero(touchPattern);
	zero(sensors);
	zeron(displaySensors,sensors.size());
	clickingSensor=selRobot==thisRobot?selId:-1;
	break;
      case PrimitiveMouseInteraction::RIGHTBUTTON:
	pushingMode=DIRECTAPPLY;
	zero(touchPattern);
	zero(sensors);
	zeron(displaySensors,sensors.size());
	clickingSensor=-1;
	break;	    
      case PrimitiveMouseInteraction::NOBUTTON:
      default:
	break;
      }
    }else{
      switch(button){	    
      case PrimitiveMouseInteraction::LEFTBUTTON:	    
	clickingSensor=-1;
	break;
      case PrimitiveMouseInteraction::RIGHTBUTTON:
      case PrimitiveMouseInteraction::MIDDLEBUTTON:
	sensors[clickingSensor]=0;
	    clickingSensor=-1;
	    break;
      case PrimitiveMouseInteraction::LEFTANDRIGHTBUTTON:
      case PrimitiveMouseInteraction::NOBUTTON:
	clickingSensor=-1;
      default:
	break;
      }
    }	     
  }//enabled
  access.unlock();
}//onClickOccurred

void MouseSimulatedTouchSensor::onSensorSelected(int selId,Robot* selRobot,int prevId,Robot* prevRobot){
  access.lock();
  if (enabled){
    //std::cout<<"selected "<<selId<<std::endl;
    if (prevRobot==thisRobot&&prevId>=0){
      displaySensors[prevId]=touchPattern[prevId];    
    }
    if (selRobot==thisRobot&&selId>=0&&clickingSensor==-1){
      displaySensors[selId]=-1;
    }
  }
  access.unlock();
}



PrimitiveKeyboardInteraction::KeyParsingState MouseSimulatedTouchSensor::onKeyPushed(unsigned char key,int x,int y){
  PrimitiveKeyboardInteraction::KeyParsingState keyParsed=PrimitiveKeyboardInteraction::UNMANAGED;
  access.lock();
  switch(key){
  case PUSHKEY:	
    //std::cout<<"push "<<enabled<<" pm "<<pushingMode<<std::endl;
    if (enabled&&pushingMode==EDITPATTERN){
      //std::cout<<"setting"<<std::endl;
      sensors=touchPattern;
    }
    keyParsed=PrimitiveKeyboardInteraction::MANAGED;
    break;
  case 'e':
    enabled=true;
    keyParsed=PrimitiveKeyboardInteraction::MANAGED;
    break;
  case 'w':
  case 'r':
    enabled=false;
    keyParsed=PrimitiveKeyboardInteraction::MANAGED;
    break;
  }      
  //std::cout<<"enabled is "<<enabled<<std::endl;
  access.unlock();
  return keyParsed;
}

PrimitiveKeyboardInteraction::KeyParsingState MouseSimulatedTouchSensor::onKeyReleased(unsigned char key,int x,int y){    
  //std::cout<<"released"<<std::endl;
  access.lock();
  //std::cout<<"zeroing"<<std::endl;
  zero(sensors);
  access.unlock();
  return PrimitiveKeyboardInteraction::MANAGED;
}

void MouseSimulatedTouchSensor::addTo(double* d){
  access.lock();
  for (size_t i=0;i<sensors.size();i++){
    d[i]+=sensors[i];
    if (d[i]>1){
      d[i]=1;
    }
  }
  access.unlock();
}

void MouseSimulatedTouchSensor::addTo(double* d,int id){
  access.lock();
  d[id]+=sensors[id];
  if (d[id]>1){
    d[id]=1;
  }
  access.unlock();
}

void MouseSimulatedTouchSensor::addTo(double* d, const std::vector<int>& toRead){
  access.lock();
  for (size_t i=0;i<toRead.size();i++){
    int idx=toRead[i];
    d[idx]+=sensors[idx];
    if (sensors[idx]>0){
      std::cout<<"add "<<i<<" "<<sensors[i]<<std::endl;
    }

    if (d[idx]>1){
      d[idx]=1;
    }
  }
  access.unlock();
}
