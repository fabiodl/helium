/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_GENERICMOTORSIGN
#define HE_GENERICMOTORSIGN
#include <helium/robot/motorInterface.h>
#include <helium/robot/motorStructures.h>
namespace helium{

  struct RelMotorSign{
    typedef motor::Interface::Position Pos;
    typedef motor::Interface::Property Prop;
    static const bool NEEDSZERO=false;

    static const Prop INVALIDPROP;
    
    static const Pos DEFAULTMIN;
    static const Pos DEFAULTMAX;

    static const Pos INVALIDPOS;
  };

  struct AbsMotorSign{
    typedef int Pos;
    typedef int Prop;
    static const bool NEEDSZERO=true;

    static const Pos DEFAULTMIN;
    static const Pos DEFAULTMAX;
    static const Pos DEFAULTZERO;

    static const Pos INVALIDPOS;
    static const Prop INVALIDPROP;
    
    static const Pos MOVINGPOS;
    static const Pos FREEPOS;
    static const Pos COLLISIONPOS;

  };

  typedef motor::MotorCalib<RelMotorSign> RelativeLimit;
  

}

#endif
