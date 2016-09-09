/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags s
 */


#include <helium/device/sidewinder2.h>
#include <helium/vector/operations.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <iostream>
#include <helium/core/exception.h>
#include <helium/os/inputEvent.h>
#include <cmath>

#define SIDEWINDER_VID 0x45e
#define SIDEWINDER_PID 0x1b
#define SIDEWINDER_NAME "Microsoft SideWinder Force Feedb"
#define AXIS_MAX_XY 512
#define AXIS_MAX_RZ 32
#define AXIS_MAX_THROTTLE 127
#define AXIS_MAX_HAT_XY 1

#define BITS_PER_LONG (sizeof(long) * 8)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)    ((array[LONG(bit)] >> OFF(bit)) & 1)





namespace helium{


  void SideWinder2::makeSpring(struct ff_effect& eff,double forceCoeff,double forceSat,double x,double y,bool newID){
  eff.type = FF_SPRING;
  if (newID) eff.id = -1;
  eff.direction = 0x8000;
  eff.u.condition[0].right_saturation = round(forceSat*0x7fff);
  eff.u.condition[0].left_saturation = round(forceSat*0x7fff);
  eff.u.condition[0].right_coeff = round(forceCoeff*0x7fff);
  eff.u.condition[0].left_coeff = round(forceCoeff*0x7fff);
  eff.u.condition[0].deadband = 0x0;
  eff.u.condition[0].center = round(-x*0x7fff);
  eff.u.condition[1] = eff.u.condition[0];
  eff.u.condition[1].center = round(-y*0x7fff);
  eff.trigger.button = 0;
  eff.trigger.interval = 0;
  eff.replay.length = -1;  
  eff.replay.delay = 0;  
  //std::cout<<(int)eff.u.condition[0].center<<" # "<<(int)eff.u.condition[1].center<<std::endl;
}


  void SideWinder2::makeForce(struct ff_effect& effect,double nx, double ny,bool newID){

    effect.type = FF_CONSTANT;
    if (newID) effect.id = -1;

    double angle= atan2(nx, -ny);
    double mag=sqrt(nx*nx+ny*ny);
    effect.u.constant.level = 0x7fff * mag;
    effect.direction = 0x8000 * (angle + M_PI)/M_PI;
    effect.u.constant.envelope.attack_length = 0;
    effect.u.constant.envelope.attack_level = 0;
    effect.u.constant.envelope.fade_length = 0;
    effect.u.constant.envelope.fade_level = 0;
    effect.trigger.button = 0;
    effect.trigger.interval = 0;
    effect.replay.length = 0xffff;
    effect.replay.delay = 0;
}


  void SideWinder2::makeDamper(struct ff_effect& effect,double force,bool newID){
    effect.type = FF_DAMPER;
    if (newID) effect.id = -1;
    effect.u.condition[0].right_saturation = 0x7fff;
    effect.u.condition[0].left_saturation = 0x7fff;
    effect.u.condition[0].right_coeff = force*0x7fff;
    effect.u.condition[0].left_coeff = force*0x7fff;
    effect.u.condition[0].deadband = 0x0;
    effect.u.condition[0].center = 0x0;
    effect.u.condition[1] = effect.u.condition[0];
    effect.trigger.button = 0;
    effect.trigger.interval = 0;
    effect.replay.length = 0xffff;  // 20 seconds 
    effect.replay.delay = 0;
  }


SideWinder2::SideWinder2():fd(-1){
  std::string devname=helium::getEventFilename(SIDEWINDER_VID,SIDEWINDER_PID,SIDEWINDER_NAME);
  std::cout<<"Found at file"<<devname<<std::endl;
  
  fd = open(devname.c_str(), O_RDWR);
  if (fd==-1) throw helium::exc::FileOpenRead(devname);
  
}

SideWinder2::~SideWinder2(){
  if (fd!=-1) close(fd);
}


void SideWinder2::printInfo(){
  unsigned long features[4];
 /* Query device */
  if (ioctl(fd, EVIOCGBIT(EV_FF, sizeof(unsigned long) * 4), features) == -1) {
    throw helium::exc::InvalidOperation("Ioctl query");
  }
  printf("Axes query: ");  
  if (test_bit(ABS_X, features)) printf("Axis X ");
  if (test_bit(ABS_Y, features)) printf("Axis Y ");
  if (test_bit(ABS_WHEEL, features)) printf("Wheel ");

  printf("\nEffects: ");

  if (test_bit(FF_CONSTANT, features)) printf("Constant ");
  if (test_bit(FF_PERIODIC, features)) printf("Periodic ");
  if (test_bit(FF_SPRING, features)) printf("Spring ");
  if (test_bit(FF_FRICTION, features)) printf("Friction ");
  if (test_bit(FF_RUMBLE, features)) printf("Rumble ");
  
  printf("\nNumber of simultaneous effects: ");
  int n_effects;	/* Number of effects the device can play at the same time */
  if (ioctl(fd, EVIOCGEFFECTS, &n_effects) == -1) {
    throw helium::exc::InvalidOperation("Ioctl number of effects");
  }
  
  printf("%d\n", n_effects);

}


void SideWinder2::uploadEffect(struct ff_effect& eff){
  //std::cout<<"Uploading "<<eff.id<<std::endl;
  if (ioctl(fd, EVIOCSFF, &eff) <0) {
    throw helium::exc::InvalidOperation("Effect upload");
  }
  
}

