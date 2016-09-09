/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include "lapackUtil.h"
#include <cstring>
#include <helium/util/ioUtil.h>
#include <helium/core/exception.h>
#include <stdlib.h> //malloc

#ifdef _WIN32

extern "C"{
#include "f2c.h"
#include "clapack.h"
}

int DSYEV(char jobz,char uplo,double* m,double *v,double *work,int &lwork,int size)
{
int info;
dsyev_(&jobz,&uplo,&size,m,&size,v,work,&lwork,&info);
return info;
}
int DGESVD(char jobu,char jobvt,int m,int n,double* a,int lda,double* sv,double* u,int ldu,double* vt,int ldvt,double* work,int lwork)
{
int info;
dgesvd_(&jobu,&jobvt,&m,&n,a,&lda,sv,u,&ldu,vt,&ldvt,work,&lwork,&info);
return info;
}

#else

#define F77_FUNC(x,y) x##_


extern "C"{
  void F77_FUNC(dsyev,DSYEV)(char *jobz, char *uplo, int *n, double *a, 
			     int *lda, double *w, double *work, int *lwork, int *info);

  void F77_FUNC(dgesvd,DGESVD)(char *jobu,char *jobvt,int *m,int *n,
			       double* a,int *lda,double* sv,double* u,
			       int *ldu,double* vt,int *ldvt,double* work,int *lwork,int* info);


  //for inverse

#ifdef __APPLE__
# define ipfint int
#endif
  // LU decomoposition of a general matrix
  //void F77_FUNC(dgetrf,DGETRF)(ipfint *m, ipfint *n,
  //			       double *A, ipfint *ldA, ipfint *IPIV,
  //			       ipfint *info);
  // generate inverse of a matrix given its LU decomposition
  //void F77_FUNC(dgetri,DGETRI)(int* N, double* A, int* lda, int* IPIV, double* WORK, int* lwork, int* INFO);
};

int DSYEV(char jobz,char uplo,double* m,double *v,double *work,int &lwork,int size)
{
int info;
F77_FUNC(dsyev,DSYEV)(&jobz,&uplo,&size,m,&size,v,work,&lwork,&info);
 return info;
}

int DGESVD(char jobu,char jobvt,int m,int n,double* a,int lda,double* sv,double* u,int ldu,double* vt,int ldvt,double* work,int lwork)
{
int info;
 F77_FUNC(dgesvd,DGESVD)(&jobu,&jobvt,&m,&n,a,&lda,sv,u,&ldu,vt,&ldvt,work,&lwork,&info);
 return info;
}
#endif

//for inverse
#ifndef _WIN32
extern "C" {
  // LU decomoposition of a general matrix
  void dgetrf_(int* M, int *N, double* A, int* lda, int* IPIV, int* INFO);
  
  // generate inverse of a matrix given its LU decomposition
  void dgetri_(int* N, double* A, int* lda, int* IPIV, double* WORK, int* lwork, int* INFO);
}

#endif

using namespace std;

namespace MatrixElements{
enum MatrixElements{xx,xy,xz,tx,
		    yx,yy,yz,ty,
		    zx,zy,zz,tz,
		    x0,y0,z0,o};
}

void helium::eigenVectors(double* vals,double *vects,double *m,int size){ //eigenvectors are returned as rows of vects
  int lwork=size*size*size;
  double* work=new double[lwork];
  memcpy(vects,m,sizeof(double)*size*size);
  DSYEV('V','U',vects,vals,work,lwork,size); //DEBUG WIN VC2010 
  delete[] work;
}

void helium::svd_economicVrows(double* v,double*m, int r,int c){
  // we take u as v so we don't need to transpose
  double* singVals=new double[(std::min)(r,c)];
  double wkopt;
  //     jobu jobvt m n a lda    s    u ldu  vt  ldvt work lwork
  DGESVD('S',  'N', c,r,m, c,singVals,v, c, NULL, r,  &wkopt,  -1); //DEBUG WIN VC2010 
  int lwork=(int)wkopt;
  //std::cout<<"work is"<<lwork<<std::endl;
  double* work=new double[lwork];
  //     jobu jobvt m n a lda    s    u ldu  vt  ldvt work lwork
  DGESVD('S',  'N', c,r,m, c,singVals,v, c, NULL, r,  work,lwork); //DEBUG WIN VC2010 
  delete[] work; 
  delete[] singVals;
}



/*
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace ublas = boost::numeric::ublas;

using namespace ublas;
*/

 /* Matrix inversion routine.
    Uses lu_factorize and lu_substitute in uBLAS to invert a matrix */
