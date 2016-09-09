/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_DYNAMIXELPROBECHECK
#define HE_DYNAMIXELPROBECHECK


#include <helium/dynamixel/dynamixelSerial.h>


namespace helium{

  class DynamixelCheck{
  public:
    int baudrate;
    int motid;
    static const int TIMEOUT=300;    
      
    void check(const std::string &name){      
      Serial s(name.c_str(),baudrate,TIMEOUT);
      DynamixelSerial<Serial> ds(s,motid);
      ds.ping(motid);
    }


  };
    
  
  template<> 
  class DefDescr<DynamixelCheck>:public Description<DynamixelCheck>{
  public:
    DefDescr(DynamixelCheck& p):Description<DynamixelCheck>(p){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(scalarMapping(obj.baudrate,"baudrate"));
      members.push_back(scalarMapping(obj.motid,"motorId"));
    }
  };


}


#endif
