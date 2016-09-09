/*Fabio Dalla Libera   sourceforge.net/projects/libhelium/  */

#ifndef HE_CORE
#define HE_CORE
#include <iostream>
#include <cmath>

#define DEGREES (M_PI/180)

#ifdef _WIN32

#undef max
#undef min
#endif
#include <limits>

#include <helium/core/exception.h>

namespace helium{
  typedef long long int Time;
  static const Time INVALIDTIME=-1;
 

   
#ifdef _WIN32
} 
static const unsigned long nan[2]={0xffffffff, 0x7fffffff};
#define NAN (*(const double *) nan)
static inline int isnan(double x){return x!=x;}
#define M_PI 3.14159265358979323846
inline double round(double x){return floor(x+0.5);}
namespace helium{
#endif


  /*  template<typename T> T tAbs(const T& t){
    return t>0?t:-t;
  }
  */
  /* double not working
  inline std::ostream& operator<<(std::ostream& o,const MotorValue& m){
    if (isnan(m)) return o<<"Nan";
    o<<d;
    return o;
  }
  
  inline std::istream& operator>>(std::istream& i,MotorValue& m){
    std::string s;
    i>>s;
    std::stringstream ss(s);
    ss>>static_cast<double>(m); 
    if (ss.fail()){
      if (ss.str()=="NaN"){
	m=NAN;      
      }else{
	i.set(ios::failbit);
      }
    }
    
    return i;
    }*/




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


};//helium
#endif
