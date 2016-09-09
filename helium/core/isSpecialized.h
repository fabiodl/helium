/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_ISSPECIALIZED
#define HE_ISSPECIALIZED

#include <helium/core/yesNo.h>

namespace helium{
  

  /*template<typename T>
  struct A{
    typedef int NotSpecialized;
    };*/


  template<typename T>
  struct isSpecialized{
    template<typename F>
    static tt::no test(typename F::Specialized);
    template<typename F>
    static tt::yes test(double);  
    static const bool value=(sizeof(test<T>(0))==sizeof(tt::yes));
  };
  

}




#endif
