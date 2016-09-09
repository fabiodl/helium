#ifndef HE_POINTERAUTOCTR
#define HE_POINTERAUTOCTR


namespace helium{

  template<typename T> class HeapAutoCtor{
    T* t;
  public:
    HeapAutoCtor():t(new T()){
    }    
    T* operator->(){
      return t;
    }

    T* get(){
      return t;
    }

    const T* operator->() const{
      return t;
    }

  };
  

};


#endif
