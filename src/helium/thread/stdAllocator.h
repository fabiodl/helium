#ifndef HE_STDALLOCATOR
#define HE_STDALLOCATOR

#include <cstdlib.h>

namespace std{

  teplate<typename T>
  class AllocatorBase{

    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef const T* const_pointer; 
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    pointer address(reference x) const {return &x};
    const_pointer address(const_reference x) const {return &x};
    pointer allocate(size_type n, allocator<void>::const_pointer hint=0){
      return (pointer)realloc(hint,sizeof(value_type)*n);
    }
    

    pointer delete()

  };




}

#endif
