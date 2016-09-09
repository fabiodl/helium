/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.01.31
 * \version 0.2
 * Release_flags s
 */


#ifndef VECTORCONTAINER
#define VECTORCONTAINER
#include <vector>
#include <cstring>
#include <stdlib.h> //for malloc
#include <helium/core/exception.h>
#include <helium/core/isBaseOf.h>
#include <helium/core/referenceType.h>
#include <helium/core/void.h>



//TODO:: make resize use realloc

namespace helium{
 

  namespace array{
    enum ArrayType{RESIZABLE=0,RESETTABLE=-1}; //resettable is for objects that do not have an assignment operator, thus resize can only consist in reset (throw away and reconstruct)


    class STACK{
    };

    class HEAP{      
    };
    
  
    class MALLOC{
    };

    class LINK{
    };
    

    template<int N>
    struct defAllocator{
      typedef STACK value;
    };

    template<>
    struct defAllocator<RESIZABLE>{
      typedef HEAP value;
    };

    template<>
    struct defAllocator<RESETTABLE>{
      typedef HEAP value;
    };

    template<typename AT> class StaticAllocator{
    };

    template<typename AT> class InternalAllocator{
    };


  };

  

  template <typename T,int N=array::RESIZABLE,typename Allocator=typename array::defAllocator<N>::value > class Array;


  /*template<typename T,int N,typename A>
  struct isIndexable<Array<T,N,A> >{
    static const bool value=true;
  };

  template<typename T,typename A>
  struct isIndexable<std::vector<T,A> >{
    static const bool value=true;
    };*/


  template<typename T> 
  struct isDirectlyCopiable{
    static const bool value=false;
  };

  template<> 
  struct isDirectlyCopiable<unsigned char>{
     static const bool value=true;
  };

  template<> 
  struct isDirectlyCopiable<signed char>{
     static const bool value=true;
  };

  template<> 
  struct isDirectlyCopiable<unsigned int>{
     static const bool value=true;
  };

template<> 
  struct isDirectlyCopiable<signed int>{
     static const bool value=true;
};

  template<> 
  struct isDirectlyCopiable<signed long long int>{
     static const bool value=true;
  };

  template<> 
  struct isDirectlyCopiable<unsigned long long int>{
     static const bool value=true;
  };

  template<> 
  struct isDirectlyCopiable<float>{
     static const bool value=true;
  };

  template<> 
  struct isDirectlyCopiable<double>{
     static const bool value=true;
  };


  template<typename T,bool prim=isDirectlyCopiable<T>::value > struct Copier;

  template<typename T> struct Copier<T,true>{
    static inline void copyMem(T*dst,const T* src,int n){
      if (n){
	memcpy(dst,src,sizeof(T)*n);
      }
    }    
  };


  template<typename T> struct Copier<T,false>{
    static inline void copyMem(T*dst,const T* src,int n){
      for (int i=0;i<n;i++){
	dst[i]=src[i];
      }
    }    
  };


  template <typename T> inline void copyMem(T* dst,const T* src,int n){
    Copier<T>::copyMem(dst,src,n);

  }



  template <typename T,typename S> inline void copyMem(T* dst,const S* src,int n){
    for (int i=0;i<n;i++){
      dst[i]=src[i];
    }
    //memcpy(dst,src,sizeof(T)*n);
  }
  



  /*template <typename T> inline void copyMem(T* dst,const T* src,int n){
    for (int i=0;i<n;i++){
      dst[i]=src[i];
    }
    memcpy(dst,src,sizeof(T)*n);
    }*/


  template <typename A>
  struct AllocatorAdapter{
    typedef A& value;
  };
  
  template <typename A>
  struct AllocatorAdapter<array::InternalAllocator<A> >{
    typedef A value;
  };



  template <typename T,int N,typename Allocator> class Array{   
  public:
  typedef T value_type;


    typename AllocatorAdapter<Allocator>::value a;
    T* data;

    operator T*(){
      return data;
    }
    
    operator const T*() const{
      return data;
    }

    template<typename AP>
    Array(AP& pa):a(pa),data(a.getNew((size_t)N)){
    }

    template<typename AP>
    Array(const AP& pa):a(pa),data(a.getNew((size_t)N)){
    }

    ~Array(){
      a.destroy(data,(size_t)N);
    }

    Array(const Array& b):a(b.pa),data(a.getNew((size_t)N)){
      copyMem(data,b.data,N);
    }


     inline size_t size()const {
       return N;
     }

    
    inline T& operator[](int x){
      return data[x];
    }

    inline const T& operator[](int x) const{
      return data[x];
    }

    inline T& back(){
      return data[N-1];
    }

    inline const T& back() const{
      return data[N-1];
    }
     
     void get(T* d){
       copyMem(d,data,N);
     }
    
    template<typename T2,int N2,typename AT2>
    Array& operator =(const Array<T2,N2,AT2>& b){
       copyMem(data,b.data,N);
       return *this;
     }

    Array& operator =(Array& b){
       copyMem(data,b.data,N);
       return *this;
     }

     Array& operator =(const T* b){
       copyMem(data,b,N);
       return *this;
     }
     
     template <typename B> Array& operator =(const B &b){
        for (int i=0;i<N;i++){
	 data[i]=b[i];
       }
       return *this;
     }

     template <typename B>
     bool operator!=(const B& b) const{
       return !equals(*this,b);
     }

    template <typename B>
    bool operator==(const B& b) const{
       return equals(*this,b);
    }


  };






