/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/core/basicEnum.h>

namespace helium{
  const char* EnumStrings<Axis::Type>::strings[EnumStrings<Axis::Type>::NUM]={"X","Y","Z","NEGX","NEGY","NEGZ"};

  const char* EnumStrings<Rotation::Type>::strings[EnumStrings<Rotation::Type>::NUM]={"yaw","pitch","roll"};



}
