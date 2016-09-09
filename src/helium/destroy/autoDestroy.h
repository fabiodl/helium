/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_AUTODESTROY_
#define HE_AUTODESTROY_
#include <vector>
namespace helium{


  template<typename T> void destroyS(T& v,int n){
  for (int i=0;i<n;i++){
    //std::cout<<"deleting "<<v[i]<<std::endl;
    delete v[i];
  }
  v.clear();
}

  template<size_t N,typename T> void destroyS(T& v){
  for (size_t i=0;i<N;i++){
    //std::cout<<"deleting "<<v[i]<<std::endl;
    delete v[i];
  }
}



  template<typename T> void destroyNS(T& v,int n){
    for (int i=0;i<n;i++){
      if (v[i]!=NULL) delete v[i];
    }
    v.clear();
  }


  template<typename T>void destroyA(T& v,int n){
    for (int i=0;i<n;i++){
      delete[] v[i];
    }
    v.clear();
  }

  template<typename T>void destroyNA(T& v,int n){
    for (int i=0;i<n;i++){
      if (v[i]!=NULL) delete[] v[i];
    }
    v.clear();
  }


  template<typename T> void destroyIS(T& v){
    for (typename T::iterator it=v.begin();
	 it!=v.end();
	 ++it){
      delete *it;
    }
    v.clear();
  }

template<typename T>void destroyS(T& v){
  destroyS(v,v.size());
}

template<typename T>void destroyNS(T& v){
  destroyNS(v,v.size());
}


template<typename T>void destroyA(T& v){
  destroyA(v,v.size());
}

template<typename T>void destroyNA(T& v){
  destroyNA(v,v.size());
}

  
  template<typename T> void destroyValueS(T& v){
    for (typename T::iterator it=v.begin();it!=v.end();++it){
      delete it->second;
    }
    v.clear();
  }
  

template<typename T> class AutodestroyVector:public std::vector<T>{
 public:
  virtual ~AutodestroyVector(){
    for (unsigned int i=0;i<std::vector<T>::size();i++){
      delete std::vector<T>::at(i);
    }
  }
};

template<typename T> class AutodestroyArrayVector:public std::vector<T>{
 public:
  virtual ~AutodestroyArrayVector(){
    for (unsigned int i=0;i<std::vector<T>::size();i++){
      delete[] std::vector<T>::at(i);
    }
  }
};


class AutoDestroy{
 public:
  enum PointerClass{Scalar,Array,Malloc};

  class AutoDestroyPointer{    
  protected:
    PointerClass pointerType;
  public:

  AutoDestroyPointer(PointerClass ppointerType):pointerType(ppointerType){
    }
  virtual void destroy()=0;

    virtual ~AutoDestroyPointer(){}

  };
  
  template <typename T> class AutoDestroyTypedPointer:public AutoDestroyPointer{
    T* p;
  public:
  AutoDestroyTypedPointer(T* pp,PointerClass ppointerType):AutoDestroyPointer(ppointerType),p(pp){
    }
    


    void destroy(){
      switch(pointerType){
      case Scalar:
	delete p;
	break;
      case Array:
	delete[] p;
	break;
      case Malloc:
	free(p);
	break;
      }
    }
              
  };
  
  
  class AutoDestroyElement{
  protected:
    std::vector<AutoDestroyPointer*> toDestroy;
  public:
    void destroy(){
      for (size_t i=0;i<toDestroy.size();i++){
	toDestroy[i]->destroy();
	delete toDestroy[i];
      }
    }

    virtual ~AutoDestroyElement(){
      destroy();
    }
    
  };
  

  class AutoDestroyHook:public AutoDestroyElement{
    PointerClass pointersType;
  public:
    
  AutoDestroyHook(PointerClass t):pointersType(t){
    }
    
    template <typename T> T* operator=(T* e){
      toDestroy.push_back(new AutoDestroyTypedPointer<T>(e,pointersType));    
      return e;
    }    
  
    template <typename T>
    T* operator()(T* e){
      toDestroy.push_back(new AutoDestroyTypedPointer<T>(e,pointersType));    
      return e;
    }    

  };

      
 public:
  AutoDestroyHook s,a,m;

 AutoDestroy():s(Scalar),a(Array),m(Malloc){}
  
  void destroy(){
    s.destroy();
    a.destroy();
    m.destroy();
  }
  //( AutoDestroy::Scalar),a(Array),m(Malloc);
};
}//helium

#endif