  template <typename T,int N,typename AT> class Array<T,N,array::StaticAllocator<AT> >{   
  public:
    typedef T value_type;
    T* data;


    operator T*(){
      return data;
    }
    
    operator const T*() const{
      return data;
    }

    Array():data(AT::getNew((size_t)N)){
    }

    ~Array(){
      AT::destroy(data,(size_t)N);
    }

     inline size_t size()const {
       return N;
     }
     
     inline T& operator[](int x){
      return data[x];
    }

    inline const T& operator[](int x) const{
      return data[x];
    }

    inline T& back(){
      return data[N-1];
    }

    inline const T& back() const{
      return data[N-1];
    }

     
     void get(T* d){
       copyMem(d,data,sizeof(T)*N);
     }


    Array& operator =(Array& b){
       copyMem(data,b.data,N);
       return *this;
     }

    template<typename T2,int N2,typename AT2>
    Array& operator =(const Array<T2,N2,AT2>& b){
       copyMem(data,b.data,N);
       return *this;
     }


     Array& operator =(const T* b){
       copyMem(data,b,N);
       return *this;
     }
     
     template <typename B> Array& operator =(const B &b){
       for (int i=0;i<N;i++){
	 data[i]=b[i];
       }
       return *this;
     }


     template <typename B>
     bool operator!=(const B& b) const{
       return !equals(*this,b);
     }

    template <typename B>
     bool operator==(const B& b) const{
       return equals(*this,b);
     }

    /*void resize(size_t n){
      if (n!=N){
	throw exc::InvalidOperation("resizing an array on Stack");
      }
      }*/

  };




  template <typename T,int N> class Array<T,N,array::STACK>{   
  public:
  typedef T value_type;
    T data[N];

    inline size_t size()const {
      return N;
    }
    
    inline T& operator[](int x){
      return data[x];
    }

    inline const T& operator[](int x) const{
      return data[x];
    }

    inline T& back(){
      return data[N-1];
    }
    
    inline const T& back() const{
      return data[N-1];
    }
     
     void get(T* d){
      copyMem(d,data,N);
     }


    template<typename T2,int N2,typename AT2>
    Array& operator =(const Array<T2,N2,AT2>& b){
       copyMem(data,b.data,N);
       return *this;
     }


     Array<T,N>& operator =(const T* b){
       copyMem(data,b,N);
	   return *this;
     }
     
     template <typename B> Array<T,N>& operator =(const B &b){
       for (int i=0;i<N;i++){
	 data[i]=b[i];
       }
       return *this;
     }

    /*void resize(size_t n){
      if (n!=N){
	throw exc::InvalidOperation("resizing an array on Stack");
      }
      }*/
     template <typename B>
     bool operator!=(const B& b) const{
       return !equals(*this,b);
     }

    template <typename B>
     bool operator==(const B& b) const{
       return equals(*this,b);
     }


    operator T*(){
      return data;
    }
    
    operator const T*() const{
      return data;
    }

    Array(){}
    Array(const Void*){}

  };



  template <typename T,int N> class Array<T,N,array::HEAP>{   
  public:
  typedef T value_type;
    T* data;

    operator T*(){
      return data;
    }
    
    operator const T*() const{
      return data;
    }

    Array():data(new T[N]){
    }

    Array(const Array& b){
      data=new T[N];
      copyMem(data,b.data,N);
    }


    ~Array(){
      delete[] data;
    }
    
     inline size_t size()const {
       return N;
     }
     
    inline T& operator[](int x){
      return data[x];
    }

    inline const T& operator[](int x) const{
      return data[x];
    }

    inline T& back(){
      return data[N-1];
    }

    inline const T& back() const{
      return data[N-1];
    }
     
     void get(T* d){
      copyMem(d,data,N);
     }
    
    template<typename T2,int N2,typename AT2>
    Array& operator =(const Array<T2,N2,AT2>& b){
       copyMem(data,b.data,N);
       return *this;
     }


     Array& operator =(const T* b){
       copyMem(data,b,N);
       return *this;
     }

    Array& operator =(Array& b){
       copyMem(data,b.data,N);
       return *this;
     }

     
     template <typename B> Array& operator =(const B &b){
       for (int i=0;i<N;i++){
	 data[i]=b[i];
       }
       return *this;
     }

     template <typename B>
     bool operator!=(const B& b) const{
       return !equals(*this,b);
     }
    
    template <typename B>
     bool operator==(const B& b) const{
       return equals(*this,b);
     }

  };








