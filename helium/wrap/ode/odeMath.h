#ifndef _ODEMATH_
#define _ODEMATH_

#include <ode/ode.h>

namespace helium{
  void matrixToODEMatrix(dMatrix3 R,const double *m);
  void mulODE(dMatrix3 r,dMatrix3 a,dMatrix3 b);
  void subODE(dReal* r,dReal* a);
  void addODE(dReal* r,dReal* a);
  void mulODEAxis(dReal* r,dReal* m, dReal* v);
  void mulODEPoint(dReal* r,dReal* m, dReal* v);
  void odeRotationToODEMatrix(dMatrix3 r,const dReal* rot);
  void odeRotationToMatrix(double*m,const dReal* _dvR,dReal tx=0,dReal ty=0,dReal tz=0);
}
#endif

