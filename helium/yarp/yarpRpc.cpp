/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/yarp/yarpRpc.h>


using namespace helium;
YarpRpc::YarpRpc(YarpClient& pc):
  client(pc){
}
