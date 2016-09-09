/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/dynamixel/dynamixelSign.h>
#include <helium/core/math.h>

namespace helium{


  namespace dynamixel{

    const Mx28Sign::Pos  Mx28Sign::INVALIDPOS=-1;
    const Mx28Sign::Prop  Mx28Sign::INVALIDPROP=-1;
    
    const Mx28Sign::Pos  Mx28Sign::MOVINGPOS=-2;
    const Mx28Sign::Pos  Mx28Sign::FREEPOS=-3;
    
    
    const double Mx28Sign::TORAW=0x800/M_PI;
    const double Mx28Sign::TONORM=M_PI/0x800;
 
    
  }


}
