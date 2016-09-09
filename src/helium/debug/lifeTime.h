#ifdef HE_LIFETIME
#define HE_LIFETIME

#include <helium/core/time.h>

namespace helium{

  class LifeTime{   
    std::string s;
    Time zeroT;
  public:
    LifeTime(const std::string& s);    
    ~LifeTime();

  };

}
#endif
