/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera 
 * \author  Fransiska Basoeki
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_ALGEBRA
#define HE_ALGEBRA

#include <helium/vector/array.h>
#include <helium/vector/operations.h>
#include <helium/core/basicEnum.h>

#ifdef _WIN32
#define  _USE_MATH_DEFINES
#include <math.h> 
#endif

namespace helium{

  static const int CUBEFACENUM=6;
  static const int CUBEVERTEXNUM=8;




  class Vector3d:public MathObject<Array<double,3> >{
  public:

    static Vector3d ZERO;

    Vector3d() {}
    Vector3d(double a, double b, double c) {
      data[0] = a;
      data[1] = b;
      data[2] = c;
    }
    Vector3d(const double *d) {
      data[0] = d[0];
      data[1] = d[1];
      data[2] = d[2];
    }
    using MathObject<Array<double,3> >::set;

    void set(const double& a,const double& b,const double& c){
      data[0] = a;
      data[1] = b;
      data[2] = c;
    }

    void setNegX(Vector3d& s){
      data[0]=-s.data[0];
      data[1]=s.data[1];
      data[2]=s.data[2];
    }

    inline double& x(){
      return data[0];
    }

    inline double& y(){
      return data[1];
    }

    inline double& z(){
      return data[2];
    }


    inline const double& x() const{
      return data[0];
    }

    inline const double& y() const{
      return data[1];
    }

    inline const double& z() const{
      return data[2];
    }
    //added 20110614 by fb
    void cross(const Vector3d& a,const Vector3d& b);


    //end of add
  };//Vector



  inline std::ostream& operator<<(std::ostream& o,const Vector3d& v){
    return o<<v.x()<<" "<<v.y()<<" "<<v.z();
  }


  inline std::istream& operator>>(std::istream& i,Vector3d& v){
    return i>>v.data[0]>>v.data[1]>>v.data[2];
  }

  

  template<> 
  struct hasSize<Vector3d>{
    static const bool value=true;
  };


  /*
  namespace Axis {
    enum vals {X,Y,Z,NUM};
  }
  */

  //added by fb 20110613
  //todo: change to template?
  void getRow(Vector3d* t,const double* m,int c);
  void getTranslation(Vector3d* t, const double* m);
  void setAsRotation(double* m,Axis::Type type,double rad);
  void setTranslation(double* m,const Vector3d* t);
  void setTranslation(double* m,const Vector3d& t);
  void setTranslation(double* m,double x,double y,double z);
  inline void setTranslation(double* m,const double* v){
    m[3]=v[0];
    m[7]=v[1];
    m[11]=v[2];   
  }
  //rotation about arbitrary axis: direction u,v,w,
  void getRotation(double* m,double u,double v,double w,double theta);
  //rotation about arbitrary axis: direction u,v,w, passing through a,b,c 
  void getRotation(double*m,double a,double b,double c,double u,double v,double w,double theta);

  void mulAxis(Vector3d* r,const double* m, const Vector3d* s);
  void mulAxis(double* r,const double* m, const double* s);

  void mulPoint(double* r,const double *m,const double *p);
  void mulPoints(double* r,const double *m,const double *p,int n);
  void mul(double* r,const double *n,const double *m);
  void mul(Vector3d* r,const double* m, const Vector3d* s);//same as mulPoint

  void mulNegativeTranslationMatrix(double* m,Vector3d &v); //M=M* [ 0 v]^-1  
  void getAxis(Vector3d* r, Axis::Type axis);

  void getAxis(Vector3d* r,double* m,Axis::Type axis);



  template<typename R,typename A,typename B> void mulMat(R& r,A& a,B& b,size_t ar,size_t ac,size_t bc){
    for (size_t i=0;i<ar;i++){
      for (size_t j=0;j<bc;j++){
	r[i*bc+j]=0;
	for (size_t k=0;k<ac;k++){
	  r[i*bc+j]+=a[i*ac+k]*b[k*bc+j];
	}
      }
    }
  }


  inline void eye(double* m,int a=4,int b=4){
    memset(m,0,sizeof(double)*a*b);
    for (int i=0;i<(std::min)(a,b);i++){
      m[i*b+i]=1;
    }
  }
  

  int nearestFace(Vector3d point,double* rototrans,double a,double b,double c,bool centered=true);
  int nearFaces(int* faces,Vector3d point,double* rototrans,double a,double b,double c,bool centered=true);
  int normalFace(Vector3d& normal,double* roto);