  /*
  template <typename T> class Array<T,array::HEAP>{
  protected:
    size_t length;
  public:
      typedef T value_type;
    T* data;
    
    inline size_t size()const {
      return length;
    }
    
    inline T& operator[](int x) const{
      return data[x];
    }


  Array(int n):length(n),data(new T[length]){                     
    }

  Array(size_t n):length(n),data(new T[length]){                
    }

  Array(const Array<T,array::HEAP> &s):length(s.length),data(new T[length]){
    }  
  
    virtual ~Array(){
      std::cout<<"deleting"<<data<<std::endl;
      delete[] data;
    }

    void get(T* d){
       memcpy(d,data,sizeof(T)*length);
    }

    template<int N> Array<T,array::HEAP>& operator =(const Array<T,N>& b){
       memcpy(data,b.data,sizeof(T)*length);
       return *this;
     }
    
    Array<T,array::HEAP>& operator =(T* b){
       memcpy(data,b,sizeof(T)*length);
       return *this;
     }
     
    template <typename B> Array<T,array::HEAP>& operator =(B &b){
       for (int i=0;i<length;i++){
	 data[i]=b[i];
       }
       return *this;
     }


  }; //Array<T,N>

  */



  /*   template<typename Param>
    Array(int n,Param &p){
      T* space=(T*)malloc(sizeof(T)*n);
      for (int i=0;i<n;i++){
	new (space+i) T(p,i);
      }
    }
  */


  template <typename T,typename Allocator> class Array<T,array::RESIZABLE,Allocator>{
  protected:
    typename AllocatorAdapter<Allocator>::value a;
    size_t length;
    
  public:
    typedef T value_type;
  

    T* data;
    
    inline size_t size()const {
      return length;
    }
    
    inline T& operator[](int x) const{
      return data[x];
    }

    inline T& back(){
      return data[int(length)-1];
    }

    inline const T& back() const{
      return data[int(length)-1];
    }

    operator T*(){
      return data;
    }
    
    operator const T*() const{
      return data;
    }


    template <typename AP>
    Array(const AP& pa,size_t n):a(pa),length(n),data(n?a.getNew(n):NULL){  

    }

    template <typename AP>
    Array(AP& pa,size_t n):a(pa),length(n),data(n?a.getNew(n):NULL){  

    }

    template<typename SizeT> //template for preventing no param problem for allocators with param
    Array(SizeT n):length(n),data(n?a.getNew(n):NULL){  

    }



    
    Array(const Array&s):a(s.a),length(s.length),data(length?a.getNew(length):NULL){      
      //std::cout<<"--copy constructor"<<std::endl;
      copyMem(data,s.data,length);
    }  
    
   
    virtual ~Array(){
      //std::cout<<"deleting"<<data<<std::endl;
      if (length){
	a.destroy(data,length);
      }
    }

    void get(T* d){
      copyMem(d,data,length);
    }


    Array& operator =(Array& b){
       if (!length&&b.size()){
	length=b.size();
	data=a.getNew(length);		
      }
       copyMem(data,b.data,length);
       return *this;
     }

    template<typename T2,int N2,typename AT2>  
    Array& operator =(const Array<T2,N2,AT2>& b){
      if (!length&&b.size()){
	length=b.size();
	data=a.getNew(length);		
      }
      copyMem(data,b.data,length);
      return *this;
    }
    
    Array& operator =(const T* b){
      copyMem(data,b,length);
      return *this;
    }
     
    template <typename B> Array& operator =(const B &b){
       for (int i=0;i<length;i++){
	 data[i]=b[i];
       }
       return *this;
     }



    void resize(size_t n){
      if (n==length) return;
      if (length){
	T* oldData=data;
	data=a.getNew(n);
	if (n) memcpy(data,oldData,sizeof(T)*(std::min)(n,length));
	a.destroy(oldData,length);
      }else{
	if (n) data=a.getNew(n);
      }
      length=n;
    }

    void clear(){
      delete[] data;
      length=0;
    }


    void reset(size_t n){
      if (n==length) return;
      if (length){
	a.destroy(data,length);
      }
      if (n){
	data=a.getNew(n);
      }
      length=n;
    }
    
    
     template <typename B>
     bool operator!=(const B& b) const{
       return !equals(*this,b);
     }

    template <typename B>
     bool operator==(const B& b) const{
       return equals(*this,b);
     }

  }; //Array<T,N>




