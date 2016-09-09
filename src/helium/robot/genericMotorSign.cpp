/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/robot/genericMotorSign.h>
namespace helium{
  
  const RelMotorSign:: Pos  RelMotorSign::DEFAULTMIN=-M_PI;
  const RelMotorSign:: Pos  RelMotorSign::DEFAULTMAX=+M_PI;
  const RelMotorSign:: Prop RelMotorSign::INVALIDPROP=NAN;
  const RelMotorSign:: Pos RelMotorSign::INVALIDPOS=NAN;



  const AbsMotorSign::Pos AbsMotorSign:: DEFAULTMIN=-1;
  const AbsMotorSign::Pos AbsMotorSign:: DEFAULTMAX=-1;
  const AbsMotorSign::Pos AbsMotorSign::DEFAULTZERO=-1;

  const AbsMotorSign::Pos AbsMotorSign::INVALIDPROP=-1;
  const AbsMotorSign::Pos AbsMotorSign::INVALIDPOS=-1;
  
  const AbsMotorSign::Pos AbsMotorSign::MOVINGPOS=-2;
  const AbsMotorSign::Pos AbsMotorSign::FREEPOS=-3;
  const AbsMotorSign::Pos AbsMotorSign::COLLISIONPOS=-4;


}
