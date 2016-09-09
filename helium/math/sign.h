/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags ge
 */

#ifndef HE_MATHSIGN
#define HE_MATHSIGN

namespace helium{
  template<typename T> static inline int sign(T t){
    if (t>0) return 1;
    if (t<0) return -1;
    return 0;
  }

}
#endif
