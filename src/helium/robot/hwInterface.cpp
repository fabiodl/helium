/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/robot/hwInterface.h>

namespace helium{
  const char* EnumStrings<HwState>::strings[EnumStrings<HwState>::NUM]={"U","OK","GF","VF","UR"};

}
