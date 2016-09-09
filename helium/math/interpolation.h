/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_INTERPOLATION
#define HE_INTERPOLATION

namespace helium {
inline void linearInterpolation(double*v, const double* v1,const double*v2,int length,double t1,double t2,double t){
  double p=(t-t1)/(t2-t1);
  for (int i=0;i<length;i++){
    v[i]=(1-p)*v1[i]+p*v2[i];
  }
}


  template<size_t length>
inline void linearInterpolationn(double*v, const double* v1,const double*v2,double t1,double t2,double t){
  double p=(t-t1)/(t2-t1);
  for (size_t i=0;i<length;i++){
    v[i]=(1-p)*v1[i]+p*v2[i];
  }
}

  inline double linearInterpolation(double x1,double y1,double x2,double y2,double x){
    return (x-x1)/(x2-x1)*(y2-y1)+y1;
  }

inline void cubicInterpolation(double* a,double* b,double*c,double*d, double x1,double y1,double x2,double y2){
  if (x1!=x2){
    double cube=(x1-x2)*(x1-x2)*(x1-x2);
    *a=-2*(y1-y2)/cube;
    *b=3*(x1+x2)*(y1-y2)/cube;
    *c=-6*x2*x1*(y1-y2)/cube;
    *d=(3*x2*x2*y1*x1 -x2*x2*x2*y1 +x1*x1*x1*y2 -3*x1*x1*y2*x2)/cube;
  }else{
    *a=0;
    *b=0;
    *c=0;
    *d=(y1+y2)/2;
  }
}

inline double getCubicValue(double a,double b,double c,double d,double x){
  double x2=x*x;
  return a*x*x2+b*x2+c*x+d;
}
}//namespace helium

#endif
