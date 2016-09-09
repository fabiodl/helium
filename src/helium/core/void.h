/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */


#ifndef HE_BASICCLASS
#define HE_BASICCLASS

#include <iostream>
#include <helium/core/exception.h>

namespace helium{

  class Void{ 
  public:
    Void(){}
    class ParamEater{
      ParamEater(...){}
    };
    
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


  inline std::ostream& operator<<(std::ostream& o,const Void &s){
    throw exc::SyntaxError("Operating on a Void");
    return o;    
  }

  inline std::istream& operator>>(std::istream& i,Void &s){
     throw exc::SyntaxError("Operating on a Void");
    return i;
  }


}


#endif
