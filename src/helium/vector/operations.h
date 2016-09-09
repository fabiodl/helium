/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera Fransiska Basoeki
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_OPERATIONS_
#define HE_OPERATIONS_
#include <iostream>
#include <list>
#include <cmath>
#include <vector>
#include <map>
#include <limits>

#include <helium/core/hasIterator.h>
#include <helium/core/hasSize.h>
#include <helium/math/tAbs.h>

#ifndef _WIN32
#include <memory.h>
#endif

namespace helium{


 

  template<typename T1,typename T2,typename T3,bool H1=hasSize<T1>::value,bool H2=hasSize<T2>::value,bool H3=hasSize<T3>::value>
  struct GetSize{
    //None of the operands has size
  };
  
  
  template<typename T1,typename T2,typename T3,bool H2,bool H3>
  struct GetSize<T1,T2,T3,true,H2,H3>  //the first operand has size
  {
   
    static inline size_t getSize(const T1& t1,const T2& t2,const T3& t3){
      return t1.size();
    }

    static inline size_t getSize(const T1& t1,const T2& t2){
      return t1.size();
    }

  };

  template<typename T1,typename T2,typename T3,bool H3>
  struct GetSize<T1,T2,T3,false,true,H3>//the second operand has size
  {
    
    static inline size_t getSize(const T1& t1,const T2& t2,const T3& t3){
      return t2.size();
    }

    static inline size_t getSize(const T1& t1,const T2& t2){
      return t2.size();
    }

  };


  template<typename T1,typename T2,typename T3>
  struct GetSize<T1,T2,T3,false,false,true>//the third operand has size
  {
    static inline size_t getSize(const T1& t1,const T2& t2,const T3& t3){
      return t3.size();
    }

  };


  template<typename T1,typename T2> inline size_t getSize(const T1& t1,const T2& t2){
    return GetSize<T1,T2,T1>::getSize(t1,t2); //T1 is repeated, so if possible,the first is selected
  }
 
  template<typename T1,typename T2,typename T3> inline size_t getSize(const T1& t1,const T2& t2,const T3& t3){
    return GetSize<T1,T2,T3>::getSize(t1,t2,t3);
  }

  ///--------------------------OPERATIONS----------------------------
 


  //----------------ASSIGN---------

  template <typename A,typename B> inline void assignn(A& a,const B& b,int n){
    for (int i=0;i<n;i++){
      a[i]=b[i];
    }
  }

  template<size_t n,typename A,typename B> inline void assignn(A& a,const B& b){
    for (size_t i=0;i<n;i++){
      a[i]=b[i];
    }
  }

  
  template <typename A> inline void assign(A* a,const A* b,int n){
    memcpy(a,b,sizeof(A)*n);
  }
  

  template<bool b>
  struct Assigner{
    template<typename A,typename B>
    static void assign(A& ,const B& );
  };

   
  template<>
  template<typename A,typename B>
  void Assigner<false>::assign(A& a,const B& b){
    assignn(a,b,getSize(a,b));
  }

  template<>
  template<typename A,typename B>  
  void Assigner<true>::assign(A& a,const B& b){
    size_t n=getSize(a,b);    
    typename B::const_iterator it=b.begin();
    for (size_t i=0;i<n;++i){      
      a[i]=*it;
    }    
  }

  


  template <typename A,typename B> inline void assign(A& a,const B& b){
    Assigner<hasConstIterator<B>::value>::assign(a,b);
  }


  template <typename A, class K, class V, class C,
	     class All > 
  inline void assignValues(A& a, const std::map<K,V,C,All>& m){
    if (a.size()==0){
      a.resize(m.size());
    }
    int i=0;
    for (typename std::map<K,V,C,All>::const_iterator it=m.begin();it!=m.end();++it){
      a[i++]=it->second;
    }
  }


  template <typename A, class K, class V, class C,
	     class All > 
  inline void assignKeys(A& a, const std::map<K,V,C,All>& m){
    if (a.size()==0){
      a.resize(m.size());
    }
    int i=0;
    for (typename std::map<K,V,C,All>::const_iterator it=m.begin();it!=m.end();++it){
      a[i++]=it->first;
    }
  }


  



  //----------------SUM-------------


  // r[i]= a[i]+b[i]


  template <typename R,typename A,typename B> inline void sumn(R& r,const A& a,const B& b,size_t n){
    for (size_t i=0;i<n;i++){
      r[i]=a[i]+b[i];
    }
  }
  
  template <size_t n,typename R,typename A,typename B> inline void sumn(R& r,const A& a,const B& b){
    for (size_t i=0;i<n;i++){
      r[i]=a[i]+b[i];
    }
  }

  template <typename R,typename A,typename B> inline void sum(R& r,const A& a,const B& b){
    sumn(r,a,b,getSize(r,a,b));
  }


  //r[i] = r[i]+a[i]

  template <typename R,typename A> inline void sumn(R& r,const A& a,size_t n){
    for (size_t i=0;i<n;i++){
      r[i]+=a[i];
    }
  }

