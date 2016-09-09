/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/robot/motorInterface.h>

namespace helium{
  namespace motor{
    const Interface::Property Interface::INVALIDPROP=NAN;
    const Interface::Position Interface::INVALIDPOS=NAN;

    const Interface::Position Interface::MOVINGPOS=-2000;
    const Interface::Position Interface::FREEPOS=-3000;
    const Interface::Position Interface::COLLISIONPOS=-4000;

     namespace spt{
       const char* noHwData::NOHARDWAREERRORMSG="The interface corresponds to no-hardware";
     }
    
  }

 
  const char* EnumStrings<motor::MotorLevel>::strings[]={"N","R","H"};
  
  const char* EnumStrings<motor::PropertyID>::strings[EnumStrings<motor::PropertyID>::NUM]=
    {"TARGET","POTENTIO","POWER","TEMP","RAWPROP"};

}
