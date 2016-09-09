#ifndef HE_MOTIONFILEPLAYER
#define HE_MOTIONFILEPLAYER

#include <helium/robot/frame.h>
#include <helium/robot/motorFramePlayMode.h>
#include <helium/robot/motionAdapter.h>
#include <helium/robot/motionParser.h>
//#include <helium/system/cellConnReference.h>


namespace helium{  

  template<typename MotorSign>
  class MotionFilePlayer{
    size_t n;

    MotorSpooler<MotorSign>& ms;
    MotorFramePlayMode<MotorSign>& framePlay;
    MotionParser parser;


    std::map<int,framePlay::Target> motions;

  public:

    MotionFilePlayer(MotorSpooler<MotorSign>& pms, MotorFramePlayMode<MotorSign>& pframePlay,const std::string& s):n(pms.getMotorNum()),ms(pms),framePlay(pframePlay),parser(s){}


    void load(int id,const std::string& filename){
      Motion m(n);      
      
      parser.openMotion(m,filename.c_str());
      motions.insert(std::pair<int, framePlay::Target>(id,framePlay::Target(n)));
      setMotionData(motions[id],m);
    }

    void play(int id){
      TargetEndReason ter;
      ms.setMotorMode(framePlay.getMotorModeId());
      framePlay.cells.wholeTarget.setData(ter,motions[id]);      
    }   
    
  };//MotionFilePlayer
  
  


}//helium





#endif
