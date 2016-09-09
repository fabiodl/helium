#ifndef HE_FIFOPIPE
#define HE_FIFOPIPE

#include <list>
#include <helium/thread/mutex.h>
namespace helium{
template <typename T>
class FifoPipe{
  std::list<T*> data;
public:
  void operator<<(const T& t){
    data.push_back(new T(t));
  }

  void pop()
  

};

}//helium

#endif