  template <size_t n,typename R,typename A> inline void sum(R& r,const A& a){
    for (size_t i=0;i<n;i++){
      r[i]+=a[i];
    }
  }

  template <typename R,typename A> inline void sum(R& r,const A& a){
    sumn(r,a,getSize(r,a));
  }


  // return a[0]+...+a[n-1]

  
  template <typename R,typename A> inline R sumn(const A& a,size_t n){
    R r=0;
    for (size_t i=0;i<n;i++){
      r+=a[i];
    }
    return r;
  }

  template <size_t n,typename R,typename A> inline R sumn(const A& a){
    R r=0;
    for (size_t i=0;i<n;i++){
      r+=a[i];
    }
    return r;
  }



    template <size_t n,typename R> inline R sumn(const R* a){
    R r=0;
    for (size_t i=0;i<n;i++){
      r+=a[i];
    }
    return r;
  }

  template<typename A> inline A sumn(A* a,int n){
    return sumn<A,A*>(a,n);
  }
  
  template<typename A> inline A sumn(A* a,size_t n){
    return sumn<A,A*>(a,n);
  }


  template<typename A> inline typename A::value_type sum(const A& a){
    return sumn<typename A::value_type,A>(a,a.size());
  }


  // a[i]= a[i]+s

  template<typename A,typename S> inline void sumScalarn(A& a,S s,size_t n){
    for (size_t i=0;i<n;i++){
      a[i]+=s;
    }
  }

  template<size_t n,typename A,typename S> inline void sumScalarn(A& a,S s){
    for (size_t i=0;i<n;i++){
      a[i]+=s;
    }
  }
  
  template<typename A,typename S> inline void sumScalar(A& a,S s){
    sumScalar(a,s,a.size());
  }


  template<typename A,typename B,typename S> inline void sumScaledn(A& a,B& b, S s,size_t n){
    for (size_t i=0;i<n;i++){
      a[i]+=b[i]*s;
    }
  }

  template<size_t n,typename A,typename B,typename S> inline void sumScaledn(A& a,B& b, S s){
    for (size_t i=0;i<n;i++){
      a[i]+=b[i]*s;
    }
  }

  template<typename A,typename B,typename S> inline void sumScaled(A& a,B& b, S s){
    sumScaledn(a,b,s,getSize(a,b));
  }

  //added by fransiska 2014/12/09
  template <typename R,typename A> inline R sumAbsn(const A& a,size_t n){
    R r=0;
    for (size_t i=0;i<n;i++){
      r+=std::fabs(a[i]);
    }
    return r;
  }

  template<typename A> inline typename A::value_type sumAbs(const A& a){
    return sumAbsn<typename A::value_type,A>(a,a.size());
  }


  //----------------SUB-------------


  // r[i]= a[i]-b[i]


  template <typename R,typename A,typename B> inline void subn(R& r,const A& a,const B& b,size_t n){
    //std::cout << "sub 1" << std::endl;
    for (size_t i=0;i<n;i++){
      r[i]=a[i]-b[i];
    }
  }

  template <size_t n,typename R,typename A,typename B> inline void subn(R& r,const A& a,const B& b){
    //std::cout << "sub 1" << std::endl;
    for (size_t i=0;i<n;i++){
      r[i]=a[i]-b[i];
    }
  }

  
  template <typename R,typename A,typename B> inline void sub(R& r,const A& a,const B& b){
    //std::cout << "sub 2" << std::endl;
    subn(r,a,b,getSize(r,a,b));
  }


  //r[i] = r[i]-a[i]

  template <typename R,typename A> inline void subn(R& r,const A& a,size_t n){
    //std::cout << "sub 3" << std::endl;
    for (size_t i=0;i<n;i++){
      r[i]-=a[i];
    }
  }

  
  template <size_t n,typename R,typename A> inline void subn(R& r,const A& a){
    //std::cout << "sub 3" << std::endl;
    for (size_t i=0;i<n;i++){
      r[i]-=a[i];
    }
  }

  template <typename R,typename A> inline void sub(R& r,const A& a){
    //std::cout << "sub 4" << std::endl;
    subn(r,a,getSize(r,a));
  }


  // a[i]= a[i]-s

  template<typename A,typename S> inline void subScalarn(A& a,S s,size_t n){
    for (size_t i=0;i<n;i++){
      a[i]-=s;
    }
  }
  
  template<size_t n,typename A,typename S> inline void subScalarn(A& a,S s){
    for (size_t i=0;i<n;i++){
      a[i]-=s;
    }
  }


  template<typename A,typename S> inline void subScalar(A& a,S s){
    subScalarn(a,s,a.size());
  }



  //revsub


  template <typename R,typename A> inline void revSubn(R& r,const A& a,size_t n){
    //std::cout << "sub 3" << std::endl;
    for (size_t i=0;i<n;i++){
      r[i]=a[i]-r[i];
    }
  }

  
  template <size_t n,typename R,typename A> inline void revSubn(R& r,const A& a){
    //std::cout << "sub 3" << std::endl;
    for (size_t i=0;i<n;i++){
      r[i]=a[i]-r[i];
    }
  }

