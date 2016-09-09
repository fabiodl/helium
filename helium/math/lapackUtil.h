/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_LAPACKUTIL_
#define HE_LAPACKUTIL
namespace helium {
void eigenVectors(double* vals,double *vects,double *m,int size); //eigenvectors are returned as rows of vects
void invert (double*  inv, double* input,int n);
void matrixToQuaternion(double* q,double * m);
void quaternionToMatrix(double*m,double *q);
void leastSquares(double* x,const double* a,double *b,int n,int m,int rhs,bool destroyB);//solves AX=B where x is NxM, X is MxRHS and B is NxRHS. if destroyB is true it calculates x using the b space, otherwise copies the matrix. B is comlumnwise
void svd_economicVrows(double* v,double*m, int r,int c); //return only the min(r,c) first v rows
}
#endif
