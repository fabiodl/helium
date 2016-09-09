/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags s
 */

#include <helium/android/androidMotorSign.h>
#include <helium/core/math.h>

namespace helium{


  namespace android{

    const MotorSign::Pos  MotorSign::INVALIDPOS=-1;
    const MotorSign::Prop  MotorSign::INVALIDPROP=-1;
    
    const MotorSign::Pos  MotorSign::MOVINGPOS=-2;
    const MotorSign::Pos  MotorSign::FREEPOS=-3;
    
    
    const double MotorSign::TORAW=0xFF/(M_PI);
    const double MotorSign::TONORM=(M_PI)/0xFF;
 
    
  }


}
