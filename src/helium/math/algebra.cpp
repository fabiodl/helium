/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera 
 * \author  Fransiska Basoeki
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/math/algebra.h>

namespace helium{
  void Vector3d::cross(const Vector3d& a,const Vector3d& b) {
    helium::cross(*this,a,b);
  }
  
  Vector3d   Vector3d::ZERO(0,0,0);


  void getTranslation(Vector3d* t, const double* m){
    t->x()=m[3];
    t->y()=m[7];
    t->z()=m[11];
  }

  

  void setAsRotation(double* m,Axis::Type type,double rad){
    eye(m);
    setRotation(m,type,rad);
  }

  void setTranslation(double* m,double x,double y,double z){
    m[3]=x;
    m[7]=y;
    m[11]=z;
  }

  void setTranslation(double* m,const Vector3d* t){
    m[3]=t->x();
    m[7]=t->y();
    m[11]=t->z();
  }

  void setTranslation(double* m,const Vector3d& t){
    m[3]=t.x();
    m[7]=t.y();
    m[11]=t.z();
  }



  //rotation about arbitrary axis: direction u,v,w, passing through a,b,c 
  void getRotation(double* m,double a,double b,double c,double u,double v,double w,double theta){  
    double co=cos(theta);
    double si=sin(theta);
    double L=sqrt(u*u+v*v+w*w);
    if (L==0) 
      eye(m); 
    else{
      m[0]=u*u+(v*v+w*w)*co;
      m[1]=u*v*(1-co)-w*L*si;
      m[2]=u*w*(1-co)+v*L*si;
      m[3]=a*(v*v+w*w)-u*(b*v+c*w)+(u*(b*v+c*w)-a*(v*v+w*w))*co+(b*w-c*v)*L*si;
      
      m[4]=u*v*(1-co)+w*L*si;
      m[5]=v*v+(u*u+w*w)*co;
      m[6]=v*w*(1-co)-u*L*si;
      m[7]=b*(u*u+w*w)-v*(a*u+c*w)+(v*(a*u+c*w)-b*(u*u+w*w))*co+(c*u-a*w)*L*si;
      
      m[8]=u*w*(1-co)-v*L*si;
      m[9]=v*w*(1-co)+u*L*si;
      m[10]=w*w+(u*u+v*v)*co;
      m[11]=c*(u*u*+v*v)-w*(a*u+b*v)+(w*(a*u+b*v)-c*(u*u+v*v))*co+(a*v-b*u)*L*si;
      
      for (int i=0;i<12;i++) 
	m[i]/=L*L;
      m[12]=m[13]=m[14]=0;
      m[15]=1;
    }
  }
  

  void getRow(Vector3d* t,const double* m,int r){
    t->x()=m[r*4+0];
    t->y()=m[r*4+1];
    t->z()=m[r*4+2];
  }

  //rotations about the origin (wrl) http://inside.mines.edu/~gmurray/ArbitraryAxisRotation/
  void getRotation(double* m,double u,double v,double w,double theta){  
    double co=cos(theta);
    double si=sin(theta);
    double L=sqrt(u*u+v*v+w*w);
    if (L==0) 
      eye(m); 
    else{
      m[0]=u*u+(v*v+w*w)*co;
      m[1]=u*v*(1-co)-w*L*si;
      m[2]=u*w*(1-co)+v*L*si;    
      
      m[4]=u*v*(1-co)+w*L*si;
      m[5]=v*v+(u*u+w*w)*co;
      m[6]=v*w*(1-co)-u*L*si;    
      
      m[8]=u*w*(1-co)-v*L*si;
      m[9]=v*w*(1-co)+u*L*si;
      m[10]=w*w+(u*u+v*v)*co;
      
      for (int i=0;i<3;i++) 
	for (int j=0;j<3;j++) {
	  m[i*4+j]/=L*L;    
	}
    }
  }

  void mul(double* r,const double* m,const double* n){
  for (int i=0;i<4;i++){
    for (int j=0;j<4;j++){
      r[i*4+j]=0;
      for (int k=0;k<4;k++){
	r[i*4+j]+=m[i*4+k]*n[k*4+j];
      }
    }
  }
  }

  void mul(Vector3d* r,const double* m, const Vector3d* s){
  r->x()=s->x()*m[0]+s->y()*m[1]+s->z()*m[2]+m[3];
  r->y()=s->x()*m[4]+s->y()*m[5]+s->z()*m[6]+m[7];
  r->z()=s->x()*m[8]+s->y()*m[9]+s->z()*m[10]+m[11];
}



  void mulAxis(Vector3d* r,const double* m, const Vector3d* s){
    r->x()=s->x()*m[0]+s->y()*m[1]+s->z()*m[2];
    r->y()=s->x()*m[4]+s->y()*m[5]+s->z()*m[6];
    r->z()=s->x()*m[8]+s->y()*m[9]+s->z()*m[10];
  }

  void mulAxis(double* r,const double* m, const double* s){
    r[0]=s[0]*m[0]+s[1]*m[1]+s[2]*m[2];
    r[1]=s[0]*m[4]+s[1]*m[5]+s[2]*m[6];
    r[2]=s[0]*m[8]+s[1]*m[9]+s[2]*m[10];
  }


