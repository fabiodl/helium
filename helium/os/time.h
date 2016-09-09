/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */


#ifndef HE_OSTIME
#define HE_OSTIME

#include <string>
#include <helium/core/time.h>

namespace helium{

  void milliSleep(int t);
  Time getSystemMillis();
  Time getElapsedMillis(Time t);
  std::string getAsciiDateTime();
  
};//helium


#endif
