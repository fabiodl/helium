#ifndef ITARO_ROTOTRATOMOTORS
#define ITARO_ROTOTRATOMOTORS

#include <helium/vector/array.h>
#include <helium/itaro/rototra.h>



namespace helium{
  enum {LLEVER,RLEVER,BASE,TOP};
  
  struct Dim{
    static const double R;
    static const double C;
    static const double L2;
    static const double H;
    static const double MINALPHA;
    static const double MAXALPHA;
    
    Dim(){
    }
  };

  class Vector2d:public Array<double,2>{
  public:
    Vector2d(){
      data[0]=data[1]=NAN;
    }
    Vector2d(double x,double y){
      data[0]=x;
      data[1]=y;
    }
    
    using Array<double,2>::operator=;
    size_t size() const{
      return Array<double,2>::size();
    }
  };

  void leverToMotors(double* m,double h,double theta);
  void rototraToMotors(double* m,double* r);


  //limits

  double theta(double h,double alpha);
  double h(double theta,double alpha);
  double thetaLimit(double h);
  void hlimits(double& minh,double& maxh,double theta);

  
}


#endif