  void mulPoint(double* r,const double *m,const double *p){
    r[0]=p[0]*m[0]+p[1]*m[1]+p[2]*m[2]+m[3];
    r[1]=p[0]*m[4]+p[1]*m[5]+p[2]*m[6]+m[7];
    r[2]=p[0]*m[8]+p[1]*m[9]+p[2]*m[10]+m[11];
  }


  void mulPoints(double* r,const double *m,const double *p,int n){
  for (int i=0;i<n;i++){
    r[0+i*3]=p[0+i*3]*m[0]+p[1+i*3]*m[1]+p[2+i*3]*m[2]+m[3];
    r[1+i*3]=p[0+i*3]*m[4]+p[1+i*3]*m[5]+p[2+i*3]*m[6]+m[7];
    r[2+i*3]=p[0+i*3]*m[8]+p[1+i*3]*m[9]+p[2+i*3]*m[10]+m[11];
  }
  }

  void mulNegativeTranslationMatrix(double* m,Vector3d &v){
    for (int i=0;i<3;i++){
      m[3+i*4]-=m[0+i*4]*v.x()+m[1+i*4]*v.y()+m[2+i*4]*v.z();
    }
  }


  void getAxis(Vector3d* r, Axis::Type axis){
  switch (axis){
  case Axis::X:
    r->set(1,0,0);
    break;
  case Axis::Y:
    r->set(0,1,0);
    break;
  case Axis::Z:
    r->set(0,0,1);
    break;
  case Axis::NEGX:
    r->set(-1,0,0);
    break;
  case Axis::NEGY:
    r->set(0,-1,0);
    break;
  case Axis::NEGZ:
    r->set(0,0,-1);
    break;    
  }
  }


  void getAxis(Vector3d* r, double* m,Axis::Type axis){
    r->set(m[axis],m[axis+4],m[axis+8]);
  }




  //added by fb 20110614
  void setRotation(double* m,Axis::Type type,double rad){
    double s=sin(rad);
    double c=cos(rad);
    switch (type){
    case Axis::X:
      m[0]=1;        m[1]=0;         m[2]=0;
      m[4]=0;        m[5]=c;         m[6]=-s;
      m[8]=0;        m[9]=s;         m[10]=c;     
      break;
    case Axis::Y:
      m[0]=c;        m[1]=0;         m[2]=s;
      m[4]=0;        m[5]=1;         m[6]=0;
      m[8]=-s;       m[9]=0;         m[10]=c;     
      break;
    case Axis::Z:
      m[0]=c;        m[1]=-s;        m[2]=0;
      m[4]=s;        m[5]=c;         m[6]=0;
      m[8]=0;        m[9]=0;         m[10]=1;     
      break;            
    case Axis::NEGX:
      m[0]=1;        m[1]=0;         m[2]=0;
      m[4]=0;        m[5]=c;         m[6]=s;
      m[8]=0;        m[9]=-s;         m[10]=c;     
      break;
    case Axis::NEGY:
      m[0]=c;        m[1]=0;         m[2]=-s;
      m[4]=0;        m[5]=1;         m[6]=0;
      m[8]=s;       m[9]=0;         m[10]=c;     
      break;
    case Axis::NEGZ:
      m[0]=c;        m[1]=s;        m[2]=0;
      m[4]=-s;        m[5]=c;         m[6]=0;
      m[8]=0;        m[9]=0;         m[10]=1;     
      break;            
    default:
      break;
    }//case    
  }//setRotation
  

  void setRotation(double* m,double m00,double m01,double m02,double m10,double m11,double m12,double m20,double m21,double m22){

    m[0]=m00; m[1]=m01; m[2]=m02;
    m[4]=m10; m[5]=m11; m[6 ]=m12;
    m[8]=m20; m[9]=m21; m[10]=m22;

  }


  void getReflection(double* m,double* dir){
    double n=0;
    for (int i=0;i<3;i++){
      n+=dir[i]*dir[i];
    }
    for (int i=0;i<3;i++){
      for (int j=0;j<3;j++){
	m[i*4+j]=(i==j?1:0)-2*dir[i]*dir[j]/n;      
      }
    }
    m[3]=m[7]=m[11]=m[12]=m[13]=m[14]=0;
    m[15]=1;
  }
  void invert(double* r,const double*m){
    for (int i=0;i<3;i++)
      for (int j=0;j<3;j++)
	r[i*4+j]=m[j*4+i];
    for (int c=0;c<3;c++){
      r[c*4+3]=0;
      for (int i=0;i<3;i++)
	r[c*4+3]-=m[i*4+c]*m[i*4+3];
    }
    r[12]=r[13]=r[14]=0;
    r[15]=1; 
  }

