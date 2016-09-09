/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags ge
 */

#ifndef HE_RANDOM
#define HE_RANDOM

#include <cstdlib>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>


namespace helium{

  inline double randn(){
    double u1=((double)rand()+1)/((double)RAND_MAX+1);
    double u2=((double)rand())/((double)RAND_MAX+1);  
    return sqrt(-2*log(u1))*cos(2*M_PI*u2);  
  }

  template<typename T>
  inline void randn(T& data,size_t n){
    for (size_t i=0;i<n;i++){
      data[i]=randn();
    }
  }

  template<size_t n,typename T>
  inline void randn(T& data){
    for (size_t i=0;i<n;i++){
      data[i]=randn();
    }
  }



  template<typename T>
  inline void randn(T& t){
    randn(t,t.size());
  }
  
  inline double randu(){
    return ((double)rand())/((double)RAND_MAX+1);
  }

  inline double randu(double min,double max){
    return min+(max-min)*randu();
  }

  
  template<typename T>
  inline void randun(T& data,size_t n){
    for (size_t i=0;i<n;i++){
      data[i]=randu();
    }
  }

  template<size_t n,typename T>
  inline void randu(T& data){
    for (size_t i=0;i<n;i++){
      data[i]=randu();
    }
  }

  template<size_t n,typename T>
  inline void randun(T& v,double min,double max){
    for (size_t i=0;i<n;i++){
      v[i]=randu(min,max);
    }
  }


  template<typename T>
  inline void randun(T& v,double min,double max, size_t n){
    for (size_t i=0;i<n;i++){
      v[i]=randu(min,max);
    }
  }

  
  template<typename T>
  inline void randu(T& data){
    randun(data,data.size());
  }

  template<typename T>
  inline void randu(T& data,double min,double max){
    randun(data,min,max,data.size());
  }
  

  
  inline bool randb(double p){
    return randu()<p;
  }

  inline void randb(bool* b,int n,int p){
    for (int i=0;i<n;i++){
      b[i]=randu()<p;
    }
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
  
}
#endif