  template <typename T,typename AT> class Array<T,array::RESIZABLE,array::StaticAllocator<AT> >{
  protected:
    size_t length;
    
  public:
    typedef T value_type;
  

    T* data;
    
    inline size_t size()const {
      return length;
    }
    
    inline T& operator[](int x) const{
      return data[x];
    }

    inline T& back(){
      return data[int(length)-1];
    }

    inline const T& back() const{
      return data[int(length)-1];
    }


    operator T*(){
      return data;
    }
    
    operator const T*() const{
      return data;
    }

    Array(size_t n=0):length(n),data(n?AT::getNew(n):NULL){                     
    }

    
    Array(const Array&s):length(s.length),data(length?AT::getNew(length):NULL){   
      copyMem(data,s.data,length);
    }  
    
   
    virtual ~Array(){
      //std::cout<<"deleting"<<data<<std::endl;
      if (length){
	AT::destroy(data,length);
      }
    }

    void get(T* d){
      copyMem(d,data,length);
    }


    Array& operator =(const Array& b){
      if (!length&&b.size()){
	length=b.size();
	data=AT::getNew((int)length);		
      }
      copyMem(data,b.data,length);
    }

    template<typename T2,int N2,typename AT2>  
    Array& operator =(const Array<T2,N2,AT2>& b){
      if (!length&&b.size()){
	length=b.size();
	data=AT::getNew((int)length);		
      }
      copyMem(data,b.data,length);
    }
    
    Array& operator =(const T* b){
      copyMem(data,b,length);
       return *this;
     }
     
    template <typename B> Array& operator =(const B &b){
       for (int i=0;i<length;i++){
	 data[i]=b[i];
       }
       return *this;
     }



    void resize(size_t n){
      if (n==length) return;
      if (length){
	T* oldData=data;
	data=AT::getNew(n);
	if (n) memcpy(data,oldData,sizeof(T)*std::min(length,n));
	AT::destroy(data,length);
      }else{	
	if (n) data=AT::getNew(n);
      }
      length=n;
    }

    void clear(){
      delete[] data;
      length=0;
    }

     void reset(size_t n){
       if (n==length) return;
       if (length){
	 AT::destroy(data,length);
       }
       if (n){
	 data=AT::getNew(n);
       }
     }

     template <typename B>
     bool operator!=(const B& b) const{
       return !equals(*this,b);
     }

    template <typename B>
    bool operator==(const B& b) const{
      return equals(*this,b);
    }
  }; //Array<T,N>






  template <typename T> class Array<T,array::RESIZABLE,array::HEAP>{
  protected:
    size_t length;
    
  public:
    typedef T value_type;
    T* data;
    
    inline size_t size()const {
      return length;
    }
    
    inline T& operator[](int x) const{
      return data[x];
    }

    inline T& back(){
      return data[int(length)-1];
    }

    inline const T& back() const{
      return data[int(length)-1];
    }

        operator T*(){
      return data;
    }
    
    operator const T*() const{
      return data;
    }


  Array(size_t n=0):length(n),data(n?new T[n]:NULL){                     
    }
    
    Array(const Array&s):length(s.length),data(length?new T[length]:NULL){     
      copyMem(data,s.data,length);
    }  
    
   
    virtual ~Array(){     
      if (length)
	delete[] data;
    }

    void get(T* d){
      if (length){
       copyMem(d,data,length);
      }
    }


      
    Array& operator =(const Array& b){
      if (!length&&b.size()){
	length=b.size();
	data=new T[length];		
      }
      copyMem(data,b.data,length);
      return *this;
     }
    


    template<typename T2,int N2,typename AT2>  
    Array& operator =(const Array<T2,N2,AT2>& b){
      if (!length&&b.size()){
	length=b.size();
	data=new T[length];		
      }
      copyMem(data,b.data,length);
      return *this;
     }
    
    
    Array& operator =(const T* b){
      for (size_t i=0;i<length;i++){
	data[i]=b[i];
      }
      return *this;
    }
    
     
    template <typename B> Array& operator =(const B &b){
       for (size_t i=0;i<length;i++){
	 data[i]=b[i];
       }
       return *this;
     }
    


    void resize(size_t n){
      if (n==length) return;
      //std::cout<<"resizing to "<<n<<std::endl;
      if (length){
	T* oldData=data;
	data=new T[n];
	//if (n) memcpy(data,oldData,sizeof(T)*(std::min)(n,length));
	for (size_t i=0;i<(std::min)(n,length);i++){
	  data[i]=oldData[i];
	}
	delete[] oldData;
      }else{
	if (n) data=new T[n];
      }
      length=n;
      //std::cout<<"resized to "<<n<<std::endl;
    }

    void clear(){
      delete[] data;
      length=0;
    }

    void reset(size_t n){
      if (n==length) return;
      //std::cout<<"resizing to "<<n<<std::endl;
      if (length){
	delete[] data;
      }      
      if (n){
	data=new T[n];
      }
      length=n;
      //std::cout<<"resized to "<<n<<std::endl;
    }

    
    template <typename B>
    bool operator!=(const B& b) const{
      return !equals(*this,b);
    }

    template <typename B>
    bool operator==(const B& b) const{
      return equals(*this,b);
    }

  }; //Array<T,N>



