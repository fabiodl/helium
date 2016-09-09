/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_DEVICEEXCEPTION
#define HE_DEVICEEXCEPTION

#include <helium/core/exception.h>


namespace helium{

  namespace exc{
    class DeviceException:public Exception{
    public:
      DeviceException(const std::string& s):Exception(s){
      }

      DeviceException(const std::string& s,const std::string& d=""):Exception("Device "+s+addDetail(d)){
      }

      virtual void rethrow();
      Exception* clone() const;
    };
    
  }
}



#endif
