#include <helium/wrap/ode/simulGraphics.h>
#include <helium/graphics/glUtil.h>
#include <helium/graphics/glHeader.h>

#define ESCAPE 27

using namespace std;

namespace helium{
static const double vdistance=0.6;
static const double views[6][16]={
  {-1.000000, -0.000000, 0.000000, 0.000000, 
   0.000000, -0.010000, 0.999950, -0.00000, 
   -0.000000, 0.999950, 0.010000, -0.9000000*vdistance, 
   0.000000, 0.000000, 0.000000, 1.000000},
  {0, -1, 0, 0,
   0, 0, 1, 0,
   -1, 0 ,0, -1*vdistance,
   0,  0, 0 ,1},
  {1,0,0,0,
   0,0,1,0,
   0,-1,0,-1*vdistance,
   0,0,0,1},
  {0,1,0,0,
   0,0,1,0,
   1,0,0,-1*vdistance,
   0,0,0,1},
  {-1,0,0,0,
   0,-1,0,0,
   0,0,1,-1*vdistance,
   0,0,0,1},
  {-1,0,0,0,
   0,1,0,0,
   0,0,-1,-0.4,
   0,0,0,1}  
};


CameraWin::CameraWin(ODEWorldCamera* poc,int w,int h,std::string name,int x,int y):
  GlPrimitiveCanvas(w,h,name,x,y),
  oc(poc),aspect(0.8)
{
  gdo=new GlDrawODE(poc->world,&viewMatrix[3],&viewMatrix[7],&viewMatrix[11]);
}


CameraWin::~CameraWin(){
  delete gdo;
}


void CameraWin::onDraw(){  
  clearWindow(0.74f,0.9f,1);
  oc->getCameraRototranslation(viewMatrix);
  setPerspective(getWidth(),getHeight(),false,aspect);      
  setCamera(viewMatrix);
  setLight();      
  gdo->drawODEEnvironment();
  for (unsigned int i=0;i<oc->world->robots.size();i++)
    gdo->drawODERobot(oc->world->robots[i],false,GlDrawODE::SOLID);
}


void CameraWin::setAspect(double a){
  aspect=a;
}

void CameraWin::onResize(int w,int h){
  glMatrixMode(GL_PROJECTION);  
  glViewport(0, 0,w,h);
  setPerspective(w,h);
}


void SimulWin::onDraw(){ 
  // cout<<"drawing "<<this<<"with time "<<w->t<<endl;
  clearWindow(0.74f,0.9f,1);
  setPerspective(getWidth(),getHeight());      
  if (movingCameraEnable&&lastSelRobot!=-1){
    double p[3];
    w->robots[lastSelRobot]->getPivotPosition(p);
    //movingCamera.setCamera(viewMatrix);
    movingCamera.getCamera(viewMatrix,p);
  }
  setCamera(viewMatrix);
  setLight();      
  gdo->drawODEEnvironment();
  gdo->printTime(w);
  for (unsigned int i=0;i<w->robots.size();i++)
    gdo->drawODERobot(w->robots[i],false,wireFrame);
  if (glDrawExtra!=NULL) glDrawExtra->onDraw();
}



SimulWin::SimulWin(ODEWorld* pw):GlPrimitiveCanvas(640,480,"ODE Simulation",0,480),
				 w(pw),movingCameraEnable(false),pauses(0),
				 currRobot(-1),lastSelRobot(-1),
				 currSensor(-1),
				 mode(WORLDMOVE),wireFrame( GlDrawODE::SOLID),viewScale(0.001),rotateScale(0.01),storedPosition(NULL),glDrawExtra(NULL){
  if (w->robots.size()==0)
    lastSelRobot=-1;
  else
    lastSelRobot=0;
  double r[16],m[16];
  setAsRotation(r,Axis::X,-M_PI/2+0.01);
  setAsRotation(m,Axis::Z,M_PI);    
  mul(viewMatrix,r,m);
  setTranslation(viewMatrix,Vector3d(0,-0.25,-0.7));
  movingCamera.setCamera(viewMatrix);
  gdo=new GlDrawODE(w,&viewMatrix[3],&viewMatrix[7],&viewMatrix[11]);
  mouseInter.push_back(this);
  keyboardInter.push_back(this);
}


void SimulWin::updatePause(PauseReason reason,bool state){
  if (state)
    pauses|=(1<<reason);
  else
    pauses&=~(1<<reason);
  
  bool shouldPause=(pauses!=0);
  //cout<<"should pause="<<shouldPause<<endl;
  if (w->paused!=shouldPause){
    w->pause(shouldPause);
  }
}


void SimulWin::setRelativeCamera(int sview){ 
  double roto[16];    
  double rotoi[16];
  w->robots[lastSelRobot]->getPivotRototranslation(roto);
  setRotation(roto,Axis::X,0);
  invert(rotoi,roto);  
  mul(viewMatrix,views[sview],rotoi);
  movingCamera.setCamera(viewMatrix);
}


PrimitiveKeyboardInteraction::KeyParsingState SimulWin::onKeyPushed(unsigned char key,int x,int y){
  static bool paused=false;
  if (key>='1'&&key<='6'&&lastSelRobot!=-1){
    setRelativeCamera(key-'1');
    return PrimitiveKeyboardInteraction::MANAGED;
  }
  switch(key){
  case ESCAPE:
    exit(0);
    return PrimitiveKeyboardInteraction::MANAGED;
  case 'r':
    mode=ROBOTMOVE;
    return PrimitiveKeyboardInteraction::MANAGED;
  case 'e':
    mode=PUSHMODE;
    return PrimitiveKeyboardInteraction::MANAGED;
  case 'w':
    mode=WORLDMOVE;
    return PrimitiveKeyboardInteraction::MANAGED;
  case 'p':
    paused=!paused;    
    updatePause(PAUSEKEY,paused);
    return PrimitiveKeyboardInteraction::MANAGED;
  case 'S':
  case 's':        
    updatePause(PAUSERESETPOSITION,true);
    resetRobot(key=='S');
    updatePause(PAUSERESETPOSITION,false);
    return PrimitiveKeyboardInteraction::MANAGED;
  case 't':
    wireFrame=(GlDrawODE::WireframeMode)((wireFrame+1)%3);
    break;
  case '0':
    movingCameraEnable=!movingCameraEnable;
    return PrimitiveKeyboardInteraction::MANAGED;

  case 'f':
  case 'F':        
      w->setGravity(key=='f'?-9.81:0);
      return PrimitiveKeyboardInteraction::MANAGED;   
  case 'g':
    if (lastSelRobot!=-1){
      if (storedPosition==NULL){
	storedPosition=new double[16];      
      }
      updatePause(PAUSEACQUIREPOSITION,true);
      w->robots[lastSelRobot]->getPivotRototranslation(storedPosition);
      updatePause(PAUSEACQUIREPOSITION,false);
      print(storedPosition);
    }//lastSelRobot
    return PrimitiveKeyboardInteraction::MANAGED;
  case 'h':
    if (lastSelRobot!=-1&&storedPosition!=NULL){
      updatePause(PAUSEACQUIREPOSITION,true);
      w->robots[lastSelRobot]->setPivotRototranslation(storedPosition);
      w->robots[lastSelRobot]->stopVelocity();
      updatePause(PAUSEACQUIREPOSITION,false);
    }
    return PrimitiveKeyboardInteraction::MANAGED;
  default:
    return PrimitiveKeyboardInteraction::UNMANAGED;      
  }
  return PrimitiveKeyboardInteraction::UNMANAGED;
}


SimulWin::~SimulWin(){
  delete gdo;
  if (storedPosition!=NULL)
    delete[] storedPosition;

}


void SimulWin::onResize(int w,int h){
  glMatrixMode(GL_PROJECTION);  
  glViewport(0, 0, w, h);
  setPerspective(w,h);
}


void SimulWin::onMouseClicked( PrimitiveMouseInteraction::MouseButton button,bool down,int x,int y){
  selectSensor(x,y);  
  if (down){
    clickedZ=getPointZ(x,getHeight()-y);
  }
  PauseReason reason;
  switch(button){
     case LEFTBUTTON:
       reason=PAUSEMOUSEMOVELEFT;
       break;
  case LEFTANDRIGHTBUTTON:
  case MIDDLEBUTTON:
       reason=PAUSEMOUSEMOVEMIDDLE;
       break;
  case RIGHTBUTTON:
    reason=PAUSEMOUSEMOVERIGHT;
    break;
  case NOBUTTON:
    break;
    
  }
  updatePause(reason,down&&mode==ROBOTMOVE);
  for (unsigned int i=0;i<mouseSensorListener.size();i++){
    mouseSensorListener[i]->onClickOccurred(button,down,currSensor,
					    currRobot==-1?NULL:w->robots[currRobot]->srcRobot);
  }
}


void SimulWin::onMouseMoved(int x,int y,int prevX,int prevY,MouseButton  pushButton,int pushX,int pushY){  
  if (pushButton==PrimitiveMouseInteraction::NOBUTTON) selectSensor(x,y);  
  //std::cout<<"mouse moved"<<x<<" "<<y<<std::endl;
  switch(mode){
  case WORLDMOVE:
    switch(pushButton){
    case LEFTBUTTON:
      translateView(x-prevX,y-prevY);
      break;
    case MIDDLEBUTTON:
    case LEFTANDRIGHTBUTTON:
      rotateView(pushX-getWidth()/2,-pushY+getHeight()/2,x-prevX,prevY-y);
      break;
    case RIGHTBUTTON:
      viewZAxis(pushX-getWidth()/2,-pushY+getHeight()/2,x-prevX,y-prevY);
      break;
    case NOBUTTON:
      break;
    }//switch button
    break;//WORLDMOVE
  case ROBOTMOVE:
    switch(pushButton){
    case LEFTBUTTON:
      translateRobot(x,y,prevX,prevY);
      break;
    case MIDDLEBUTTON:
    case LEFTANDRIGHTBUTTON:
      rotateRobot(x,y,x-prevX,y-prevY);
      break;
    case RIGHTBUTTON:
      robotZAxis(x,y,prevX,prevY);
      break;
    case NOBUTTON:
      break;
    }//switch button
    break;//ROBOTMOVE
  case PUSHMODE:
    break;
  }//switch mode
}//onMouseMoved


void SimulWin::translateView(int dx,int dy){
  viewMatrix[3]+=dx*viewScale;
  viewMatrix[7]+=-dy*viewScale;  
  movingCamera.setCamera(viewMatrix);
}


void SimulWin::rotateView(int x,int y,int dx,int dy,bool zAxis){  
  double inv[16];
  invert(inv,viewMatrix);
  double rot[16];
  Vector3d rc;
  rc.set(-x*viewScale-viewMatrix[3],-y*viewScale+viewMatrix[7],0);
  Vector3d ra,axis(-dy,dx,0);
  if (zAxis) 
    axis.set(0,0,dx);
  mulAxis(&ra,inv,&axis);
  getRotation(rot,rc.x(),rc.y(),rc.z(),ra.x(),ra.y(),ra.z(),sqrt((double)dx*dx+dy*dy)*rotateScale);
  double ris[16];
  mul(ris,viewMatrix,rot);
  memcpy(viewMatrix,ris,sizeof(double)*16);
  movingCamera.setCamera(viewMatrix);
}


void SimulWin::viewZAxis(int x,int y,int dx,int dy){
  viewMatrix[11]+=dy*viewScale;
  if (dx!=0) rotateView(x,y,-dx,0,true);
  movingCamera.setCamera(viewMatrix);
}


void SimulWin::translateRobot(int x,int y,int prevX,int prevY){
  double prevPos[3],currPos[3];
  getSpacePos(prevPos,prevX,getHeight()-prevY,clickedZ);
  getSpacePos(currPos,x,getHeight()-y,clickedZ);
  for (int i=0;i<3;i++)
    currPos[i]-=prevPos[i];
  if (currRobot!=-1){
    //std::cout<<"translate robot"<<currPos[0]<<" "<<currPos[1]<<" "<<currPos[2]<<std::endl;
    w->robots[currRobot]->translateRobot(currPos[0],currPos[1],currPos[2]);
    double z=w->robots[currRobot]->getMinZ();
    if (z<0){  
      w->robots[currRobot]->translateRobot(0,0,-z);
    }
    w->robots[currRobot]->stopVelocity();
  }else{
    //std::cout<<"curr no robot"<<std::endl;
  }  
}


void SimulWin::rotateRobot(int x,int y,int dx,int dy,bool zAxis){
  double inv[16];
  invert(inv,viewMatrix);
  Vector3d ra,axis(dy,dx,0);
  if (zAxis) axis.set(0,0,dx);
  mulAxis(&ra,inv,&axis);
  double rot[16];
  getRotation(rot,0,0,0,ra.x(),ra.y(),ra.z(),sqrt((double)dx*dx+dy*dy)*rotateScale);    
  if (currRobot!=-1){
    w->robots[currRobot]->rotateRobot(rot);
    double z=w->robots[currRobot]->getMinZ();
    if (z<0){    
      w->robots[currRobot]->translateRobot(0,0,-z);
    }
    w->robots[currRobot]->stopVelocity();
  }
}


void SimulWin::robotZAxis(int x,int y,int prevX,int prevY){
  int dx=x-prevX;
  int dy=y-prevY;
  double prevPos[3],currPos[3];
  getSpacePos(prevPos,prevX,getHeight()-prevY,clickedZ);
  getSpacePos(currPos,prevX,getHeight()-prevY,clickedZ-dy*viewScale/10);
  for (int i=0;i<3;i++)
    currPos[i]-=prevPos[i];
  if (currRobot!=-1){
    w->robots[currRobot]->translateRobot(currPos[0],currPos[1],currPos[2]);   
    if (dx!=0) rotateRobot(x,y,dx,dy,true);
    double z=w->robots[currRobot]->getMinZ();
    if (z<0){    
      w->robots[currRobot]->translateRobot(0,0,-z);
    }
    w->robots[currRobot]->stopVelocity();
  }
}


void SimulWin::resetRobot(bool rotation){
  //cout<<"lastsel"<<lastSelRobot<<endl;
  if (lastSelRobot!=-1){
    ODERobot* r=w->robots[lastSelRobot];
    r->resetMotors();
    if (rotation){
      double m[16],m2[16];
      r->getPivotRototranslation(m);
      invert(m2,m);	  
    r->rotateRobot(m2);	  
    }	
    double pos[3];
    r->getPivotPosition(pos);
    for (int i=0;i<3;i++)
      pos[i]=-pos[i];
    r->translateRobot(pos[0],pos[1],pos[2]);    		
    
    double z=r->getMinZ();
    if (z<0){	  
      r->translateRobot(0,0,-z);
    }
    r->stopVelocity();
  }
}



void SimulWin::getSensor(int& robotID,int& sensorID,int x,int y){
  //see http://gpwiki.org/  
  y=getHeight()-y;

  static GLuint buff[640] = {0};
  GLint hits, view[4];
  
  glSelectBuffer(640, buff); //choose the storage buffer  
  glGetIntegerv(GL_VIEWPORT, view);//retrieve viewport data   
  
  /*cout<<"view";
   for (int i=0;i<4;i++) cout<< " "<<view[i];
   cout<<endl;*/
  
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  gluPickMatrix(x, y, 0.1, 0.1*(float)(getHeight())/((float)getWidth()), view);//restrict the draw area to the cursor surrounding
  //gluPerspective(45.0f,(GLfloat)getWidth()/(GLfloat)getHeight(),10.0f,10000.0f);
  //localSetPerspective(getWidth(),getHeight(),true);        
  setPerspective(getWidth(),getHeight(),true);        
  glMatrixMode(GL_MODELVIEW);
  //cout<<"w h"<<x<<" "<<y<<" "<<getWidth()<<" "<<getHeight()<<endl;

  glRenderMode(GL_SELECT);
  glInitNames();//Clearing the name's stack 
  clearWindow(0.5,0.5,1);
  glPushName(-1);//Robot's name;
  setCamera(viewMatrix);
  for (unsigned int i=0;i<w->robots.size();i++){
    glLoadName(i);//Robot's name;
    glPushName(-1);//Robot's part
    /*double sides[3]={0.01,0.01,0.4};
    gdo->drawBox(sides,false);     
    glutSolidCube(0.1);*/
    gdo->drawODERobot(w->robots[i]);    
    glPopName();     
   }      
  glFlush();
  glutSwapBuffers();
  hits = glRenderMode(GL_RENDER);//get number of objects drawed in the area and return to render mode   
  glMatrixMode(GL_MODELVIEW);    
  double minz;
   if (hits){
     sensorID=buff[4];     
     robotID=buff[3];
     minz=(double)buff[1]/0x7fffffff;
   }else{
     robotID=-1;
     sensorID=-1;
   }
   //std::cout<<"buffh"<<hits<<std::endl;
   for (int i=0;i<hits;i++){
     double cmz=(double)buff[i * 5 + 1]/0x7fffffff;
     if (cmz<minz){
       minz=cmz;
       sensorID=buff[i * 5 + 4];
       robotID=buff[i * 5 + 3];       
     }     
   }
   //cout<<"sensor "<<sensorID<<endl;
   //cout<<"robotID"<<robotID<<endl;
   //int ax;
   //cin>>ax;
   //usleep(100000);
}


void SimulWin::selectSensor(int x,int y,bool force){

  static long long prevTime=getSystemMillis();
  long long currTime=getSystemMillis();
  if (currTime-prevTime<100&&!force){
    //std::cout<<currTime<<" "<<prevTime<<std::endl;
    return;
  }
  prevTime=currTime;
  int prevRobot=currRobot;
  int prevSensor=currSensor;
  getSensor(currRobot,currSensor,x,y);  
  //std::cout<<"select sensor"<<currRobot<<" "<<currSensor<<std::endl;
  for (unsigned int i=0;i<mouseSensorListener.size();i++){
    mouseSensorListener[i]->onSensorSelected(currSensor,
					   currRobot==-1?NULL:w->robots[currRobot]->srcRobot,
					   prevSensor,
					   prevRobot==-1?NULL:w->robots[prevRobot]->srcRobot);
					   
  }
  if (currRobot!=-1)
    lastSelRobot=currRobot;
}
}//helium
