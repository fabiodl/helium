/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <algorithm>
#include <vector>
#include <map>

#include <helium/signal/callback.h>
#include <helium/thread/thread.h>

#include <helium/graphics/glDraw.h>
#include <helium/graphics/rawImage.h>
#include <helium/graphics/glDraw_private.h>
#include <helium/graphics/glPrimitiveCanvas.h>
#include <helium/graphics/glUtil.h>
#include <helium/graphics/glHeader.h>
//#include <helium/destroy/destroy.h>
#include <helium/destroy/autoDestroy.h>
#include <helium/os/time.h>
#include <helium/thread/thread_private.h>
#include <helium/signal/tsSignal.h>
using namespace helium;



class Window:public helium::Callback<void>{    
public:

  typedef int WindowID;
  GlPrimitiveCanvas* canvas;
  int prevX,prevY,pushX,pushY;
  PrimitiveMouseInteraction::MouseButton button;
  bool shouldDraw;
  CountMutex drawStateMutex;
  int glutId;
  WindowID winId;
  helium::Time lastRedraw;
  void unloadTextures();
  Window(GlPrimitiveCanvas* pcanvas,WindowID id):canvas(pcanvas),prevX(-1),prevY(-1),pushX(-1),pushY(-1),		      
						 button(PrimitiveMouseInteraction::NOBUTTON),shouldDraw(false),
						 drawStateMutex("drawStateMutex"),winId(id),lastRedraw(0){
    canvas->requestDraw.connect(this);
    }
    ~Window();
    

  void operator()();//for connection to canvas->arequestdraw
  void draw(helium::Time now,helium::Time nextDraw);
  void initWindow();
    void handleKeyPressed(unsigned char key,int x, int y);
    void handleKeyReleased(unsigned char key,int x, int y);
    void handleMouseClick(int button, int state, int x, int y);
    void handleMouseMotion(int x,int y);
    void handleMousePassiveMotion(int x,int y);  
    static WindowID drawingWindow;
};
Window::WindowID Window::drawingWindow;
namespace helium{

class GlDrawer:public helium::Thread{
  int* argc;char** argv;
  void run();
  Semaphore drawingRequired;
  enum State{CREATED,RUNNING,STOPPING,STOPPED,DESTROYING};
  volatile State state;
  int nextId;
  Time calcPeriod();
  Time period;
  Time now;
  Time zero;
  ThreadId myID;
  Mutex stateMutex;
  friend class helium::GlDraw;
  TsSignal<void> onTermination; 
public: 
  bool isDestroying(){
    return state==DESTROYING;
  }
  Time getPeriod(){return period;}
  std::vector<Window*> winDet;
  std::map<int,Window*> winToData;
  std::map<std::pair<Window::WindowID,RawImage*>,TextInfo > textures;
  
  GlDrawer();
  ~GlDrawer();
  
  void set(int&pargc, char** pargv){
    argc=&pargc;
    argv=pargv;
  }
  void requestDraw(){
    drawingRequired.signal();
  }
  void idleFunc();
  void timerFunc();
  void push_back(GlPrimitiveCanvas*c);
  void requestTermination();
};
}

class Timer:public Thread{
  volatile bool running;
  Time period;
  
  void run();
public:
  ~Timer(){
    //std::cout<<"destroy timer"<<std::endl;
    running=false;
    join();
  }
  void setPeriod(Time pperiod){
    period=pperiod;
  }

};

static Timer timer;


static GlDrawer drawer;

static exc::PrintExceptionHandler printEh;
static exc::ExceptionHandler* eh=&printEh;






namespace helium{
  TextInfo& TextInfo::getCurrent(RawImage* image){
    return drawer.textures[std::make_pair(Window::drawingWindow,image)];
  }

}


void Timer::run(){
  {
    running=true;
    running=running&&(period!=0);
    while(running){
      milliSleep(period);      
      drawer.timerFunc();

    }
  }
  //std::cout<<"timer exited"<<std::endl;
}

