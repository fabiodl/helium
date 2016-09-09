/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags ge
 */

#include <helium/robot/policy.h>
#include <iostream>

using namespace helium;

Policy::Policy() {
}

double Policy::getPosture(int id, Time t) {
  std::cerr<<"Implement a movement by subclasses"<<std::endl;
  return 0;
}
