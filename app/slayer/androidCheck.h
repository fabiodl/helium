/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags s
 */

#ifndef _SLAYER_ANDROIDCHECK_
#define _SLAYER_ANDROIDCHECK_

#include "androidMd.h"
#include <helium/system/cellDescription.h>

class AndroidCheck{
public:
  
  void check(const std::string &name){
    helium::Serial s(name.c_str(),AndroidMd::BAUDRATE,AndroidMd::TIMEOUT);
    helium::mout(helium::msg::PINFO)(name+" opened: assuming it is an A-lab android");
  }
  
};

namespace helium{

DECLARE_DEFAULT_DESCRIPTION(AndroidCheck)

  void DefDescr<AndroidCheck>:: getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
  }

}//ns helium


#endif
