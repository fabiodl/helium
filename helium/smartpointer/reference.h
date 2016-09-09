/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_REFERENCE
#define HE_REFERENCE

#include <cstring>

namespace helium{

  template<typename T>
  class SingleReference{
    T* t;
    //SingleReference(const SingleReference& r){
    ////not allowed!
    //}
    SingleReference& operator=(const SingleReference& b);            //not allowed!


  public:

    SingleReference():t(NULL){
    }

    SingleReference(T* pt):t(pt){
    }
    
    explicit SingleReference(SingleReference& r){
      t=r.t;
      r.t=NULL;
    }


    SingleReference& operator=(SingleReference& b){
      if (t) delete t;
      t=b.t;
      b.t=NULL;
      //std::cout<<"Object "<<t<<" transferred from "<<&b<<" to "<<this<<std::endl;
      return *this;
    }    

    SingleReference& operator=(T* tp){
      if (t) delete t;
      t=tp;
      //std::cout<<"Reference "<<this<<"kinked to object "<<t<<std::endl;
      return *this;
    }    
 
    void reset(T* tp){
       if (t) delete t;
       t=tp;
    }


    T* operator&(){
      return t;
    }

    const T* operator&() const{
      return t;
    }
    
    T* operator->(){
      return t;
    }
  
    T* get() const{
      return t;
    }

    /*const T* get() const{
      return t;
      }*/

    const T* operator->() const{
      return t;
    }

    T& operator*(){
      return *t;
    }

    const T& operator*() const{
      return *t;
    }


    T* release(){
      T* p=t;
      t=NULL;
      return p;
    }

    ~SingleReference(){
      if (t!=NULL){
	//std::cout<<"Reference "<<this<<"deleted object "<<t<<std::endl;
	delete t;
      }
    }
    
    template <typename S> bool operator<(const SingleReference<S>& b){
      return *t<*b.t;
    }

  };




  
  template<typename T>
  class Referred{
    int count;
    T t;
  public:
    
    
    Referred(const T& pt):count(0),t(pt){    
    }
    bool isUnreferred(){
      return count==0;
    }
    void inc(){
      count++;
    };
    void dec(){
      count--;
    }
  };


  /*  template<typename T>
  class Reference{
    Referred<T>* p;
  public:    

    Reference():p(NULL){
    }

    Reference(const T& t){
      p=new Referred<T>(t);
      p->inc();
    }

    Reference(Reference& b){
      p=b.p;
      p->inc();
    }

    Reference(Referred<T>* pp){
      p=pp;
      p->inc();      
    }

        Reference& operator=(Reference& b){
      p=b.p;
      p->inc();
      return *this;
    }


    Reference& operator=(Referred<T>* pp){
      p=pp;
      p->inc();
      return *this;
    }

    
    ~Reference(){
      if (p!=NULL){
	p->dec();	
      }
      if (p->isUnreferred()){
	delete p;
      }
    }
    
    

    };*/


  /* template<typename T>
  class Pointer{
    Referred<T>* p;
  public:    

    Pointer():p(NULL){
    }

    Pointer(const T& t){
      p=new Referred<T>(t);
      p->inc();
    }

    Pointer(Pointer& b){
      p=b.p;
      p->inc();
    }

    Pointer(Referred<T>* pp){
      p=pp;
      p->inc();      
    }

    Pointer& operator=(Pointer& b){
      p=b.p;
      p->inc();
      return *this;
    }


    Pointer& operator=(Referred<T>* pp){
      p=pp;
      p->inc();
      return *this;
    }

    
    ~Pointer(){
      if (p!=NULL){
	p->dec();	
      }
      if (p->isUnreferred()){
	delete p;
      }
    }

    T* operator&(){
      return p;
    }
    */
    /*T* operator->(){
      return p;
      }
    */
  /*
    T& operator*(){
      return *p;
      }

    bool operator<(const Referred<T>& b){
      return *p<*b.p;
    }

  };
  */


  /*template<typename T>
    class SingleReference{
    T* t;
    bool toDel;

    SingleReference(const SingleReference& r){}//cannot copy
    SingleReference& operator=(const SingleReference& b){return *this;}
    
  public:

    SingleReference():toDel(false){
    }


    void set(T* pt,bool ptoDel){
      t=pt;
      toDel=ptoDel;
    }
    
    T* operator->(){
      return t;
    }
  
    T& operator*(){
      return *t;
    }
    ~SingleReference(){
    }
    };*/
  
 


}




#endif
