/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_YARPMOTIONPLAYER
#define HE_YARPMOTIONPLAYER

#include <helium/yarp/yarpMotor.h>
#include <helium/robot/framePolicy.h>
#include <helium/robot/timeSource.h>
#include <helium/robot/motorFramePlayMode.h>
#include <helium/thread/timer.h>


namespace helium{
  

  class YarpMotionPlayer:
    public Thread{    
    Mutex mutex;
    bool threadRunning;
    YarpMotor& ym;
    RealTimeSource timeSource;

    struct TempData{
      std::vector<double> posture,maxv,v;
      TempData(int);
    } tempData;

    Motion currMotion;
    FramePolicy policy;
    MotionParameter motionParam;

    void run();
    Semaphore hasWork;
    ConnReference<void> hasWorkConnection;
    PosticipableTimer timer;
    Motion::MotionMap::iterator currFrame;
    Time zeroTime;
    bool playing;
  public:
    Signal<std::pair<helium::framePlay::PlayState,Time> > framePlayState;

    void play(const Motion&,const MotionParameter& );
    void stop();
    YarpMotionPlayer(YarpMotor& ,const double* maxv);
    ~YarpMotionPlayer();
  };


  

}





#endif
