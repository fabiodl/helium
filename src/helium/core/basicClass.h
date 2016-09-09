#ifndef HE_BASICCLASS
#define HE_BASICCLASS

class Void{
};

template <typename T,bool b>
class trueToVoid{
};

template <typename T>
struct trueToVoid<T,false>{
  
    typedef T value;
};

  template <typename T>
  struct trueToVoid<T,true>{
    typedef Void value;
  };



template <typename T> struct VoidToFalse{
  static const bool value=true;
};

template<> struct VoidToFalse<Void>{
  static const bool value=false;
};

template <typename T> struct VoidToTrue{
  static const bool value=!VoidToFalse<T>::value;
};



#endif
