#ifndef HE_POSITIONDATA
#define HE_POSITIONDATA

#include <helium/core/core.h>
namespace helium{
  template<typename T> class PositionData{
    enum Type{ABSOLUTE,RELATIVE}; //to current position
    T val;
    Time t;
  };
};

#endif