  template <typename R,typename A> inline void revSub(R& r,const A& a){
    //std::cout << "sub 4" << std::endl;
    subn(r,a,getSize(r,a));
  }


  //----ABSSUB



  template <typename A,typename B> inline void absSubn(A& a,const B& b,size_t n){
    //std::cout << "sub 1" << std::endl;
    for (size_t i=0;i<n;i++){
      a[i]=tAbs(a[i]-b[i]);
    }
  }

  template <size_t n,typename A,typename B> inline void absSubn(A& a,const B& b){
    //std::cout << "sub 1" << std::endl;
    for (size_t i=0;i<n;i++){
      a[i]=tAbs(a[i]-b[i]);
    }
  }

  
  template <typename R,typename A,typename B> inline void absSub(A& a,const B& b){
    //std::cout << "sub 2" << std::endl;
    absSubn(a,b,getSize(a,b));
  }



  template <typename R,typename A,typename B> inline void absSubn(R& r,const A& a,const B& b,size_t n){
    //std::cout << "sub 1" << std::endl;
    for (size_t i=0;i<n;i++){
      r[i]=tAbs(a[i]-b[i]);
    }
  }


  template <size_t n,typename R,typename A,typename B> inline void absSubn(R& r,const A& a,const B& b){
    //std::cout << "sub 1" << std::endl;
    for (size_t i=0;i<n;i++){
      r[i]=tAbs(a[i]-b[i]);
    }
  }

  
  template <typename R,typename A,typename B> inline void absSub(R& r,const A& a,const B& b){
    //std::cout << "sub 2" << std::endl;
    absSubn(r,a,b,getSize(r,a,b));
  }




  //-------mul


  template <typename R,typename A,typename B> inline void multiplyn(R& r,const A& a,const B& b,size_t n){
    for (size_t i=0;i<n;i++){
      r[i]=a[i]*b[i];
    }
  }
  
  template <size_t n,typename R,typename A,typename B> inline void multiplyn(R& r,const A& a,const B& b){
    for (size_t i=0;i<n;i++){
      r[i]=a[i]*b[i];
    }
  }

  template <typename R,typename A,typename B> inline void multiply(R& r,const A& a,const B& b){
    multiplyn(r,a,b,getSize(r,a,b));
  }




  template <typename R,typename A> inline void multiplyn(R& r,const A& a,size_t n){
    for (size_t i=0;i<n;i++){
      r[i]*=a[i];
    }
  }

  template <size_t n,typename R,typename A> inline void multiplyn(R& r,const A& a){
    for (size_t i=0;i<n;i++){
      r[i]*=a[i];
    }
  }

  template <typename R,typename A> inline void multiplyn(R& r,const A& a){
    multiplyn(r,a,getSize(r,a));
  }




  //------div



  template <typename R,typename A,typename B> inline void dividen(R& r,const A& a,const B& b,size_t n){
    for (size_t i=0;i<n;i++){
      r[i]=a[i]/b[i];
    }
  }
  
  template <size_t n,typename R,typename A,typename B> inline void dividen(R& r,const A& a,const B& b){
    for (size_t i=0;i<n;i++){
      r[i]=a[i]/b[i];
    }
  }

  template <typename R,typename A,typename B> inline void divide(R& r,const A& a,const B& b){
    dividen(r,a,b,getSize(r,a,b));
  }


  //r[i] = r[i]+a[i]

  template <typename R,typename A> inline void dividen(R& r,const A& a,size_t n){
    for (size_t i=0;i<n;i++){
      r[i]/=a[i];
    }
  }

  template <size_t n,typename R,typename A> inline void dividen(R& r,const A& a){
    for (size_t i=0;i<n;i++){
      r[i]/=a[i];
    }
  }

  template <typename R,typename A> inline void divide(R& r,const A& a){
    dividen(r,a,getSize(r,a));
  }


  // return a[0]+...+a[n-1]

  
  

  


 //----------------MUL DIV(fb20110627)-------------
  template <typename A, typename S> inline void mulScalarn(A &a, const S s, size_t n) {
    for (size_t i=0;i<n;i++){
      a[i] *= s;
    }
  }

  template <size_t n,typename A, typename S> inline void mulScalar(A &a, const S s) {
    for (size_t i=0;i<n;i++){
      a[i] *= s;
    }
  }

  template <typename A, typename S> inline void mulScalar(A &a, const S s) {
    mulScalarn(a,s,a.size());
  }


  template <typename R, typename A, typename S> inline void mulScalarn(R& r,const A &a, const S s, size_t n) {
    for (size_t i=0;i<n;i++){
      r[i] =a[i]* s;
    }
  }


