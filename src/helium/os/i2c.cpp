#include <helium/os/i2c.h>


namespace helium{
  namespace exc{

    void I2CException::rethrow(){
      throw *this;
    }
    
    Exception* I2CException::clone() const{
	return new I2CException(*this);
    }
    
  }


}
