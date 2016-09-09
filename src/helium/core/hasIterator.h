/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_HASITERATOR
#define HE_HASITERATOR

#include <helium/core/yesNo.h>

namespace helium{


  template<typename T> struct hasConstIterator{

    template<typename T2>
    static tt::yes check(typename T2::const_iterator* it,int*);
    template<typename T2,typename X>
    static tt::no check(const int*,const X*);    
    static const bool value=sizeof(check<T>(NULL,(int*)NULL))==sizeof(tt::yes);

  };

  template<typename T>  struct hasConstIterator<T*>{
    static const bool value=false;    
  };


  template<typename T>  struct hasConstIterator<T&>{
    static const bool value=hasConstIterator<T>::value;    
  };


}


#endif
