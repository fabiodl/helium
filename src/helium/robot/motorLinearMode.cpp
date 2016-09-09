/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * Release_flags s
 * \version 0.1
 */

#include <helium/robot/motorLinearMode.h>

namespace helium{

  
   const char* EnumStrings<mlin::TargetType>::strings[EnumStrings<mlin::TargetType>::NUM]={
     "PV","EPV","HLT"
  };
  
}
