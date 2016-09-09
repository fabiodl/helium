#ifndef HE_CONSTREFERENCE
#define HE_CONSTREFERENCE

namespace helium{

  template<typename T> struct constReference{
    typedef  const T& value;
  };

  template<typename T> struct constReference<T&>{
    typedef const T& value;
  };

  template<typename T> struct constReference<const T&>{
    typedef const T& value;
  };

  template<> struct constReference<void>{
    typedef void value;
  };


  template<typename T> struct natReference{
    typedef  const T& value;
  };

  template<typename T> struct natReference<T&>{
    typedef  T& value;
  };

  template<typename T> struct natReference<const T&>{
    typedef const T& value;
  };

  template<typename T> struct natReference<T*>{
    typedef T* value;
  };

  template<typename T> struct natReference<const T*>{
    typedef const T* value;
  };


  template<> struct natReference<void>{
    typedef void value;
  };


 
}



#endif
