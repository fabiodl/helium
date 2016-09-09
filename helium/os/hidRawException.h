/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_HIDRAWEXCEPTION
#define HE_HIDRAWEXCEPTION

#include <helium/core/exception.h>

namespace helium{
  namespace exc{
  
  class HidRawException:public Exception{
  public:
    enum Operation{OPEN,REQUESTREAD,READ,WRITE,REQUESTLOOPREAD,NUMOPERATIONS};

    HidRawException(int dev,int addr,int length,Operation op,const std::string& detail="");
    virtual void rethrow();
    Exception* clone() const;
  };
  
  }

}


#endif
