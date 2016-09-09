/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_ISSAME
#define HE_ISSAME
namespace helium{
   template<typename U,typename V> struct isSame{
      static const bool value=false;
    };

    template<typename U> struct isSame<U,U>{
      static const bool value=true;
    };

}


#endif
