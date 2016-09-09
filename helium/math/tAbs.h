/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_TABS
#define HE_TABS

namespace helium{
  template<typename T> T tAbs(const T& t){
    return t>=0?t:-t; //fransiska 20150821 avoid -0
  }
}

#endif

