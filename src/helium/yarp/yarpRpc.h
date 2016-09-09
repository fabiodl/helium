/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_YARPRPC
#define HE_YARPRPC

#include <helium/yarp/yarpClient.h>

namespace helium{
class YarpRpc{




  YarpClient& client;

public:
  YarpRpc(YarpClient& c);
  template<typename T>
  void get(T& t,const std::string& path,const std::string& what);
  template<typename T>
  void set(const T& t,const std::string& path,const std::string& what);   


};


}

#include <helium/yarp/yarpRpc.hpp>

#endif