  template <size_t n,typename R,typename A, typename S> inline void mulScalarn(R& r,const A &a, const S s) {
    for (size_t i=0;i<n;i++){
      r[i] =a[i]* s;
    }
  }

 
  template <typename R,typename A, typename S> inline void mulScalar(R& r,const A &a, const S s) {
    mulScalarn(r,a,s,getSize(r,a));
  }

  /*
  //add from matrixUtil by fb 20110613

  template <typename A, typename B, typename C> inline void mul(A &r,const B &m1,const C &m2, int n){
    for (int i=0;i<n;i++){
      for (int j=0;j<n;j++){
	r[i*n+j]=0;
	for (int k=0;k<n;k++){
	  r[i*n+j]+=m1[i*n+k]*m2[k*n+j];
	}
      }
    }
  }
  */


  //added by fb 20110614

  template <typename R,typename A, typename S> inline void divScalarn(R& r,const A &a, S s, size_t n) {
    for (size_t i=0;i<n;i++){
      r[i] = a[i] / s;
    }
  }

  template <size_t n,typename R,typename A, typename S> inline void divScalarn(R& r,const A &a, S s) {
    for (size_t i=0;i<n;i++){
      r[i] = a[i] / s;
    }
  }

  
template <typename R,typename A, typename S> inline void divScalar(R& r,const A &a, S s) {
  divScalarn(r,a,s,getSize(r,a));
}


  template <typename A, typename S> inline void divScalarn(A &a, const S s, size_t n) {
    for (size_t i=0;i<n;i++){
      a[i] = a[i] / s;
    }
  }

  template <size_t n,typename A, typename S> inline void divScalarn(A &a, const S s) {
    for (size_t i=0;i<n;i++){
      a[i] = a[i] / s;
    }
  }

  template <typename A, typename S> inline void divScalar(A &a, const S s) {
    divScalarn(a,s,a.size());
  }

  //---------------end------------------


  ///----square
  
  template<typename A> inline void squaren(A& a,size_t n){
    for (size_t i=0;i<n;i++){
      a[i]=a[i]*a[i];
    }
  }

  template<size_t n,typename A> inline void squaren(A& a){
    for (size_t i=0;i<n;i++){
      a[i]=a[i]*a[i];
    }
  }

   template<typename A>  inline void square(A& a){
     squaren(a,a.size());
   }



  template<typename R,typename A> inline void squaren(R& r,const A& a,size_t n){
    for (size_t i=0;i<n;i++){
      r[i]=a[i]*a[i];
    }
  }

  template<size_t n,typename R,typename A> inline void squaren(R& r,const A& a){
    for (size_t i=0;i<n;i++){
      r[i]=a[i]*a[i];
    }
  }

  template<typename R,typename A>  inline void square(R& r,const A& a){
    squaren(r,a,getSize(r,a));
  }

  

  ///----sqrt (with R not to override stdlib's sqrt)

    template<typename A> inline void sqRtn(A& a,size_t n){
    for (size_t i=0;i<n;i++){
      a[i]=sqrt(a[i]);
    }
  }

  template<size_t n,typename A> inline void sqRtn(A& a){
    for (size_t i=0;i<n;i++){
      a[i]=sqrt(a[i]);
    }
  }

   template<typename A>  inline void sqRt(A& a){
     sqrtn(a,a.size());
   }



  template<typename R,typename A> inline void sqRtn(R& r,const A& a,size_t n){
    for (size_t i=0;i<n;i++){
      r[i]=sqrt(a[i]);
    }
  }

  template<size_t n,typename R,typename A> inline void sqRtn(R& r,const A& a){
    for (size_t i=0;i<n;i++){
      r[i]=sqrt(a[i]);
    }
  }

  template<typename R,typename A>  inline void sRrt(R& r,const A& a){
    sqrtn(r,a,getSize(r,a));
  }

  //-----mean----

   template<typename A> inline double meann(A& a,size_t n){
     double d=0;
    for (size_t i=0;i<n;i++){
      d+=a[i];
    }
    return d/n;
  }
  
  template<size_t n,typename A> inline double meann(A& a){
    double d=0;
    for (size_t i=0;i<n;i++){
      d+=a[i];
    }
    return d/n;
  }

  template<typename A> inline double mean(A& a){
    return meann(a,a.size());
  }


  //----------------NORMALIZE-------------

  template<typename A> inline double normn(const A& a,size_t n){
    double d=0;
    for (size_t i=0;i<n;i++){
      d+=a[i]*a[i];
    }
    return sqrt(d);
  }
  
  template<size_t n,typename A> inline double normn(const A& a){
    double d=0;
    for (size_t i=0;i<n;i++){
      d+=a[i]*a[i];
    }
    return sqrt(d);
  }


  template<size_t n,typename A> inline double normn(const A* a){
    double d=0;
    for (size_t i=0;i<n;i++){
      d+=a[i]*a[i];
    }
    return sqrt(d);
  }

  template<typename A> inline double norm(const A& a){
    return normn(a,a.size());
  }