  template <typename T> class Array<T,array::RESETTABLE,array::HEAP>{
  protected:
    size_t length;
    
  public:
    typedef T value_type;
    T* data;
    
    inline size_t size()const {
      return length;
    }
    
    inline T& operator[](int x) const{
      return data[x];
    }

    inline T& back(){
      return data[int(length)-1];
    }

    inline const T& back() const{
      return data[int(length)-1];
    }

        operator T*(){
      return data;
    }
    
    operator const T*() const{
      return data;
    }

  Array(size_t n=0):length(n),data(n?new T[n]:NULL){                     
    }
    
    Array(const Array&s):length(s.length),data(length?new T[length]:NULL){     
      copyMem(data,s.data,length);
    }  
    
   
    virtual ~Array(){     
      if (length)
	delete[] data;
    }

    void get(T* d){
      if (length){
       copyMem(d,data,length);
      }
    }


  
    Array& operator =(const Array& b){
      if (!length&&b.size()){
	length=b.size();
	data=new T[length];		
      }
      copyMem(data,b.data,length);
      return *this;
     }
    

    template<typename T2,int N2,typename AT2>  
    Array& operator =(const Array<T2,N2,AT2>& b){
      if (!length&&b.size()){
	length=b.size();
	data=new T[length];		
      }
      copyMem(data,b.data,length);
      return *this;
     }
    
    
    Array& operator =(const T* b){
      for (size_t i=0;i<length;i++){
	data[i]=b[i];
      }
      return *this;
    }
    
     
    template <typename B> Array& operator =(const B &b){
       for (size_t i=0;i<length;i++){
	 data[i]=b[i];
       }
       return *this;
     }
    


    void resize(size_t n){
      reset(n);
    }

    void clear(){
      delete[] data;
      length=0;
    }

    void reset(size_t n){
      if (n==length) return;
      //std::cout<<"resizing to "<<n<<std::endl;
      if (length){
	delete[] data;
      }      
      if (n){
	data=new T[n];
      }
      length=n;
      //std::cout<<"resized to "<<n<<std::endl;
    }

    
    template <typename B>
    bool operator!=(const B& b) const{
      return !equals(*this,b);
    }

    template <typename B>
    bool operator==(const B& b) const{
      return equals(*this,b);
    }

  }; //Array<T,N>





  template <typename T> class Array<T,array::RESIZABLE,array::LINK>{   
  public:
  typedef T value_type;
    T* data;
    size_t length;

        operator T*(){
      return data;
    }
    
    operator const T*() const{
      return data;
    }

    Array():data(NULL),length(0){
    }


    Array(T* d,int n):data(d),length(n){
    }

    template<int N,typename A>
    Array(Array<T,N,A>& a):data(a.data),length(a.size()){
    }


    inline size_t size()const {
      return length;
    }
     
     inline T& operator[](int x){
      return data[x];
    }

    inline const T& operator[](int x) const{
      return data[x];
    }

    inline T& back(){
      return data[int(length)-1];
    }

    inline const T& back() const{
      return data[int(length)-1];
    }
     
    void get(T* d){
      copyMem(d,data,length);
    }


     Array& operator =(const Array& b){
       copyMem(data,b.data,length);
       return *this;
     }

    template<typename T2,int N2,typename AT2>  
    Array& operator =(const Array<T2,N2,AT2>& b){
       copyMem(data,b.data,length);
       return *this;
     }

     Array& operator =(const T* b){
       copyMem(data,b,length);
       return *this;
     }
     
     template <typename B> Array& operator =(const B &b){
        for (int i=0;i<length;i++){
	 data[i]=b[i];
       }
       return *this;
     }

     template <typename B>
     bool operator!=(const B& b) const{
       return !equals(*this,b);
     }

    template <typename B>
    bool operator==(const B& b) const{
      return equals(*this,b);
    }

    void resize(size_t n){
      length=n;
    }
  };



  template <typename T,int N> class Array<T,N,array::LINK>{   
  public:
  typedef T value_type;
    T* data;

        operator T*(){
      return data;
    }
    
    operator const T*() const{
      return data;
    }

    Array(){
      throw exc::InvalidOperation("Constructing a link array with no link");
    }


    Array(T* d):data(d){
    }

    inline size_t size()const {
      return N;
    }
    
    inline T& operator[](int x){
      return data[x];
    }

    inline const T& operator[](int x) const{
      return data[x];
    }

    inline T& back(){
      return data[N-1];
    }

    inline const T& back() const{
      return data[N-1];
    }
     
     void get(T* d){
      copyMem(d,data,N);
     }

    Array& operator =(const Array& b){
       copyMem(data,b.data,N);
       return *this;
     }

    template<typename T2,int N2,typename AT2>  
    Array& operator =(const Array<T2,N2,AT2>& b){
       copyMem(data,b.data,N);
       return *this;
     }

     Array<T,N>& operator =(const T* b){
       copyMem(data,b,N);
       return *this;
     }
     
     template <typename B> Array<T,N>& operator =(const B &b){
       for (int i=0;i<N;i++){
	 data[i]=b[i];
       }
       return *this;
     }

    /*void resize(size_t n){
      if (n!=N){
	throw exc::InvalidOperation("resizing an array on Stack");
      }
      }*/
     template <typename B>
     bool operator!=(const B& b) const{
       return !equals(*this,b);
     }

    template <typename B>
    bool operator==(const B& b) const{
      return equals(*this,b);
    }

  };





