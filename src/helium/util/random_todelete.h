#ifndef _GAUSSIANNOISE_
#define _GAUSSIANNOISE_
#include <cstdlib>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

inline double randn(){
  double u1=((double)rand()+1)/((double)RAND_MAX+1);
  double u2=((double)rand())/((double)RAND_MAX+1);  
  return sqrt(-2*log(u1))*cos(2*M_PI*u2);  
}

inline void randn(double* data,int n){
  for (int i=0;i<n;i++){
    data[i]=randn();
  }
}

inline double randu(){
  return ((double)rand())/((double)RAND_MAX+1);
}

inline void randu(double* data,int n){
  for (int i=0;i<n;i++){
    data[i]=randu();
  }
}


inline double randu(double min,double max){
  return min+(max-min)*randu();
}

inline int randi(std::vector<double> &weights){
  double v=randu();
  int el=0;
  double sum=0;
  do{
    sum+=weights[el++];
  }while(sum<v);
  return el-1;
  
}


inline double myRand(int i,int j){
  static const int N1=179;
  static const int N2=45;
  long int pos=i+N2*j;
  double u1=((double)((long int)pow((double)pos,2)%N1+1))/(N1+2);
  double u2=((double)((long int)pow((double)pos,3)%N1+1))/(N1+2);
  return sqrt(-2*log(u1))*cos(2*M_PI*u2);
}


inline void randomPermutation(int *p,int N){
  for (int i = 0; i < N; ++i) {
    int j = rand() % (i + 1);
    p[i] = p[j];
    p[j] = i;
  }
}

#endif
