#include "odeMath.h"
#include <iostream>


using namespace std;


namespace helium{
  void transpose(dReal* t,dReal* s){
    for (int i=0;i<4;i++)
      for (int j=0;j<4;j++)
	t[i*4+j]=s[j*4+i];
  }
  
  
  void matrixToODEMatrix(dMatrix3 R,const double *m){  
    dRFrom2Axes(R,m[0],m[4],m[8],m[1],m[5],m[9]);
    /*  if (m[0]+m[4]+m[8]==0){
	printTrace();
	throw 3;
  }
  if (m[1]+m[5]+m[9]==0){
  printTrace();
  throw 5;
  }*/
    
  } 
  
  
  void odeRotationToMatrix(double*m,const dReal* _dvR,dReal tx,dReal ty,dReal tz){
    m[0]=  _dvR[0];
    m[4]=  _dvR[4];
    m[8]=  _dvR[8];
    m[12]=  0;
    m[1]=  _dvR[1];
    m[5]=  _dvR[5];
    m[9]=  _dvR[9];
    m[13]=  0;
    m[2]=  _dvR[2];
    m[6]=  _dvR[6];
    m[10]= _dvR[10];
    m[14]= 0;
    m[3]= tx;
    m[7]= ty;
    m[11]= tz;
    m[15]= 1;
  }
  
  
  void odeRotationToODEMatrix(dMatrix3 r,const dReal* rot){
    for (int i=0;i<4;i++)
      for (int j=0;j<3;j++){
	r[4*i+j]=rot[3*j+i];
      }
}
  
  void mulODE(dMatrix3 r,dMatrix3 a,dMatrix3 b){
    dQuaternion qa,qb,qr;
    dRtoQ (a,qa);
    dRtoQ (b,qb);
    dQMultiply0(qr,qa,qb);
    dQtoR(qr,r);
  }
  
  
  void addODE(dReal* r,dReal* a){
  for (int i=0;i<3;i++) r[i]+=a[i];
  }
  
  
  void subODE(dReal* r,dReal* a){
    for (int i=0;i<3;i++) r[i]-=a[i];
  }
  
  
  void mulODEAxis(dReal* r,double* m, dReal* v){
    r[0]=v[0]*m[0]+v[1]*m[1]+v[2]*m[2];
    r[1]=v[0]*m[4]+v[1]*m[5]+v[2]*m[6];
    r[2]=v[0]*m[8]+v[1]*m[9]+v[2]*m[10];
  }
  
  void mulODEPoint(dReal* r,dReal* m, dReal* v){
    r[0]=v[0]*m[0]+v[1]*m[1]+v[2]*m[2]+m[3];
    r[1]=v[0]*m[4]+v[1]*m[5]+v[2]*m[6]+m[7];
    r[2]=v[0]*m[8]+v[1]*m[9]+v[2]*m[10]+m[11];
  }
  
  
  
  
}
