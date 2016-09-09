/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/yarp/yarpPaths.h>


namespace helium{

YarpPaths::YarpPaths(const std::string& prefix){
  paths.push_back(prefix+"head");
  paths.push_back(prefix+"torso");
  paths.push_back(prefix+"left_arm");
  paths.push_back(prefix+"right_arm");
  paths.push_back(prefix+"left_leg");
  paths.push_back(prefix+"right_leg"); 

}

}