  template <typename R,typename T> inline R normn(const T&a, int n) {
    R r = 0;
    for(int i=0;i<n;i++)
      r+=a[i]*a[i];
    return sqrt(r);
  }
  /*  template <typename A> inline typename A::value_type norm(const A &a) {
    return norm<typename A::value_type,A>(a,a.size());
    }*/



  template<typename A> inline void normalizen(A& a,size_t n){
    double no=normn(a,n);
    for (size_t i=0;i<n;i++){
      a[i]/=no;
    }
  }

  template<size_t n,typename A> inline void normalizen(A& a){
    double no=normn(a,n);
    for (size_t i=0;i<n;i++){
      a[i]/=no;
    }
  }


  template<size_t n,typename A> inline void normalizen(A* a){
    double no=normn(a,n);
    for (size_t i=0;i<n;i++){
      a[i]/=no;
    }
  }

  template<typename A> inline void normalize(A& a){
    normalizen(a,a.size());
  }
  



    template<typename A> inline double squaredNormn(A& a,size_t n){
    double d=0;
    for (size_t i=0;i<n;i++){
      d+=a[i]*a[i];
    }
    return d;
  }
  
  template<size_t n,typename A> inline double squaredNormn(A& a){
    double d=0;
    for (size_t i=0;i<n;i++){
      d+=a[i]*a[i];
    }
    return d;
  }

  template<typename A> inline double squaredNorm(A& a){
    return squaredNormn(a,a.size());
  }



  //----------------NEGATE-------------


  template<typename A> inline void negaten(A& a,size_t n){
    for (size_t i=0;i<n;i++){
      a[i]=-a[i];
    }
  }

  template<size_t n,typename A> inline void negaten(A& a){
    for (size_t i=0;i<n;i++){
      a[i]=-a[i];
    }
  }


  template<typename A> inline void negate(A& a){
    negaten(a,a.size());
  }
  
  //----------------EUCL DISTANCE-------------

  template <typename A,typename B> inline double euclideanDistancen(const A &a,const B &b,int n){
    double r=0;
    for (int i=0;i<n;i++){
      r+=(a[i]-b[i])*(a[i]-b[i]);      
    }
    return sqrt(r);
  }


  template <size_t n,typename A,typename B> inline double euclideanDistancen(const A &a,const B &b){
    double r=0;
    for (size_t i=0;i<n;i++){
      r+=(a[i]-b[i])*(a[i]-b[i]);      
    }
    return sqrt(r);
  }

  template <typename A,typename B> inline double euclideanDistance(const A &a,const B &b){    
    return euclideanDistancen(a,b,getSize(a,b));
  }


 
  template <typename A,typename B> inline double squaredEuclideanDistancen(const A &a,const B &b,size_t n){
    double r=0;
    //std::cout<<"n="<<n<<std::endl;
    for (size_t i=0;i<n;i++){
      //std::cout<<"a="<<a[i]<<"b="<<b[i]<<std::endl;
      r+=(a[i]-b[i])*(a[i]-b[i]);      
      //std::cout<<"r="<<r<<std::endl;
    }
    return r;
  }

  


  template <size_t n,typename A,typename B> inline double squaredEuclideanDistancen(const A &a,const B &b){
    double r=0;
    //std::cout<<"n="<<n<<std::endl;
    for (int i=0;i<n;i++){
      //std::cout<<"a="<<a[i]<<"b="<<b[i]<<std::endl;
      r+=(a[i]-b[i])*(a[i]-b[i]);      
      //std::cout<<"r="<<r<<std::endl;
    }
    return r;
  }


  template <typename A,typename B> inline double squaredEuclideanDistance(const A &a,const B &b){
    return squaredEuclideanDistancen(a,b,getSize(a,b));
  }


  //--------------DOT---------


  template <typename A,typename B> inline double dotn(const A &a,const B &b,size_t n){
    double r=0;
    for (size_t i=0;i<n;i++){
      r+=a[i]*b[i];
    }
    return r;
  }

  template <size_t n,typename A,typename B> inline double dotn(const A &a,const B &b){
    double r=0;
    for (size_t i=0;i<n;i++){
      r+=a[i]*b[i];
    }
    return r;
  }


  template <typename A,typename B> inline double dot(const A &a,const B &b){
    return dotn(a,b,getSize(a,b));
  }


  //--------------ZERO----------------


  template<typename A> inline void zeron(A* a,size_t n){
    //std::cout<<"memsetting array"<<std::endl;
    memset(a,0,sizeof(A)*n);
  }

  template<size_t n,typename A> inline void zeron(A* a){
    //std::cout<<"memsetting array"<<std::endl;
    memset(a,0,sizeof(A)*n);
   }

  
  template<typename A> inline void zero(A &a){
    //std::cout<<"element by element"<<std::endl;
    for (size_t i=0;i<a.size();i++){
      a[i]=0;
    }
  }


  //-------------SETCONSECUTIVE-----------


template<typename A> inline void setConsecutiven(A& a,size_t n){
  for (size_t i=0;i<n;i++){
    a[i]=i;
  }
}

  
  template<typename A> inline void setConsecutive(A &a){
    setConsecutiven(a,a.size());
  }


