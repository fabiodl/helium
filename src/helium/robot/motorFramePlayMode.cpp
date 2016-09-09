/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/robot/motorFramePlayMode.h>


namespace helium{
  const char*  EnumStrings<framePlay::PlayState>::strings[EnumStrings<framePlay::PlayState>::NUM]=
    {"p","r","s"};

}
