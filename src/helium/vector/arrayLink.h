#infdef HE_ARRAYLINK
#define HE_ARRAYLINK

#include <helium/vector/array.h>


namespace helium{


  template<typename T,typename Alloc=typename array::defAllocator<array::RESIZABLE>::value>
  class ArrayLink:public Array<T,array::RESIZABLE,Alloc>{


    ArrayLink& operator=(const ArrayLink& b)

  public:
    typedef ArrayLink:public Array<T,array::RESIZABLE,Alloc> Super;
    
    ArrayLink(Array<T,array::RESIZABLE,Alloc>& a){
      data=a.data;
      length=a.size();
    }

   
    
    void resize(size_t n){
      length=n;
    }


    void clear(){
      length=0;
    }

    ~ArrayLink(){
      data=NULL;
      length=0;
    }

};

}//ns helium

#endif
