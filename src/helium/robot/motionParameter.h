/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_MOTIONPARAMETER
#define HE_MOTIONPARAMETER


#include <helium/core/time.h>

namespace helium{

  struct MotionParameter {
    MotionParameter( int i=1, helium::Time t=0, double s=1):iters(i),startTime(t),playSpeed(s) {}
    int iters;
    Time startTime;
    double playSpeed;  
  };

}
#endif
