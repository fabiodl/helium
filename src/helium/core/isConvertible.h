/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_ISCONVERTIBLE
#define HE_ISCONVERTIBLE

#include <helium/core/yesNo.h>

namespace helium{
  
  template<typename T,typename F> struct isConvertible{
    struct AnythingOk{
      template<typename X>AnythingOk(X& x);
    };
    

    static tt::yes conv(const T& t,int);
    
    template<typename X>
    static tt::no conv(const AnythingOk& t,X);
    static const F& f();
    static int g();
    static const bool value=sizeof(conv(f(),g()))==sizeof(tt::yes);
  
  };


};


#endif
