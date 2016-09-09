#ifndef ITARO_MOVEMENTS
#define ITARO_MOVEMENTS

namespace helium{

  void twist(double* m,double t,double turn,double mh);
  void lean(double* m,double t,double lean,double mh);
  void rise(double* m,double t,double lean,double mh);
  void ellipse(double* m,double t,double dx,double lean,double mh);
  void look(double* m,double t,double twist,double lean,double mh);



}


#endif
