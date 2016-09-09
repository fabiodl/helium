#ifndef HE_VELLIMITER
#define HE_VELLIMITER

#include <helium/core/math.h>
#include <helium/math/tAbs.h>
#include <helium/math/sign.h>
#include <helium/vector/operations.h>
namespace helium{

  
  template<typename T>
  class VelLimiter{
    Array<T> data;
    double maxJump;

  public:
    operator T* (){
      return data.data;
    }
    
    operator const T*() const{
      return data.data;
    }



    VelLimiter(int n,double pmaxJump=0):data(n),maxJump(pmaxJump){
      zero(data);
    }

    void setMaxJump(double mj){
      maxJump=mj;
    }
    
    void force(T* v){
      assign(data,v);
    }

    void set(T* newVal,double maxJump){
      for (size_t i=0;i<data.size();++i){
	data[i]=data[i]+sign(newVal[i]-data[i])*min(maxJump,tAbs(newVal[i]-data[i]));
      }      
    }


    void set(T* newVal){
      for (size_t i=0;i<data.size();++i){
	data[i]=data[i]+sign(newVal[i]-data[i])*min(maxJump,tAbs(newVal[i]-data[i]));
      }      
    }
    
    
  };



}

#endif
