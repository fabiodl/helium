/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/device/deviceException.h>

namespace helium{

  namespace exc{
    void DeviceException::rethrow(){
      throw *this;
    }

     Exception* DeviceException::clone() const{
       return new DeviceException(*this);
    }
    

  }

}
