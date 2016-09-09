/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */


#ifndef HE_MAKERETURNVALUE
#define HE_MAKERETURNVALUE

#include <helium/vector/operations.h>

namespace helium{

template<typename C,typename T> T makeReturnValue(void (C::*f)(T&),C& c){
  T x;
  (c.*f)(x);
  return x;
}


  template<typename A> A makeArrayReturnValue(int n,const typename A::value_type& value){
    A a(n);
    setAll(a,value);
    return a;
  }


}


#endif