void Window::unloadTextures(){
  glutSetWindow(glutId);
  typedef  std::map<std::pair<Window::WindowID,RawImage*>,TextInfo >::iterator IT;
  for(IT it=drawer.textures.begin(); it != drawer.textures.end(); ) {     
    if (it->first.first==winId) {
      if (it->second.state==TextInfo::LOADED){
	glDeleteTextures (1,&it->second.name);
      }
      drawer.textures.erase(it++);
    } else {
      ++it;
    }
  }
};

  

Window::~Window(){

  if (!drawer.isDestroying()){
    unloadTextures();
  }
}

void Window::operator()(){
  drawStateMutex.lock();
  shouldDraw=true;
  drawStateMutex.unlock();
  drawer.requestDraw();
}




void Window::draw(helium::Time now,helium::Time nextDraw){   
  drawStateMutex.lock(); 
  bool timeToDraw=false;
  //std::cout<<"now="<<now<<"next="<<nextDraw<<"last="<<lastRedraw<<std::endl;
  if (!shouldDraw){ //we check only if we're not sure;
    if (!nextDraw){
      timeToDraw=true;
    }else{
      if (canvas->getAutomaticRedrawPeriod()&&nextDraw>=lastRedraw+canvas->getAutomaticRedrawPeriod()){	
	  lastRedraw=now;
	  timeToDraw=true;
      }//if next is far      
    }//else (now!=1)
  }//if !shuldDraw;

  if (shouldDraw||timeToDraw){
    try{
      //std::cout<<"d/*"<<std::endl;
      glutSetWindow(glutId);
      //std::cout<<"d*/"<<std::endl;
      drawingWindow=winId;
      canvas->draw();
      glutSwapBuffers();
      //std::cout<<"drawing"<<std::endl;
      if (shouldDraw){
	shouldDraw=false;	
	canvas->drawCompleted();
      }
    }catch(exc::Exception& e){
      drawStateMutex.unlock();
      eh->handle(e);      
    }catch(...){	
      drawStateMutex.unlock();
      throw;
    }
  }//if
  drawStateMutex.unlock();
}

void Window::initWindow(){
  glutInitWindowSize(canvas->getWidth(),canvas->getHeight());  
  glutInitWindowPosition(canvas->getX(),canvas->getY());
  glutId=glutCreateWindow(canvas->getName().c_str());
  drawer.winToData[glutId]=this;
  initGL(canvas->getWidth(),canvas->getHeight());  
}

void Window::handleKeyPressed(unsigned char key,int x, int y){ 
  bool parsed=false;

  for (size_t i=0;i<canvas->keyboardInter.size();i++){   
    try{
      PrimitiveKeyboardInteraction::KeyParsingState s=canvas->keyboardInter[i]->onKeyPushed(key,x,y);
      parsed=parsed|(s!=PrimitiveKeyboardInteraction::UNMANAGED);  
      if (s==PrimitiveKeyboardInteraction::EXCLUSIVE) break;

    }catch(exc::Exception& e){
      eh->handle(e);
    }
  }
  if (!parsed)
    std::cout<<"Command "<<key<<"("<<(int)key<<") is not available."<<std::endl;     

}

void Window::handleKeyReleased(unsigned char key,int x, int y){
  bool parsed;
  for (size_t i=0;i<canvas->keyboardInter.size();i++){
    try{
      PrimitiveKeyboardInteraction::KeyParsingState s=canvas->keyboardInter[i]->onKeyReleased(key,x,y);
      parsed=parsed|(s!=PrimitiveKeyboardInteraction::UNMANAGED);  
      if (s==PrimitiveKeyboardInteraction::EXCLUSIVE) break;
    }catch(exc::Exception& e){
      eh->handle(e);
    }
  }
}