  /* moved to operations.h

  //template isinlimit
  template <typename T> inline bool isInLimits(T minv,T maxv,T x){
    return (x>=minv&&x<=maxv);
  }

  //template return norm
  template <typename R,typename T> inline R norm(const T&a, int n) {
    R r = 0;
    for(int i=0;i<n;i++)
      r+=a[i]*a[i];
    return sqrt(r);
  }
  template <typename A> inline typename A::value_type norm(const A &a) {
    return norm<typename A::value_type,A>(a,a.size());
  }
  //end off add 20110613
  */

  //added by fb 20110614
  static const int boxQuadFaces[]={
    0,3,2,1,
    4,5,6,7,
    0,1,5,4,
    1,2,6,5,
    2,3,7,6,
    0,4,7,3
  };
  void setRotation(double* m,Axis::Type type,double rad);
  void setRotation(double* m,double m00,double m01,double m02,double m10,double m11,double m12,double m20,double m21,double m22);
  void getReflection(double* m,double* dir);
  inline void copyMatrix(double* m,const double* s){
    memcpy(m,s,sizeof(double)*16);
  }
  void invert(double* r,const double*m);

  //template getBoxVertex
  template <typename A, typename B> inline void getBoxVertex(A &r, B a, B b, B c, bool centered=false) {
    B xo=centered?a/2:0;
    B yo=centered?b/2:0;
    B zo=centered?c/2:0;
    r[0]=0-xo; r[1]=0-yo; r[2]=0-zo;
    r[3]=a-xo; r[4]=0-yo; r[5]=0-zo;
    r[6]=a-xo; r[7]=b-yo; r[8]=0-zo;
    r[9]=0-xo; r[10]=b-yo;r[11]=0-zo;
    r[12]=0-xo;r[13]=0-yo;r[14]=c-zo;
    r[15]=a-xo;r[16]=0-yo;r[17]=c-zo;
    r[18]=a-xo;r[19]=b-yo;r[20]=c-zo;
    r[21]=0-xo;r[22]=b-yo;r[23]=c-zo;     
  }

  //template transpose
  template <typename A,typename B> inline void transpose(A &a,const B &b,int n=4) {
    for (int i=0;i<n;i++)
      for (int j=0;j<n;j++)
	a[i*n+j]=b[j*n+i];
  }

  //template return dot
  template <typename R,typename A, typename B> inline R dot(const A&a, const B&b, int n) {
    R r = 0;
    for(int i=0;i<n;i++)
      r+=a[i]*b[i];
    return r;
  }
  /*template <typename A, typename B> inline typename A::value_type dot(const A &a,const B &b) {
    return dot<typename A::value_type,A>(a,b,a.size());
    }*/ //HACK, FIX THIS,see operations.h


  //template return pointPointDistance
  template <typename R,typename A, typename B> inline R pointPointDistance(const A&a, const B&b, int n) {
    R d = 0;
    R diff;
    for (int i=0;i<n;i++){
      diff=a[i]-b[i];
      d+=diff*diff;
    }
    return sqrt(d);
  }
  template <typename A, typename B> inline typename A::value_type pointPointDistance(const A &a,const B &b) {
    return pointPointDistance<typename A::value_type,A>(a,b,a.size());
  } 
  //todo: pointPointDistance<double>(d,e,3), <double>ok? same with others

  //template cross
  template <typename R, typename A, typename B> inline void cross(R &r, const A &a, const B &b) {
    //std::cout << "crosshere" << std::endl;
    r[0]=a[1]*b[2]-a[2]*b[1];
    r[1]=a[2]*b[0]-a[0]*b[2];
    r[2]=a[0]*b[1]-a[1]*b[0];
  }


  //template setColumn
  template <typename R, typename A> void setColumn(R &r, const A &a, int c, int nc, int na) {
    for(int i=0;i<na;i++)
      r[i*nc+c] = a[i];
  }
  template <typename R, typename A> void setColumn(R &r, const A &a, int c, int nc=4) {
    setColumn(r,a,c,nc,a.size());
  }
  /*template <typename R, typename A> void setColumn(R &r, const A &a, int c) {
    setColumn(r,a,c,a.size(),a.size());
    }*/


  template<typename T>
  inline int sgn(T& t){
    if (t>0) return 1;
    if (t<0) return -1;
    return 0;
  }

  //end of add
}


#endif
