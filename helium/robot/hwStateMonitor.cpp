/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/robot/hwStateMonitor.h>
namespace helium{
  
  const char* EnumStrings<MonitorMode>::strings[EnumStrings<MonitorMode>::NUM]=
    {"N","F","I","U"};
}