void Window::handleMouseClick(int btn, int state, int x, int y){
  if (state==GLUT_DOWN){
    switch(btn){
    case GLUT_LEFT_BUTTON:
      button=PrimitiveMouseInteraction::LEFTBUTTON;
      break;
    case GLUT_RIGHT_BUTTON:
      button=PrimitiveMouseInteraction::RIGHTBUTTON;
    break;
    case GLUT_MIDDLE_BUTTON:
      button=PrimitiveMouseInteraction::MIDDLEBUTTON;
      break;
    default:
      button=PrimitiveMouseInteraction::NOBUTTON;
    }//switch(button)

    prevX=pushX=x;
    prevY=pushY=y;
  }else{
    pushX=-1;
    pushY=-1;
  }
    
  for (size_t i=0;i<canvas->mouseInter.size();i++){
    try{
      canvas->mouseInter[i]->onMouseClicked(button,state==GLUT_DOWN,x,y);
    }catch(exc::Exception& e){
      eh->handle(e);
    }
  }//for
  glutPostRedisplay();
}


void Window::handleMouseMotion(int x,int y){

  for (size_t i=0;i<canvas->mouseInter.size();i++){
    try{
      canvas->mouseInter[i]->onMouseMoved(x,y,prevX,prevY,button,pushX,pushY);
    }catch(exc::Exception& e){
      eh->handle(e);
    }
  }
  prevX=x;
  prevY=y;
  glutPostRedisplay();
}

void Window::handleMousePassiveMotion(int x,int y){
  button=PrimitiveMouseInteraction::NOBUTTON;
  pushX=-1;
  pushY=-1;
  for (size_t i=0;i<canvas->mouseInter.size();i++){
    try{
      canvas->mouseInter[i]->onMouseMoved(x,y,prevX,prevY,button,pushX,pushY);
    }catch(exc::Exception& e){
      eh->handle(e);
    }
  }
  prevX=x;
  prevY=y;
  glutPostRedisplay();  
}

void drawGLScene(){
  drawer.winToData[glutGetWindow()]->draw(0,0);
}

void resizeGlScene(int w,int h){
  h=(std::max)(h,1);
  glViewport(0, 0, w, h);
  drawer.winToData[glutGetWindow()]->canvas->resize(w,h);
}

void keyPressed(unsigned char key,int x, int y){
  drawer.winToData[glutGetWindow()]->handleKeyPressed(key,x,y);
}


void keyReleased(unsigned char key,int x, int y){
  drawer.winToData[glutGetWindow()]->handleKeyReleased(key,x,y);
}


void mouseClick(int button, int state, int x, int y){
  drawer.winToData[glutGetWindow()]->handleMouseClick(button,state,x,y);
}

void mouseMotion(int x, int y){
  drawer.winToData[glutGetWindow()]->handleMouseMotion(x,y);  
}

void mousePassiveMotion(int x, int y){
  drawer.winToData[glutGetWindow()]->handleMousePassiveMotion(x,y);  
}


