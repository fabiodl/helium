#ifndef HE_FPS
#define HE_FPS


#include <helium/os/time.h>

namespace helium{


  class Fps{
    helium::Time pt;
  public:
    void print(){
      helium::Time t=helium::getSystemMillis();
      std::cout<<1000.0/(t-pt)<<" fps"<<std::endl;
      pt=t;
    }
  };


}

#endif
