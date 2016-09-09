/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#include <helium/vstone/vServoSign.h>
#include <helium/core/math.h>


namespace helium{
 const VServoSign::Pos  VServoSign::INVALIDPOS=-1;
  const VServoSign::Prop  VServoSign::INVALIDPROP=-1;

  const VServoSign::Pos  VServoSign::MOVINGPOS=-2;
  const VServoSign::Pos  VServoSign::FREEPOS=-3;
  

  const double VServoSign::TORAW=(0xDC5-0x237)/M_PI;
  const double VServoSign::TONORM=M_PI/(0xDC5-0x237);
  

}