  template<typename T> class Array<T,array::RESIZABLE,array::MALLOC>{
    size_t capacity;    
    size_t length;
   
  public:
    T* data;

        operator T*(){
      return data;
    }
    
    operator const T*() const{
      return data;
    }

    Array():capacity(0),length(0),data(NULL){
    }
    
    ~Array(){
      if (capacity){
	free(data);
      }
    }



     Array& operator =(const Array& b){
       copyMem(data,b.data,length);
       return *this;
     }

    template<typename T2,int N2,typename AT2>  
    Array& operator =(const Array<T2,N2,AT2>& b){
       copyMem(data,b.data,length);
       return *this;
     }

     Array& operator =(const T* b){
       copyMem(data,b,length);
       return *this;
     }



    T& operator[](int i){
      return data[i];
    }

    const T& operator[](int i) const{
      return data[i];
    }

    inline T& back(){
      return data[int(length)-1];
    }

    inline const T& back() const{
      return data[int(length)-1];
    }

    size_t size() const{
      return length;
    }

    void resize(size_t n){
      if (n==length) return;
      size_t ncapacity;
      if (n>capacity){
	ncapacity=capacity*2+1;
      }else if (n<capacity/4){
	ncapacity=capacity/2+1;
      }else{
	ncapacity=capacity;
      }
      if (ncapacity!=capacity){
	void* space=realloc(data,ncapacity*sizeof(T));
	if (space!=NULL){
	  data=(T*)space;
	}else{
	  std::cerr<<"Memory allocation error, size "<<n<<std::endl;
	  return;
	}
      }
      length=n;
      capacity=ncapacity;
    }

    void clear(){
      resize(0);
    }
    
    void push_back(const T& t=T()){
      resize(length+1);
      memcpy(&data[length-1],&t,sizeof(T));
    }

    void pop_back(){
      if (!length) return;
      resize(length-1);
    }
      
    void eraseAll(const T& t){
      T* swap=NULL;
      int erased=0;
      int start=-1;
      int out=0;
      for (int i=0;i<(int)length;i++){
	if (data[i]!=t&&start==-1){
	  start=i;
	}
	if (data[i]==t&&start!=-1){
	  if (!erased){
	    swap=(T*)malloc(sizeof(T)*length);
	    if (swap==NULL){
	      std::cerr<<"Error in allocation of swap space for eraseAll"<<std::endl;
	      return;
	    }
	  }
	  erased++;
	  memcpy(swap+out,data+start,sizeof(T)*(i-start));
	  //std::cout<<"copying "<<start<<"..."<<i-1<<std::endl;
	  out+=i-start;
	  start=-1;
	}
      }//for
      if (erased){
	if (start!=-1){
	  memcpy(swap+out,data+start,sizeof(T)*((int)length-start));
	  erased++;
	}
	memcpy(data,swap,sizeof(T)*((int)length-erased));
	free(swap);
	resize(length-erased);
      }//if erased      	

    }//eraseAll


    void erase(const T& t){
      for (int i=0;i<(int)length;i++){
	if (data[i]==t){
	  if (i<(int)length-1){
	    T* swap=(T*)malloc(sizeof(T)*((int)length-i-1));
	    if (swap==NULL){
	      std::cerr<<"Error in allocation of swap space for erase"<<std::endl;
	      return;
	    }
	    memcpy(swap,&data[i+1],sizeof(T)*((int)length-i-1));
	    memcpy(&data[i],swap,sizeof(T)*((int)length-i-1));	    	    
	    free(swap);
	  }
	  resize(length-1);
	  return;
	}
      }//for
    }

    std::pair<int,bool> push_once(const T& t){
      for (int i=0;i<(int)length;i++){
	if (data[i]==t) return std::make_pair(i,false);
      }
      push_back(t);      
      return std::make_pair(length-1,true);
    }


  };//array<MALLOC>


  template<typename T,int N,typename A>
  void init(Array<T,N,A>& v,int start,int n){
    for (int i=start;i<n;i++){
      new (v.data+i) T();
    }
    
  }

  template<typename T,int N,typename A,typename Allocator>
  void allocInit(Array<T,N,A>& v,int start,int n,const Allocator& a){
    a.construct(v.data,start,n);    
  }

  template<typename T,int N,typename A,typename Allocator >
  void allocInit(Array<T,N,A>& v,int start,int n){
    Allocator::construct(v.data,start,n);    
  }


  template<typename T,typename P,typename PP=const P&>
  class ParamAllocator{
    P p;
    
  public:
    ParamAllocator(PP pp):p(pp){

    }
  
    T* getNew(size_t n) const {
      T* space=(T*)malloc(sizeof(T)*n);
      for (size_t i=0;i<n;i++){
	new (space+(int)i) T(p);  //Placement new
      }
      return space;
    }

    void destroy(T* t,size_t n)const{
      for (int i=0;i<(int)n;i++){
	(t+i)->~T();
      }
      free(t);
    }

    void construct(T* space,int start,int n)const{
      for (int i=start;i<start+n;i++){
	new (space+i) T(p);
      }
    }

  };




  template<typename T,typename P,typename PP=typename natReference<P>::value>
  class ParamIdxAllocator{
    P p;
    
  public:
    ParamIdxAllocator(PP pp):p(pp){      
    }
  
    T* getNew(size_t n) const {
      T* space=(T*)malloc(sizeof(T)*n);
      for (size_t i=0;i<n;i++){
	new (space+(int)i) T(p,(int)i);  //Placement new
      }
      return space;
    }

    void destroy(T* t,size_t n)const{
      for (size_t i=0;i<n;i++){
	(t+i)->~T();
      }
      free(t);
    }

    void construct(T* space,int start,int n)const{
      for (int i=start;i<start+n;i++){
	new (space+i) T(p,i);
      }
    }

  };


