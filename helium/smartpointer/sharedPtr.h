/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SHAREDPTR
#define HE_SHAREDPTR

#include <helium/thread/mutex.h>

namespace helium{

  class SharedCount{
  Mutex m;
  int v;
public:
  
  SharedCount():m("sc"),v(1){}
  void operator++(int){
    m.lock();
    v++;
    m.unlock();
  }

  int operator--(int){
    int r;
    m.lock();
    r=--v;
    m.unlock();
    return r;
  }
};


template<typename T> class SharedPtr{
  
  T* p;
  SharedCount* cnt;

  void checkDeletion(){
    if (p&&(*cnt)--==0){
      delete p;
      delete cnt;
    }
  }

  

public:  
  
  SharedPtr(const SharedPtr& src):p(src.p),cnt(src.cnt){
    if (p){
      (*src.cnt)++;
    }
  }

  SharedPtr():p(NULL),cnt(NULL){
  }

  SharedPtr(T* pp):p(pp),cnt(new SharedCount()){
  }

  ~SharedPtr(){
    checkDeletion();
  }

  T* get(){
    return p;
  }

  T* operator->(){
    return p;
  }

  T& operator*(){
    return *p;
  }
  
  void operator=(T* pp){   
    checkDeletion();
    cnt=new SharedCount();
    p=pp;
  }

  
  SharedPtr& operator=(SharedPtr& src){
    checkDeletion(); 
    p=src.p;
    cnt=src.cnt;
    if (p){
      (*src.cnt)++;
    }
    return *this;
  }

  void assure(T* pp){   
    if (p!=pp){
      *this=pp;
    }
  }

  void assure(SharedPtr& src){   
    if (p!=src.p){
      *this=src;
    }
  }



  void reset(T* t){
    *this=t;
  }
    

};


}



#endif
