/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_DELETABLELIST
#define HE_DELETABLELIST

#include <helium/robot/hwInterface.h>
namespace helium{

  
  template<typename Payload> class DeletablePayload:public Payload,public Deletable{
  public:
    DeletablePayload(const Payload& t):Payload(t){}
  };

  typedef DeletablePayload<HwIdList> DeletableList;
 
  

  
}


#endif