  template<typename T,typename R,typename C,R (C::*f)(int) >
  class ClassFuncIdxAllocator{
    C* c;
    
  public:
    ClassFuncIdxAllocator(C* pc):c(pc){      
    }
  
    T* getNew(size_t n) const {
      T* space=(T*)malloc(sizeof(T)*n);
      construct(space,0,n);
      return space;
    }

    void destroy(T* t,size_t n)const{
      for (int i=0;i<(int)n;i++){
	(t+i)->~T();
      }
      free(t);
    }

    void construct(T* space,int start,int n)const{
      for (int i=start;i<start+n;i++){
	new (space+i) T( (c->*f)(i) );
      }
    }

  };


  template<typename T,typename R,R (*f)(int) >
  class FuncAllocator{        
  public:

    T* getNew(size_t n) const {
      T* space=(T*)malloc(sizeof(T)*n);
      construct(space,0,n);
      return space;
    }

    void destroy(T* t,size_t n)const{
      for (int i=0;i<(int)n;i++){
	(t+i)->~T();
      }
      free(t);
    }

    void construct(T* space,int start,int n)const{
      for (int i=start;i<start+n;i++){
	new (space+i) T( (*f)(i) );
      }
    }

  };


  //passes the constructed objects p and f(i)
  template<typename T,typename P,typename R,R (*f)(int),typename PP=typename natReference<P>::value>
  class ParamFuncAllocator{        
    P p;
  public:
    
    ParamFuncAllocator(PP pp):p(pp){      
    }
    
    T* getNew(size_t n) const {
      T* space=(T*)malloc(sizeof(T)*n);
      construct(space,0,n);
      return space;
    }

    void destroy(T* t,size_t n)const{
      for (int i=0;i<(int)n;i++){
	(t+i)->~T();
      }
      free(t);
    }

    void construct(T* space,int start,int n)const{
      for (int i=start;i<start+n;i++){
	new (space+i) T(p, (*f)(i) );
      }
    }

  };


  //passes the constructed objects f(p,i)

  template<typename T,typename P,typename R,R (*f)(P,int),typename PP=typename natReference<P>::value>
  class ParametricFuncAllocator{        
    P p;
  public:
    
    ParametricFuncAllocator(PP pp):p(pp){      
    }
    
    T* getNew(size_t n) const {
      T* space=(T*)malloc(sizeof(T)*n);
      construct(space,0,n);
      return space;
    }

    void destroy(T* t,size_t n)const{
      for (int i=0;i<(int)n;i++){
	(t+i)->~T();
      }
      free(t);
    }

    void construct(T* space,int start,int n)const{
      for (int i=start;i<start+n;i++){
	new (space+i) T((*f)(p,i) );
      }
    }

  };

  

  //calls a function, which must use the placeement new
  template<typename T,typename C,void (C::*ctf)(T*,int) >
  class ClassPlacerAllocator{
    C* c;
    
  public:
    ClassPlacerAllocator(C* pc):c(pc){      
    }
  
    T* getNew(size_t n) const {
      T* space=(T*)malloc(sizeof(T)*n);
      construct(space,0,n);
      return space;
    }

    void destroy(T* t,size_t n)const{
      for (int i=0;i<(int)n;i++){
	(t+i)->~T();
      }
      free(t);
    }

    void construct(T* space,int start,int n)const{
      for (int i=start;i<start+n;i++){
	(c->*ctf)(space+i,i);
      }
    }

  };

  //calls a function, which much do a placement new. this function is specified at construction time
  template<typename T,typename C >
  class DynamicClassPlacerAllocator{
    C* c;
    typedef void (C::*AllocFunc)(T*,int);
    AllocFunc ctf;


  public:
    DynamicClassPlacerAllocator(const std::pair<C*,AllocFunc>& p):c(p.first),ctf(p.second){      
    }
  
    T* getNew(size_t n) const {
      T* space=(T*)malloc(sizeof(T)*n);
      construct(space,0,n);
      return space;
    }

    void destroy(T* t,size_t n)const{
      for (int i=0;i<(int)n;i++){
	(t+i)->~T();
      }
      free(t);
    }

    void construct(T* space,int start,int n)const{
      for (int i=start;i<start+n;i++){
	(c->*ctf)(space+i,i);
      }
    }

  };




  
  template<typename T,typename P>
  class IdxParamAllocator{
    P p;
    
  public:
    IdxParamAllocator(const P& pp):p(pp){      
    }
  
    T* getNew(size_t n) const {
      T* space=(T*)malloc(sizeof(T)*n);
      for (size_t i=0;i<n;i++){
	new (space+(int)i) T((int)i,p);  //Placement new
      }
      return space;
    }

    void destroy(T* t,size_t n)const{
      for (int i=0;i<n;i++){
	(t+i)->~T();
      }
      free(t);
    }

    void construct(T* space,int start,int n)const{
      for (int i=start;i<start+n;i++){
	new (space+i) T(i,p);
      }
    }

  };


  //passing an element of an array as parameter
  template<typename T,typename P>
  class VectAllocator{
    P& p;
    
  public:
    VectAllocator(P& pp):p(pp){      
    }
  