void helium::invert ( double*  inv, double* input,int n) {

  //fb 20111014
  memcpy(inv,input,sizeof(double)*n);

  int *IPIV = new int[n+1];
  int LWORK = n*n;
  double *WORK = new double[LWORK];
  int INFO;
  

  dgetrf_(&n,&n,inv,&n,IPIV,&INFO); //DEBUG WIN VC2010 
  dgetri_(&n,inv,&n,IPIV,WORK,&LWORK,&INFO); //DEBUG WIN VC2010 
  
  delete IPIV;
  delete WORK;
  
  /*   typedef permutation_matrix<std::size_t> pmatrix;
   // create a working copy of the input
   matrix<double> A(n,n);
   matrix<double> inverse(n,n);
   for (int i=0;i<n*n;i++) A(i/n,i%n)=input[i];
   // create a permutation matrix for the LU-factorization
   pmatrix pm(A.size1());
   
   // perform LU-factorization
   int res = lu_factorize(A,pm);
   if( res != 0 ) return;
   
   // create identity matrix of "inverse"
   inverse.assign(ublas::identity_matrix<double>(A.size1()));
   
   // backsubstitute to get the inverse
   lu_substitute(A, pm, inverse);


  for (int i=0;i<n*n;i++) inv[i]=inverse(i/n,i%n);

  */

  cerr << "WARNING: unconfirmed implementation of inverse" << endl;
  //throw helium::exc::Bug("CHECK IMPLEMENT INVERSE!!!");

 }


//http://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
void helium::matrixToQuaternion(double* q,double * m){
  using namespace MatrixElements;
  double qt[16];
  //looks just half of the matrix
  qt[0]=m[xx]-m[yy]-m[zz];  //qt[1]=m[yx]+m[xy];       qt[2]= m[zx]+m[xz];       qt[3]=m[yz]-m[zy];
  qt[4]=m[yx]+m[xy];      qt[5]=m[yy]-m[xx]-m[zz]; //qt[6]= m[zy]+m[yz];       qt[7]=m[zx]-m[xz];
  qt[8]=m[zx]+m[xz];        qt[9]=m[zy]+m[yz];     qt[10]=m[zz]-m[xx]-m[yy]; //qt[11]=m[xy]-m[yx];
  qt[12]=m[yz]-m[zy];       qt[13]=m[zx]-m[xz];      qt[14]=m[xy]-m[yx];     qt[15]=m[xx]+m[yy]+m[zz];

  for (int i=0;i<4;i++){
    for (int j=0;j<=i;j++){
      qt[i*4+j]/=3;
    }
  }
  double vals[4];double vects[16];
  eigenVectors(vals,vects,qt,4);
  int mv=0;
  for (int i=1;i<4;i++){
    if (vals[i]>vals[mv]) mv=i;
  }

  double norm=0;
  for (int i=0;i<4;i++){
    norm+=vects[mv*4+i]*vects[mv*4+i];
  }
  norm=sqrt(norm);
  q[0]=vects[mv*4+0]/norm;
  q[1]=vects[mv*4+1]/norm;
  q[2]=vects[mv*4+2]/norm;
  q[3]=vects[mv*4+3]/norm;
}

void helium::quaternionToMatrix(double*m,double *q){
  double a=q[0],b=q[1],c=q[2],d=q[3];
  m[0]=a*a-b*b-c*c+d*d; m[1]=2*a*b+2*c*d;     m[2]=2*a*c-2*b*d;
  m[4]=2*a*b-2*c*d;     m[5]=b*b-a*a-c*c+d*d; m[6]=2*a*d+2*b*c;
  m[8]=2*a*c+2*b*d;     m[9]=2*b*c-2*a*d;     m[10]=c*c+d*d-a*a-b*b;
}


void normalizeRototra(double *m){
  double q[4];
  helium::matrixToQuaternion(q,m);
  //cout<<"q=";
  //print(q,4)<<endl;
  helium::quaternionToMatrix(m,q);
  m[12]=m[13]=m[14]=0; m[15]=1;
}

#ifndef _WIN32
extern "C"{
extern void dgels_( char* trans, int* m, int* n, int* nrhs, double* a, int* lda,
                double* b, int* ldb, double* work, int* lwork, int* info );
}
#endif

void helium::leastSquares(double* x,const double* a,double *pb,int n,int m,int nrhs,bool destroyB){//solves AX=B where x is NxM, X is BxRHS and B is NxRHS
  int lda=m;//how far elements of consecutive cols are in memory
  int ldb=(std::max)(m,n);//how far elements of consecutive cols are in memory
  int info,lwork;
  double wkopt;
  double* work;
  double* b;
  char* transp=(char*)"Transpose";
  if (destroyB){
    b=pb;
  }else{
    b=new double[ldb*nrhs];
    memcpy(b,pb,sizeof(double)*n);
    //std::cout<<"created a "<<n*nrhs<<"@"<<b<<std::endl;
  }
  /* Query and allocate the optimal workspace */
  lwork = -1;
  dgels_(transp , &m, &n, &nrhs, (double*)a, &lda, b, &ldb, &wkopt, &lwork, &info ); //DEBUG WIN VC2010  
  lwork = (int)wkopt;
  work = (double*)malloc( lwork*sizeof(double) );
  /* Solve the equations A*X = B */
  dgels_(transp, &m, &n, &nrhs, (double*)a, &lda, b, &ldb, work, &lwork,&info ); //DEBUG WIN VC2010 
  /* Check for the full rank */
  if( info > 0 ) {
    cout<<"The diagonal element %i of the triangular factor "<< info<<
      "of A is zero, so that A does not have full rank; the least squares solution could not be computed."<<endl;
    return;
  }
  //print(b,n,nrhs,cout);
  for (int i=0;i<nrhs;i++){
    for (int j=0;j<m;j++){
      x[i*m+j]=b[n*i+j];
    }
  }
  if (!destroyB){
    //std::cout<<"destroying "<<b<<std::endl;
    delete[] b;
  }
  
}

