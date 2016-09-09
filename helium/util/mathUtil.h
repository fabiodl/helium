/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_MATHUTIL
#define HE_MATHUTIL

#define _USE_MATH_DEFINES
#include <math.h>

namespace helium{

  inline void degtoRad(double &d) {
    d *= M_PI/180;
  }

  inline double getDegtoRad(double d) {
    return d * M_PI/180;
  }

  inline void radtoDeg(double &d) {
    d *= 180/M_PI;
  }

  inline double getRadtoDeg(double d) {
    return d * 180/M_PI;
  }

  inline bool segmentsAreDisjoint(double a,double b,double c,double d){ //returns if a..b intersects c..d  (a<=b,c<=d)
    if (b<c||a>d) return true;
    return false;
  }

  inline bool segmentsAreDisjoint(double a,double b,double c,double d,double margin){ //returns if a..b intersects c..d  (a<=b,c<=d)
    if (b+margin<c||a>d+margin) return true;
    return false; 
  }

  inline double distance(double* a,double *b) {
    double d=0;
    for (int i=0;i<3;i++) d+=(a[i]-b[i])*(a[i]-b[i]);
    return sqrt(d);
  }

  inline void hsv2rgb(double &r,double &g,double &b,double h,double s,double v){
    //std::cout<<"converting "<<h<<","<<s<<","<<v<<std::endl;
    double f= h*6-floor(h*6);
    double p = v * (1 - s);
    double q = v * (1 - s * f);
    double t = v * (1 -s * (1 - f));
    switch((int)floor(h*6)){
    case 0:
      r=v;
      g=t;
      b=p;
      break;
    case 1:
      r=q;
      g=v;
      b=p;
      break;
    case 2:
      r=p;
      g=v;
      b=t;
      break;
    case 3:
      r=p;
      g=q;
      b=v;
      break;
    case 4:
      r=t;
      g=p;
      b=v;
      break;
    case 5:
      r=v;
      g=p;
      b=q;
      break;
    default:
      break;
    }
  }


  template<typename R,typename N>
  inline R expandBits(R val,N n){
  if (val&(1<<(n-1)))
    return  (~((1<<n)-1))|val;
  else
    return val;
  }
  



};

#endif
