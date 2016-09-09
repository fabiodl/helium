/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef POSTUREDESCRIPTION
#define POSTUREDESCRIPTION
#include <helium/system/cellDescription.h>
#include <helium/robot/frame.h>

namespace helium {
template<> 
class DefDescr<helium::Posture>:public Description<helium::Posture>{
public:    
  DefDescr(helium::Posture& s):Description<helium::Posture>(s){      
  }
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members) {
    //std::cout << "getmapmembers postureDescription" << std::endl;
    members.push_back(fscalarMapping<SILENTFAIL>(obj.name,"name"));
    members.push_back(scalarMapping(static_cast<Array<double>&>(obj),"angle"));
  }
};
}
#endif
