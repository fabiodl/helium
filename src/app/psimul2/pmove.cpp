#include "psimul.h"
#include <helium/robot/frame.h>
#include <helium/robot/motionAdapter.h>
#include <helium/robot/motionFilePlayer.h>
#include <helium/graphics/glDraw.h>

using namespace helium;

  





class Pmove:public PSimul{
  enum Motions{FORWARD,LEFT,RIGHT};

  

  void run(const Components& comp){

    MotionFilePlayer<SimulMotorSign> player(comp.ms,*comp.mfp,"neony");
    player.load(FORWARD,"forward.xide");
    player.load(LEFT,"turnleft.xide");
    player.load(RIGHT,"turnright.xide");

    
    player.play(LEFT);

    //player.play(FORWARD);
    //GlDraw::join();
  }
public:
  Pmove(){
    
  }
  

};

int main(int argc,char** argv){
   
  {
    Pmove pmove;  
    pmove.launch(argc,argv);
  }
  std::cout<<"main thread returned"<<std::endl;
  

}