namespace helium{
GlDrawer::GlDrawer():state(CREATED),nextId(0),now(0),zero(getSystemMillis()),stateMutex("glDrawSM"){


};//period is initialized at run


void GlDrawer::requestTermination(){
  stateMutex.lock();
  switch(state){
  case CREATED:
  case RUNNING:
    state=STOPPING;
    break;
  case STOPPING:
  case STOPPED:
  case DESTROYING:
    onTermination.signalAndClear();
    break;
  }
  stateMutex.unlock();
};


GlDrawer::~GlDrawer(){
  //  std::cout<<"~GlDrawer()"<<std::endl;
  stateMutex.lock();
  state=DESTROYING;
  stateMutex.unlock();
  requestTermination();
  //std::cout<<"destroy drawer"<<std::endl;
  if (getThreadId()==myID){
    //std::cout<<"the destroier is glut"<<std::endl;
    //stopCurrentThread();
    deconstructWithoutJoin=true;
  }else{
    drawingRequired.signal();
    join();
  }
  //
  //std::cout<<"destroyed"<<std::endl;
  onTermination.signalAndClear();

  destroyS(winDet);   
  
}


void GlDrawer::idleFunc(){
  drawingRequired.wait();
  //std::cout<<"idle check"<<std::endl;
  stateMutex.lock();
  if (state!=RUNNING){    
    //std::cout<<"idle,stopping thread"<<std::endl;
    stateMutex.unlock();
    onTermination.signalAndClear();
    state=STOPPED;
    stopCurrentThread();
  }else{
    stateMutex.unlock();
  }
  
  for (size_t i=0;i<winDet.size();i++){
    winDet[i]->draw(now,now+period);
  }
}

void GlDrawer::timerFunc(){
  now=getElapsedMillis(zero);
  drawingRequired.signal();
}


void GlDrawer::push_back(GlPrimitiveCanvas*c){
  winDet.push_back(new Window(c,nextId));
  nextId++;
}
}
void idleFunc(){
  drawer.idleFunc();
}
namespace helium{
Time GlDrawer::calcPeriod(){
  Time t=0;
  Time lt;
  for (size_t i=0;i<winDet.size();i++){
    lt=winDet[i]->canvas->getAutomaticRedrawPeriod();
    if (t==0||lt<t){
      t=lt;
    } //if
    lt=winDet[i]->canvas->getEventParsingPeriod();
    if (t==0||lt<t){
      t=lt;
    } //if
    //std::cout<<"period is"<<t<<std::endl;
  }//for

  return t;
}



void GlDrawer::run(){
  stateMutex.lock();
  if (state!=CREATED){
    state=STOPPED;
    stateMutex.unlock();
    onTermination.signalAndClear();
    return;
  }
  myID=getThreadId();
  period=calcPeriod();
  timer.setPeriod(period/2);
  timer.start();
  glutInit(argc, argv);  

  //std::cout <<"init" << std::endl;
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);  

  for (size_t i=0;i<winDet.size();i++){
    winDet[i]->initWindow();        
    glutDisplayFunc(drawGLScene);      
    glutReshapeFunc(resizeGlScene);        
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyReleased);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(mousePassiveMotion);
    
    //glutTimerFunc(1000/FRAMERATE,::timerFunc,0); 
    //initGL(getX(),getY());
  }  

  glutIdleFunc(::idleFunc);

  state=RUNNING;
  stateMutex.unlock();
  glutMainLoop();

  //std::cout<<"glut returned"<<std::endl;
  stateMutex.lock();
  state=STOPPED;
  stateMutex.unlock();
  onTermination.signalAndClear();

}


}




namespace helium{
  void GlDraw::push_back(GlPrimitiveCanvas* c){
    drawer.push_back(c);
  }
  void GlDraw::start(int& argc,char** argv){
    drawer.set(argc,argv);
    drawer.start();
  }

  void GlDraw::execute(int& argc,char** argv){
    drawer.set(argc,argv);
    drawer.execute();
  }

  void setExceptionHandler(exc:: ExceptionHandler* peh){
    eh=peh;
  }



  void GlDraw::Stopper::operator()(){
    state.lock();
    alreadyDead=true;
    //std::cout<<"Termination signalled on stopper"<<std::endl;
    s.signal();
    state.unlock();    
  }

  GlDraw::Stopper::Stopper():cr(this),alreadyDead(false),state("glterm"){
    drawer.onTermination.connect(cr);
  }
  
  GlDraw::Stopper::~Stopper(){
    state.lock();
    if (!alreadyDead){
      drawer.onTermination.disconnect(cr);
    }else{
      //the disc information is automatically deleted by the ~ConnReference()
    }
    state.unlock();
    //std::cout<<"Stopper is gone"<<std::endl;
  }

  void GlDraw::Stopper::join(){
    state.lock();
    bool mustWait=!alreadyDead;
    state.unlock();
    if (mustWait){
      //std::cout<<"waiting for death"<<std::endl;
      s.wait();
    }
  }

  void GlDraw::Stopper::stop(){
    state.lock();
    if (!alreadyDead){
      drawer.requestTermination();
    }   
    state.unlock();
    join();
  }

};


void GlDraw::join(){
  GlDraw::Stopper stopper;
  stopper.join();
}
