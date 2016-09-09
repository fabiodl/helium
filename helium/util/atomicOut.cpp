#include <helium/util/atomicOut.h>
#include <iostream>
namespace helium{
  MtLog<std::ostream> atomicOut(std::cout);
}
