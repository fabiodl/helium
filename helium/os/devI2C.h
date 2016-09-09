#ifndef HE_DEVI2C
#define HE_DEVI2C

#include <string>
#include <helium/core/time.h>


namespace helium{ 
  class DevI2C{
    int fd;
    const std::string devname;
  public:
    DevI2C(const std::string& filename,int address);
    void write(const unsigned char* c,int n);
    void read(unsigned char* c,int n);    
  };
  
  class DelayedDevI2C:public DevI2C{
    Time delay;
  public:
    DelayedDevI2C(const std::string& filename,int address,Time delay);
    void write(const unsigned char* c,int n);
  };


}




#endif
