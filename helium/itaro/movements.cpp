#include <helium/core/core.h>
#include <helium/core/basicEnum.h>
#include <helium/math/algebra.h>

#include <helium/itaro/movements.h>
#include <helium/itaro/rototra.h>
#include <helium/itaro/rototraToMotors.h>

namespace helium{


void twist(double* m,double t,double turn,double mh){  
  RotoTranslation r;
  double p=sin(t*2*M_PI);
  setAsRotation(r,Axis::Z,turn*p);
  r[RT_TZ]=mh*p;
  //print(r);
  //cout<<p<<endl;
  rototraToMotors(m,r);
}




void lean(double* m,double t,double lean,double mh){  
  RotoTranslation r;
  double p=sin(t*2*M_PI);
  setAsRotation(r,Axis::X,lean*cos(t*2*M_PI));
  r[RT_TZ]=mh*p;
  //print(r);
  //cout<<p<<endl;
  rototraToMotors(m,r);
}



void rise(double* m,double t,double lean,double mh){  
  RotoTranslation r;
  double p=sin(t*2*M_PI);
  setAsRotation(r,Axis::Y,lean*cos(t*2*M_PI));
  r[RT_TZ]=mh*p;
  //print(r);
  //cout<<p<<endl;
  rototraToMotors(m,r);
}


void ellipse(double* m,double t,double dx,double lean,double mh){  
  RotoTranslation rhead,rlean,r;
  double p=sin(t*2*M_PI);
  setAsRotation(rhead,Axis::Z,dx*p);
  setAsRotation(rlean,Axis::Y,lean*cos(t*2*M_PI));
  mul(r,rhead,rlean);  
  r[RT_TZ]=mh*p;
  //print(r);
  //cout<<p<<endl;
  rototraToMotors(m,r);
}


void look(double* m,double t,double twist,double lean,double mh){  
  RotoTranslation r,rdown,rhead;
  double p=sin(t*2*M_PI);
  setAsRotation(rhead,Axis::Z,twist*p);
  setAsRotation(rdown,Axis::Y,lean*sin(t*M_PI));
  mul(r,rhead,rdown);  
  rototraToMotors(m,r);
}

}
