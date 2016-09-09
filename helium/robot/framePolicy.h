/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags ge
 */

#ifndef HE_FRAMEPOLICY
#define HE_FRAMEPOLICY

#include <helium/robot/frame.h>
#include <helium/robot/policy.h>
#include <iostream>

namespace helium {
  /** \brief manage the interpolation between keyframes
   */
  class FramePolicy:public Policy{
    const Motion &motion;
    bool cubic;
  public:
    FramePolicy(const Motion &pmotion, bool pcubic = false);
    //double getPosture(int id, Time t, bool disabled);
    double getPosture(int id, Time t);
    double getInterpolatedPosture(int id,Time t);


    double getPostureBetween(helium::Time t,helium::Time prev, helium::Time next,int id);
    //void getPosture(Posture &p) {}
    void getPosture(double* p,Time t);
    void getPosture(Posture &p,Time t);
    Time getEndTime();
    Policy* clone() {return this;}

    helium::Motion::MotionMap getInterpolatedMotionMap(); ///< return interpolated motion (no NANs)
  };
}

#endif