    T* getNew(size_t n)const{
      T* space=(T*)malloc(sizeof(T)*n);
      //std::cout<<"getting new "<<n<<"@"<<space<<std::endl;
      for (size_t i=0;i<n;i++){
	new (space+(int)i) T(p[i]);  //Placement new
      }
      return space;
    }

    void construct(T* space,int start,int n)const{
      for (int i=start;i<start+n;i++){
	new (space+i) T(p[i]);
      }
    }

    void destroy(T* t,size_t n)const{
      ///std::cout<<"destroying "<<n<<"@"<<t<<std::endl;
      for (int i=0;i<(int)n;i++){
	//std::cout<<"invoking the deconstructor of"<<(t+i)<<std::endl;
	(t+i)->~T();
      }
      free(t);
    }


  };

  //fb 2014.10.05
  //passing an element of an array as parameter plus index
  template<typename T,typename P>
  class VectAllocatorIdx{
    P& p;
    
  public:
    VectAllocatorIdx(P& pp):p(pp){      
    }
  
    T* getNew(size_t n)const{
      T* space=(T*)malloc(sizeof(T)*n);
      //std::cout<<"getting new "<<n<<"@"<<space<<std::endl;
      for (size_t i=0;i<n;i++){
	new (space+(int)i) T(p[i],(int)i);  //Placement new
      }
      return space;
    }

    void construct(T* space,int start,int n)const{
      for (int i=start;i<start+n;i++){
	new (space+i) T(p[i],(int)i);
      }
    }

    void destroy(T* t,size_t n)const{
      ///std::cout<<"destroying "<<n<<"@"<<t<<std::endl;
      for (int i=0;i<(int)n;i++){
	//std::cout<<"invoking the deconstructor of"<<(t+i)<<std::endl;
	(t+i)->~T();
      }
      free(t);
    }


  };

  template<typename T,typename P>
  class VectElemPointerAllocator{
    P& p;
    
  public:
    VectElemPointerAllocator(P& pp):p(pp){      
    }
  
    T* getNew(size_t n)const{
      T* space=(T*)malloc(sizeof(T)*n);
      for (size_t i=0;i<n;i++){
	new (space+(int)i) T(&p[i]);  //Placement new
      }
      return space;
    }

    void construct(T* space,int start,int n)const{
      for (int i=start;i<start+n;i++){
	new (space+i) T(&p[i]);
      }
    }

    void destroy(T* t,size_t n)const{
      for (int i=0;i<(int)n;i++){
	(t+i)->~T();
      }
      free(t);
    }


  };




  template<typename T>
  class IdxAllocator{

    
  public:
    
    template<typename U> IdxAllocator(const U&){
    }

    static T* getNew(size_t n){
      T* space=(T*)malloc(sizeof(T)*n);
      for (size_t i=0;i<n;i++){
	new (space+(int)i) T((int)i);  //Placement new
      }
      return space;
    }

    static void construct(T* space,int start,int n){
      for (int i=start;i<start+n;i++){
	new (space+i) T(i);
      }
    }

    static void destroy(T* t,size_t n){
      for (int i=0;i<(int)n;i++){
	(t+i)->~T();
      }
      free(t);
    }


  };


  template<typename T,typename P,typename P2>
  class Vect2Allocator{
    const P& p;
    const P2& p2;
  public:
    Vect2Allocator(const P& pp,const P2& pp2):p(pp),p2(pp2){      
    }
  
    T* getNew(size_t n)const{
      T* space=(T*)malloc(sizeof(T)*n);
      for (size_t i=0;i<n;i++){
	new (space+(int)i) T(p[i],p2[i]);  //Placement new
      }
      return space;
    }

    void construct(T* space,int start,int n)const{
      for (int i=start;i<start+n;i++){
	new (space+i) T(p[i],p2[i]);
      }
    }

    void destroy(T* t,size_t n)const{
      for (int i=0;i<(int)n;i++){
	(t+i)->~T();
      }
      free(t);
    }


  };


  
  /*  template<typename T,int N,typename A> std::ostream& operator<<(std::ostream &o,const helium::Array<T,N,A> &a){
    for (int i=0;i<(int)a.size();i++){      
      o<<" "<<a.data[i];
    }
    return o;   	
  }


  template<typename T,int N,typename A> std::istream& operator>>(std::istream &i,helium::Array<T,N,A> &a){
    for (int c=0;c<(int)a.size();c++){      
      i>>a[c];
    }
    return i;   	
  }

  template<typename T,typename A> std::istream& operator>>(std::istream &i,helium::Array<T,array::RESIZABLE,A> &a){
    if (a.size()){
      for (int c=0;c<(int)a.size();c++){      
	i>>a[c];
      }
    }else{
      std::vector<T> x;
      T v;
      while(i){	
	i>>v;
	if (!i.fail()){
	  x.push_back(v);
	}
      }//while i
      a.resize(x.size());
      a=x;
    }
    i.clear();
    return i;   	
    }*/


  /*template<typename C>
  struct hasSize;
  
  template<typename T,int N,typename A> 
  struct hasSize<Array<T,N,A> >{
    static const bool value=true;
  };

  */


}//helium





#endif
