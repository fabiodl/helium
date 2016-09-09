/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_DEBUGCAST
#define HE_DEBUGCAST


#include <cstddef>
#include <helium/core/exception.h>
#include <helium/debug/debugOptions.h>


namespace helium{

#ifdef DEBUGCAST
  
  template<typename T,typename T2,bool dynamicOK> class TrustCast;
  
  template<typename T,typename T2>
  class TrustCast<T,T2,true>{
  public:
    static T trust_cast(T2 t2){
      T t=dynamic_cast<T>(t2);
      if (t==NULL){	
	throw exc::InvalidCast(t2,t);
      }
      return t;
    }
  };
  
  template<typename T,typename T2>
  class TrustCast<T,T2,false>{
  public:
    static T trust_cast(T2 t2){      
      return static_cast<T>(t2);
    }
  };
  
  
  
  
  template<typename T>
  struct DynamicOK{
    static const bool value=true;    
  };


  template<>
  struct DynamicOK<const void*>{
    static const bool value=false;    
  };


  
  template<>
  struct DynamicOK<void*>{
    static const bool value=false;    
  };

 


template<typename T,typename T2>
T trust_cast(T2 t2){
  return TrustCast<T,T2,DynamicOK<T>::value&&DynamicOK<T2>::value>::trust_cast(t2);
}


#else
#define trust_cast static_cast
#endif

}
#endif
