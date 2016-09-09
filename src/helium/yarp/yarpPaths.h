/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_YARPPATHS
#define HE_YARPPATHS

#include <vector>
#include <string>

namespace helium{

class YarpPaths{
public:
  std::vector<std::string> paths;
  YarpPaths(const std::string& prefix);
};


}

#endif
