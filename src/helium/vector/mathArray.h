#ifndef HE_MATHARRAY
#define HE_MATHARRAY

#include <helium/vector/array.h>
#include <helium/vector/operations.h>

namespace helium{
  template <typename T,int N=array::RESIZABLE,typename Allocator=typename array::defAllocator<N>::value > 
  class MathArray:public MathObject<Array<T,N,Allocator> >{  
  };
}


#endif