  //--------------ISZERO-------------


  template<typename A> inline bool isZeron(const A& a,size_t n){
    for (size_t i=0;i<n;i++){
      if (a[i]!=0) return false;    
    }
    return true;
  }

  template<size_t n,typename A> inline bool isZeron(const A& a){
    for (int i=0;i<n;i++){
      if (a[i]!=0) return false;    
    }
    return true;
  }

  template<typename A> inline bool isZero(const A& a){
    return isZeron(a,(int)a.size());
  }


  //------------SETALL------------

  template<typename A,typename T> inline void setAlln(A &a,const T&t,size_t n){
    //std::cout<<"element by element"<<std::endl;
    for (size_t i=0;i<n;i++){
      a[i]=t;
    }
  }

  template<size_t n,typename A,typename T> inline void setAlln(A &a,const T&t){
    //std::cout<<"element by element"<<std::endl;
    for (size_t i=0;i<n;i++){
      a[i]=t;
    }
  }


  template<typename A,typename T> inline void setAll(A &a,const T&t){
    //std::cout<<"element by element"<<std::endl;
    for (int i=0;i<(int)a.size();i++){
      a[i]=t;
    }
  }




  //-------------ELEMWISE

  template<typename T,typename U>  inline void elemWiseAndn(T& t,const U& u,size_t n){
    for (size_t i=0;i<n;i++){
      t[i]=t[i]&&u[i];
    }
  }
  
  template<size_t n,typename T,typename U>  inline void elemWiseAnd(T& t,const U& u){
    for (size_t i=0;i<n;i++){
      t[i]=t[i]&&u[i];
    }
  }
  
  template<typename T,typename U>  inline void elemWiseAnd(T& t,const U& u){
    elemWiseAndn(t,u,getSize(t,u));
  }






  //-------------CONTAINS


  template<typename Set,typename Element> inline bool contains(Set &s,const Element& e){
    for (size_t i=0;i<s.size();i++){
      if (s[i]==e) return true;
    }
    return false;
  }

  template<typename Set,typename Element> inline bool containsOtherThan(Set &s,const Element& e){
    for (size_t i=0;i<s.size();i++){
      if (s[i]!=e) return true;
    }
    return false;
  }


  template<typename ASet,typename BSet> inline bool intersects(ASet &a,BSet& b){
    for (size_t i=0;i<a.size();++i){
      for (size_t j=0;j<b.size();++j)
	if (a[i]==b[j]) return true;
    }
    return false;
  }

  
  template<typename RSet, typename ASet,typename BSet> inline void setDifference(RSet& r,ASet& a,BSet& b){
    r.clear();
    for (size_t i=0;i<a.size();++i){
      if (!contains(b,a[i])){
	r.push_back(a[i]);
      }
    }
  }

  
  template<typename ASet,typename BSet> inline bool isIncluded(ASet& a,BSet& b){
    for (size_t i=0;i<a.size();++i){
      if (!contains(b,a[i])) return false;
    }
    return true;
  }



  template<typename T,typename M> void vlimitScalarn(T& t,M minv,M maxv,size_t n){
    for (size_t i=0;i<n;i++){
      if (t[i]<minv){
	t[i]=minv;
      }else if (t[i]>maxv){
	t[i]=maxv;
      }
    }//for    
  }



  template<typename T,typename M> void vlimitScalar(T& t,M minv,M maxv){
    vlimitScalarn(t,minv,maxv,t.size());
  }

  //-----------------FOREACH


 template <typename V>
    void forEach(V &v,void (V::value_type::*m)()){
    for (size_t i=0;i<v.size();i++){
      (v[i].*m)();
    }
  }

 template <typename T,typename A>
   void forEach(std::list<T,A> &v,void (std::list<T,A>::value_type::*m)()){
   for (typename std::list<T,A>::iterator i=v.begin();i!=v.end();i++){
     ((*i).*m)();
   }
 }


 template <typename V>
    void forEachPassIdx(V &v,void (V::value_type::*m)(int)){
    for (size_t i=0;i<v.size();i++){
      (v[i].*m)((int)i);
    }
  }

 template <typename T,typename A>
   void forEachPassIdx(std::list<T,A> &v,void (std::list<T,A>::value_type::*m)(int)){
   int c=0;
   for (typename std::list<T,A>::iterator i=v.begin();i!=v.end();i++){
     ((*i).*m)(c++);
    }
  }


  template <typename V,typename P>
    void forEach(V &v,void (V::value_type::*m)(P),P p){
    for (size_t i=0;i<v.size();i++){
      (v[i].*m)(p);
    }
  }

  template <typename T,typename A,typename P>
    void forEach(std::list<T,A> &v,void (std::list<T,A>::value_type::*m)(P),P p){
    for (typename std::list<T,A>::iterator i=v.begin();i!=v.end();i++){
      ((*i).*m)(p);
   }
 }



