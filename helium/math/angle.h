#ifndef HE_ANGLES
#define HE_ANGLES

#include <cmath>

namespace helium{


  inline double angleDist(double a,double b){
    double d=fabs(a-b);
    while(d>2*M_PI){
      d-=2*M_PI;
    }
    if (d<M_PI) return d;
    return 2*M_PI-d;
  }




  
  /* legend: _ = -	
             Uppercase=included
	     lowercase=excluded

   */
  inline double angleNorm_piPi(double a){
    while (a<=-M_PI) a+=2*M_PI;
    while (a>M_PI) a-=2*M_PI;
    return a;
  }


  


  inline double angleNormZero2pi(double a){
    while (a<0) a+=2*M_PI;
    while (a>=2*M_PI) a-=2*M_PI;
    return a;
  }


  inline double mod(double range,double x){
    while (x<0) x+=range;
    while (x>range) x-=range;
    return x;
  }


}


#endif
