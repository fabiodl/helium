#ifndef HE_MOUSETOUCH
#define HE_MOUSETOUCH

#include <helium/thread/timer.h>
#include <helium/simul/mouseSimulInteraction.h>

namespace helium{


  
  class MouseSimulatedTouchSensor:public PeriodicTimer,public MouseSensorListener,public PrimitiveKeyboardInteraction{
    Mutex access;

    Array<double> sensors;
    Array<double> touchPattern;
    //public:
    //Array<double> displaySensor;
    double* displaySensors;  //the OR of the 2, plus indication selection
    Robot* thisRobot;

    enum PushingMode{EDITPATTERN,DIRECTAPPLY} pushingMode;
    int clickingSensor;
    bool enabled;
    void timerRun();

    void onClickOccurred(PrimitiveMouseInteraction::MouseButton button, bool down,int selId,const Robot* selRobot);
    void onSensorSelected(int selId,Robot* selRobot,int prevId,Robot* prevRobot);

    PrimitiveKeyboardInteraction::KeyParsingState onKeyPushed(unsigned char key,int x,int y);    
    PrimitiveKeyboardInteraction::KeyParsingState onKeyReleased(unsigned char key,int x,int y);    
  public:    
    MouseSimulatedTouchSensor(double* displaySensor,int n,Robot* targetRobot);

    virtual ~MouseSimulatedTouchSensor();

    void addTo(double*); //adds and saurates
    void addTo(double*,int);//n sensors
    void addTo(double*, const std::vector<int>& toRead);//n sensors
    
  
  };


}


#endif