  template <typename C,typename V>
    void forEach(C* c,void (C::*m)(typename V::value_type&),V &v){
    for (size_t i=0;i<v.size();i++){
      (c->*m)(v[i]);
    }
  }
  
  template <typename V1,typename V2>
    void forEachCombination(V1& v1,V2&v2,void (*f)(typename V1::value_type&,typename V2::value_type&) ){
    for (typename V1::iterator i=v1.begin();i!=v1.end();i++){
      for (typename V2::iterator j=v2.begin();j!=v2.end();j++){
	(*f)( (*i),(*j) );
      }
    }
  }


  

  template<typename Super>
  class MathObject:public Super{
  public:
    
    template <typename T> void set(const T& t){
      *static_cast<Super*>(this)=t;
    }

    template <typename T> void sum(const T& t){
      helium::sum(*this,t);
    }
    template <typename A,typename B> void sum(const A &a,const B& b){
      helium::sum(*this,a,b);
    }

    template <typename S> void sumScalar(S s){
      helium::sumScalar(*this,s);
    }

    template <typename T> void sub(const T& t){
      //std::cout << "sub 5" << std::endl;
      helium::sub(*this,t);
    }
    
    template <typename T> void revsub(const T& t){
      //std::cout << "sub 5" << std::endl;
      helium::sub(t,*this);
    }

    
    template <typename A,typename B> void sub(const A &a,const B& b){
      //std::cout << "sub 6" << std::endl;
      helium::sub(*this,a,b);
    }


    template <typename S> void subScalar(S s){
      helium::subScalar(*this,s);
    }
    
    double norm(){
      return helium::norm(*this);
    }

    template<typename T> double dot(const T& t){
      return helium::dot(*this,t);
    }
    
    void normalize(){
      helium::normalize(*this);
    }
    
    
    void negate(){
      helium::negate(*this);
    }
    

    void zero(){
      helium::zero(*this);
    }

    template<typename T> void mulScalar(const T& t){
      helium::mulScalar(*this,t);
    }

     template<typename T> void divScalar(const T& t){
      helium::divScalar(*this,t);
    }

    bool isZero(){
      return helium::isZero(*this);
    }

    template<typename T> 
    double squaredEuclideanDistance(const T &a){
      return helium::squaredEuclideanDistance(*this,a);
    }
    
  }; //MathObject
  
  template<typename Super>
  struct hasSize<MathObject<Super> >{
    static const bool value=true;
  };

  template<typename A,typename B> inline bool equals(const A& a,const B& b,int n){
    for (int i=0;i<n;i++){
      if (a[i]!=b[i]) return false;
    }   
    return true;
  }

  template<typename A,typename B> inline bool equals(const A& a,const B& b){
    if (a.size()!=b.size()) return false;
    return equals(a,b,(int)a.size());
  }

  template<size_t n,typename A,typename B> inline bool equalsn(const A& a,const B& b){
    for (size_t i=0;i<n;i++){
      if (a[i]!=b[i]) return false;
    }   
    return true;
  }



  template<size_t n,typename A,typename B> inline bool differsn(const A& a,const B& b){
    for (size_t i=0;i<n;i++){
      if (a[i]!=b[i]) return true;
    }   
    return false;
  }

  template<typename A,typename B> inline bool differsn(const A& a,const B& b,size_t n){
    for (size_t i=0;i<n;i++){
      if (a[i]!=b[i]) return true;
    }   
    return false;
  }


  template<typename A,typename B> inline bool differs(const A& a,const B& b){
    return differsn(a,b,getSize(a,b));
  }


  template<typename A,typename B> inline bool differsc(const A& a,const B& b){
    if (a.size()!=b.size()) return true;
    return differsn(a,b,a.size());
  }





  //added by fb 20110614
  template <typename T> inline T limit(T minv,T maxv,T x){
    x=(std::max)(x,minv);
    x=(std::min)(x,maxv);
    return x;
  }
  //end of add
  
  template <typename T> bool isInRange(T minv,T maxv,T x){
    return (x>=minv&&x<=maxv);
  }

  //template return infinitynorm
  template <typename R,typename T> inline R infinityNorm(const T&a, int n) {
    R r = 0;
    for(int i=0;i<n;i++)
      r=(std::max)(r,fabs(a[i]));
    return r;
  }
  template <typename A> inline typename A::value_type infinityNorm(const A &a){
    return infinityNorm<typename A::value_type,A>(a,a.size());
  }



 
  template <typename A, typename B> inline double infinityNormDistancen(const A &a,const B &b,size_t n){
    double d=0;
    for (size_t i=0;i<n;i++)
      d=(std::max)(d,fabs(a[i]-b[i]));
    return d;
  }

  template <size_t n,typename A, typename B> inline double infinityNormDistance(const A &a,const B &b){
    double d=0;
    for (size_t i=0;i<n;i++)
      d=(std::max)(d,fabs(a[i]-b[i]));
    return d;
  }

  template <typename A, typename B> inline double infinityNormDistance(const A &a,const B &b){
    return infinityNormDistancen(a,b,getSize(a,b));
  }