  //end of add

#define SMALL_NUM  0.00000001

int intersectRayTriangle(Vector3d rp0,Vector3d rp1, Vector3d tv0, Vector3d tv1, Vector3d tv2, Vector3d* i,double epsilon )
{
    Vector3d    u, v, n;             // triangle vectors
    Vector3d    dir, w0, w;          // ray vectors
    double     r, a, b;             // params to calc ray-plane intersect

    // get triangle edge vectors and plane normal
    u.sub(tv1,tv0);
    v.sub(tv2,tv0);
    n.cross(u,v); // cross product
    if (n.isZero())            // triangle is degenerate
        return -1;                 // do not deal with this case

    dir.sub(rp1,rp0);             // ray direction vector
    w0.sub(rp0,tv0);
    a=-n.dot(w0);
    b=n.dot(dir);

    if (fabs(b) < SMALL_NUM) {     // ray is parallel to triangle plane
        if (a == 0)                // ray lies in triangle plane
            return 2;
        else return 0;             // ray disjoint from plane
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0)                   // ray goes away from triangle
        return 0;                  // => no intersect
    // for a segment, also test if (r > 1.0) => no intersect
    i->set(dir);
    i->mulScalar(r);
    i->sum(rp0); // intersect point of ray and plane
    // is I inside T?
    double    uu, uv, vv, wu, wv, D;
    uu = u.dot(u);
    uv = u.dot(v);
    vv = v.dot(v);
    w.sub(*i,tv0);
    wu = w.dot(u);
    wv = w.dot(v);
    D = uv * uv - uu * vv;
    // get and test parametric coords
    double s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0-epsilon || s > 1.0+epsilon)        // I is outside T
        return 0;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0-epsilon || (s + t) > 1.0+epsilon)  // I is outside T
        return 0;

    return 1;                      // I is in T
}

int nearestFace(Vector3d point,double* rototrans,double a,double b,double c,bool centered){  
  double v[3*8];
  double nv[3];
  getBoxVertex(v,a,b,c,centered);
  
  for (int i=0;i<8;i++){//rotate the cube according to the robotranslation
    mulPoint(nv,rototrans,&v[i*3]);
    v[i*3]=nv[0];    v[i*3+1]=nv[1];     v[i*3+2]=nv[2];
  }
  Vector3d rp0;
  Vector3d center(0,0,0);
  if (!centered){
    center.set(a/2,b/2,c/2);
  }
  mul(&rp0,rototrans,&center);
  Vector3d tv0,tv1,tv2,tv3,r;
  int f=-1;
  double epsilon=0;
  while (f==-1){
    for (int i=0;i<6&&f==-1;i++){
      tv0.set(&v[boxQuadFaces[i*4]*3]);
      tv1.set(&v[boxQuadFaces[i*4+1]*3]);
      tv2.set(&v[boxQuadFaces[i*4+2]*3]);
      tv3.set(&v[boxQuadFaces[i*4+3]*3]);
      if (intersectRayTriangle(rp0,point,tv0,tv1,tv2,&r,epsilon)||
	  intersectRayTriangle(rp0,point,tv2,tv3,tv0,&r,epsilon))
	f=i;
    }//for face
    epsilon+=0.1;
  }//f==-1
  return f;
}


int nearFaces(int* faces,Vector3d point,double* rototrans,double a,double b,double c,bool centered){  
  double v[3*8];
  double nv[3];
  getBoxVertex(v,a,b,c,centered);  
  for (int i=0;i<8;i++){//rotate the cube according to the robotranslation
    mulPoint(nv,rototrans,&v[i*3]);
    v[i*3]=nv[0];    v[i*3+1]=nv[1];     v[i*3+2]=nv[2];
  }
  Vector3d rp0;
  Vector3d center(0,0,0);
  if (!centered){
    center.set(a/2,b/2,c/2);
  }
  mul(&rp0,rototrans,&center);
  Vector3d tv0,tv1,tv2,tv3,r;
  int n=0;
  for (int i=0;i<6;i++){
    tv0.set(&v[boxQuadFaces[i*4]*3]);
    tv1.set(&v[boxQuadFaces[i*4+1]*3]);
    tv2.set(&v[boxQuadFaces[i*4+2]*3]);
    tv3.set(&v[boxQuadFaces[i*4+3]*3]);
    if (intersectRayTriangle(rp0,point,tv0,tv1,tv2,&r,0.1)||
	intersectRayTriangle(rp0,point,tv2,tv3,tv0,&r,0.1))
      faces[n++]=i;
  }//for face
  return n;
}


int normalFace(Vector3d& normal,double* roto){
  double v[3*8];
  getBoxVertex(v,1,1,1);  
  double maxValue=0;
  int maxAxis=0;
  Vector3d axis;
  double dot;
  for (int i=0;i<3;i++){
    axis.set(roto[0+i],roto[4+i],roto[8+i]);
    dot=axis.dot(normal);
    if (fabs(dot)>fabs(maxValue)){
      maxValue=dot;
      maxAxis=i;
    }
  }
  switch(maxAxis){
  case 0:
    if (maxValue>0) return 5; else return 3;
  case 1:
    if (maxValue>0) return 2; else return 4;
  case 2:
    if (maxValue>0) return 0; else return 1;
  }
  return -1;
}//normalFace


}//helium