  void SideWinder2::removeEffect(struct ff_effect& eff){
    //std::cout<<"Removing "<<eff.id<<std::endl;
    if (ioctl(fd, EVIOCRMFF, &eff) <0) {
      throw helium::exc::InvalidOperation("Effect removal");
    }
    
  }

void SideWinder2::setAutocenter(double force){
  struct input_event ie;
  ie.type = EV_FF;
  ie.code=FF_AUTOCENTER;
  ie.value=0xFFFFUL*force;
  if (write(fd,&ie,sizeof(ie))==-1){
    helium::exc::InvalidOperation("Set auto-center");
  }
}


void SideWinder2::setGain(double force){
  struct input_event ie;
  ie.type = EV_FF;
  ie.code=FF_GAIN;
  ie.value=0xFFFFUL*force;
  if (write(fd,&ie,sizeof(ie))==-1){
    helium::exc::InvalidOperation("Set auto-center");
  }
}

void SideWinder2::activateEffect(struct ff_effect& eff){
  struct input_event ie;
  ie.type = EV_FF;
  ie.code = eff.id;
  //std::cout<<"activating"<<eff.id<<std::endl;
  ie.value=1;
  if (write(fd, (const void*) &ie, sizeof(ie)) == -1) {
    helium::exc::InvalidOperation("Activate effect");
  }
}


void SideWinder2::disactivateEffect(struct ff_effect& eff){
  struct input_event ie;
  ie.type = EV_FF;
  ie.code = eff.id;
  //std::cout<<"disactivating"<<eff.id<<std::endl;
  ie.value=0;
  if (write(fd, (const void*) &ie, sizeof(ie)) == -1) {
    helium::exc::InvalidOperation("Disactivate effect");
  }
}



  int SideWinder2::getEvents(struct input_event* ev,int maxSize){
    int rd=read(fd, ev, sizeof(struct input_event) * maxSize);
    if (rd < (int) sizeof(struct input_event)) {
      throw helium::exc::InvalidOperation("Read events");
    }
    return rd/ sizeof(struct input_event);
  }
  




void SideWinder2::readPos(double& x,double& y){
  struct input_event ev[64];
  int rd = getEvents(ev,64);
  for (int i = 0; i < rd ; i++){
    switch (ev[i].type){
    case EV_ABS:
      switch(ev[i].code){
      case 0:
	x=double((int)ev[i].value)/AXIS_MAX_XY;
	break;
      case 1:
	y=double((int)ev[i].value)/AXIS_MAX_XY;
	break;
      }
    }
    
  }

		  
  /*
		  
			printf("Event: time %ld.%06ld, type %d (%s), code %d (%s), value %d\n",
				ev[i].time.tv_sec, ev[i].time.tv_usec, ev[i].type,
				events[ev[i].type] ? events[ev[i].type] : "?",
				ev[i].code,
				names[ev[i].type] ? (names[ev[i].type][ev[i].code] ? names[ev[i].type][ev[i].code] : "?") : "?",
				ev[i].value);

  */
  
}


  SideWinder2::EventReader::State::State(){
    zeron(button,NUMBUTTONS);
    zeron(axis,NUMAXES);
  }



  SideWinder2::EventReader::EventReader(SideWinder2& _sw,bool autostart):
    sw(_sw)
  {    
    if (autostart){
      start();
    }

  }


  SideWinder2::EventReader::~EventReader(){
    running=false;
    join();
  }



  void SideWinder2::EventReader::parseKey(int button,int key){
    state.button[button]=key;
    onButtonStateChange[button](&state);    
  }


  void SideWinder2::EventReader::parseAxis(int axisId,double value){
    state.axis[axisId]=value;
    onAxisStateChange[axisId](&state);    
  }


  void SideWinder2::EventReader::run(){
    running=true;
    struct input_event ev[64];
    while(running){
      int rd = sw.getEvents(ev,64);
      for (int i = 0; i < rd ; i++){
	switch(ev[i].type){	    
	case  EV_KEY:
	  if (ev[i].code>=BTN_JOYSTICK&&ev[i].code<=BTN_BASE2){
	    parseKey(ev[i].code-BTN_JOYSTICK,ev[i].value);
	  }else if(ev[i].code==BTN_DEAD){
	    parseKey(State::BTNDEAD,ev[i].value);
	  }
	  break;	 
	case EV_ABS:
	  switch(ev[i].code){
	  case ABS_X:
	  case ABS_Y:
	    parseAxis(ev[i].code-ABS_X,double((int)ev[i].value)/AXIS_MAX_XY);
	    break;
	  case ABS_RZ:
	    parseAxis(State::RZ,double((int)ev[i].value)/AXIS_MAX_RZ);
	    break;
	  case ABS_THROTTLE:
	    parseAxis(State::THROTTLE,double((int)ev[i].value)/AXIS_MAX_THROTTLE);
	    break;
	  case  ABS_HAT0X:
	  case  ABS_HAT0Y:
	    parseAxis(ev[i].code-ABS_HAT0X+State::HATX,double((int)ev[i].value)/AXIS_MAX_HAT_XY);	    
	  }
	  break;
	  
	}//switch
	
      }//for i
    }//while running
  }//run

  const char* EnumStrings<SideWinder2::EventReader::State::Axis>::strings[EnumStrings<SideWinder2::EventReader::State::Axis>::NUM]={  "X","Y","RZ","THROTTLE","HATX","HATY"};

  const char* EnumStrings<SideWinder2::EventReader::State::Button>::strings[EnumStrings<SideWinder2::EventReader::State::Button>::NUM]={  "TRIGGER","2","3","4","5","6","7","8","DEAD"};
}