  template <typename A, typename B> inline bool lessThann(const A*a,const B*b,size_t n){
    for (size_t i=0;i<n;i++){
      if (a[i]<b[i]) return true;
      if (a[i]>b[i]) return false;
      //we continue the check when they're equal
    }
    return false;
  }


    template <typename A, typename B> inline bool lessThann(const A&a,const B&b,size_t n){
    for (size_t i=0;i<n;i++){
      if (a[i]<b[i]) return true;
      if (a[i]>b[i]) return false;
      //we continue the check when they're equal
    }
    return false;
  }


  template <size_t n,typename A, typename B> inline bool lessThan(const A*a,const B*b){
    for (size_t i=0;i<n;i++){
      if (a[i]<b[i]) return true;
      if (a[i]>b[i]) return false;
    }
    return false;
  }


  template <typename A, typename B> inline  bool lessThan(const A &a,const B &b){
    return lessThan(a,b,getSize(a,b));
  }




  template <typename A, typename B> inline int cmpn(const A*a,const B*b,size_t n){
    for (size_t i=0;i<n;i++){
      if (a[i]<b[i]) return -1;
      if (a[i]>b[i]) return +1;
      //we continue the check when they're equal
    }
    return 0;
  }


  template <typename A, typename B> inline int cmpn(const A&a,const B&b,size_t n){
    for (size_t i=0;i<n;i++){
      if (a[i]<b[i]) return -1;
      if (a[i]>b[i]) return +1;
      //we continue the check when they're equal
    }
    return 0;
  }


  template <size_t n,typename A, typename B> inline int cmp(const A&a,const B&b){
    for (size_t i=0;i<n;i++){
      if (a[i]<b[i]) return -1;
      if (a[i]>b[i]) return +1;
      //we continue the check when they're equal
    }
    return 0;
  }


  template <typename A, typename B> inline int cmp(const A&a,const B&b){
    return cmpn(a,b,getSize(a,b));
  }


  /*euclidean distance, aded by fb 20110611

  template <typename A,typename B> inline double euclideanDistance(const A &a, const B &b) {
    return euclideanDistancen(a,b,a.size());
  }

  //end of euclidean*/

  //accumulateminmax
  template <typename A, typename B> inline void accumulateMinn(A &a, const B &b,int n) {
    for (int i=0;i<n;i++){
      a[i]=(std::min)(a[i],b[i]);
    } 
  }

   template <typename A, typename B> inline void accumulateMaxn(A &a, const B &b,int n) {
     for (int i=0;i<n;i++){
       a[i]=(std::max)(a[i],b[i]);
     } 
   }

    template <typename A, typename B> inline void accumulateMin(const A &a, const B &b) {
    accumulateMinn(a,b,a.size());
  }
  
  template <typename A, typename B> inline void accumulateMax(const A &a, const B &b) {
    accumulateMaxn(a,b,a.size());
  }





  template <typename A> inline A getMinn(const A* a, int n) {
    A r=(std::numeric_limits<A>::max)();
    for (int i=0;i<n;i++){
      r=(std::min)(a[i],r);
    } 
    return r;
  }

   template <typename A> inline A getMaxn(const A* a, int n) {
     A r=(std::numeric_limits<A>::min)();
     for (int i=0;i<n;i++){
       r=(std::max)(a[i],r);
     } 
     return r;
   }
 

  template <typename A> inline typename A::value_type getMin(const A& a) {
    typename A::value_type r=(std::numeric_limits<typename A::value_type>::max)();
    for (size_t i=0;i<a.size();i++){
      r=(std::min)(a[i],r);
    } 
    return r;
  }

  template <typename A> inline typename A::value_type getMax(const A& a) {
    typename A::value_type r=(std::numeric_limits<typename A::value_type>::min)();
    for (size_t i=0;i<a.size();i++){
       r=(std::max)(a[i],r);
     } 
     return r;
   }
 

  /** added by fransiska 2015.01.26
   *  return the id which value is minimum
   */
  template <typename A> inline size_t getMinIdn(const A* a, size_t n) {
    A r=(std::numeric_limits<A>::max)();
    size_t minid = 0;
    for (size_t i=0;i<n;i++){
      if(a[i] < r) {
	r = a[i];
	minid = i;
      }
    } 
    return minid;
  }
  template <typename A> inline size_t getMinId(const A& a) {
    return getMinIdn(a.data,a.size());
  }
  template <typename A> inline size_t getMaxIdn(const A* a, size_t n) {
    A r=(std::numeric_limits<A>::min)();
    size_t maxid = 0;
    for (size_t i=0;i<n;i++){
      if(a[i] > r) {
	r = a[i];
	maxid = i;
      }
    } 
    return maxid;
  }
  template <typename A> inline size_t getMaxId(const A& a) {
    return getMaxIdn(a.data,a.size());
  }
 



  //end of accumulateminmax

  //---------------end------------------




}//namespace helium

#endif
