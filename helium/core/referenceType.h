/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.02.18
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_REFERENCETYPE
#define HE_REFERENCETYPE

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


  //natReference is used for passing to the copy constructors
  //simpleReference is used for compiler problems of reference to reference

  template<typename T> struct natReference{
    typedef const T& value;
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

  //-----
  
  template<typename T> struct simpleReference{
    typedef T& value;
  };

  template<typename T> struct simpleReference<T&>{
    typedef  T& value;
  };

  template<typename T> struct simpleReference<const T>{
    typedef const T& value;
  };

  template<typename T> struct simpleReference<const T&>{
    typedef const T& value;
  };


  template<typename T> struct simpleReference<T*>{
    typedef T* value;
  };

  template<typename T> struct simpleReference<const T*>{
    typedef const T* value;
  };

  template<> struct simpleReference<void>{
    typedef void value;
  };





 
}



#endif
