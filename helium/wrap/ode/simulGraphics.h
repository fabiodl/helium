#ifndef HE_SIMULGRAPHICS_
#define HE_SIMULGRAPHICS_

#include <helium/graphics/glPrimitiveCanvas.h>
#include <helium/graphics/movingCamera.h>
#include <helium/wrap/ode/odeStructure.h>
#include <helium/wrap/ode/glDrawOde.h>
#include <helium/simul/mouseSimulInteraction.h>

namespace helium{
class CameraWin:public GlPrimitiveCanvas{
  ODEWorldCamera* oc;  
  GlDrawODE *gdo;
  double viewMatrix[16];
  double aspect;
 public:
  void draw(int w,int h);
  CameraWin(ODEWorldCamera* oc,int w,int h,std::string name="Robot view",int x=0,int y=0);
  virtual ~CameraWin();
  void onDraw();
  void onResize(int w,int h);
  void setAspect(double a);
};



class GlDrawExtra{
 public:
  virtual void onDraw()=0;
};

class SimulWin:public PrimitiveKeyboardInteraction,public PrimitiveMouseInteraction,public GlPrimitiveCanvas{
  enum InteractionMode {WORLDMOVE,ROBOTMOVE,PUSHMODE};
  enum PauseReason{PAUSEKEY,
		   PAUSEMOUSEMOVELEFT,PAUSEMOUSEMOVEMIDDLE,PAUSEMOUSEMOVERIGHT,		   
		   PAUSERESETPOSITION,PAUSEACQUIREPOSITION};
  ODEWorld* w;
  
  GlDrawODE *gdo;
  MovingCamera movingCamera;
  bool movingCameraEnable;
  double viewMatrix[16];
  int pauses;
  int currRobot,lastSelRobot;
  int currSensor;
  InteractionMode mode;      
  GlDrawODE::WireframeMode wireFrame;
  double viewScale,rotateScale;
  double *storedPosition;
  double clickedZ;
  std::vector<MouseSensorListener*> mouseSensorListener;
  void updatePause(PauseReason reason,bool state);

  void getSensor(int& robotID,int& sensorID,int x,int y);
  void selectSensor(int x,int y,bool force=false);
  void translateView(int dx,int dy);
  void rotateView(int x,int y,int dx,int dy,bool zAxis=false);
  void viewZAxis(int x,int y,int dx,int dy);
  void translateRobot(int x,int y,int prevX,int prevY);
  void rotateRobot(int x,int y,int dx,int dy,bool zAxis=false);
  void robotZAxis(int x,int y,int prevX,int prevY);
  void resetRobot(bool rotation);
 public: 
  GlDrawExtra *glDrawExtra;
  SimulWin(ODEWorld* w);
  virtual ~SimulWin();
  PrimitiveKeyboardInteraction::KeyParsingState onKeyPushed(unsigned char key,int x,int y);
  void onMouseClicked(MouseButton button,bool down,int x,int y);
  void onMouseMoved(int x,int y,int prevX,int prevY,MouseButton  pushButton,int pushX,int pushY);
  void onResize(int w,int h);
  void onDraw();
  void addMouseSensorListener(MouseSensorListener* msl){
    mouseSensorListener.push_back(msl);
  }
  const double* getView(){
    return viewMatrix;
  }


  void enableMovingCamera(bool s){
    movingCameraEnable=s;
  }
  void setRelativeCamera(int sview);
  void setView(const double* view){
    memcpy(viewMatrix,view,16*sizeof(double));
    movingCamera.setCamera(view);
  }

  void getView(double* view){
    memcpy(view,viewMatrix,16*sizeof(double));
  }

  MovingCamera& getMovingCamera(){
    return movingCamera;
  }



};
}//helium

#endif
