/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera 
 * \author  Fransiska Basoeki
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SLCONTAINER
#define HE_SLCONTAINER

#include <cstring>
#include <iostream>

namespace helium{
  template <typename T> class SLNode{    
  public:
    T data;
    SLNode* next;    
  SLNode():next(NULL){      
    }
    
  SLNode(const T& t):data(t),next(NULL){      
    }

    T& operator*(){
      return data;
    }

  };


  template <typename T> 
  class UniqueReference{ //specialized underneath  for SLNode
    T* p;
  public:
  UniqueReference():p(NULL){
    }
    virtual ~UniqueReference(){      
      delete p;
    }
    
    void link(T* pp){
      if (p!=NULL) delete p;
      p=pp;
    }
    
    T& operator->(){
      return p;
    }

    T& operator*(){
      return *p;
    }

  };

  /*  template <typename T>
    class UniqueReference<SLNode<T> >{
    SLNode<T>* p;
  public:
  UniqueReference():p(NULL){
    }

    virtual ~UniqueReference(){
      if (p!=NULL)
      delete p;
    }

    bool isValid(){
      return p!=NULL;
    }

    void link(SLNode<T>* pp){
      if (p!=NULL) delete p;
      p=pp;
    }
    
    T& operator->(){
      return p->data;
    }

    T& operator*(){
      return p->data;
    }

    bool hasNext(){
      return p->next!=NULL;
    }

    UniqueReference & operator++(int){
      link(p->next);      
    }
    
  };//

  */

  /*  template <typename T>
    class SLFifo{
    SLNode<T>* head;
    SLNode<T>** insertPoint;
  public:
  SLFifo():head(NULL),insertPoint(&head){
      std::cout<<"New fifo @"<<this<<std::endl;
    }

    void push_back(const T& data){
      SLNode<T>* n=new SLNode<T>(data);      
      *insertPoint=n;
      insertPoint=&n->next;      
      //std::cout<<"pushed "<<data<<" on "<<this<<"that has head "<<head<<std::endl;
    }

    void pop(UniqueReference<SLNode<T> > &r){
      //std::cout<<"popping with head "<<head<<std::endl;
      r.link(head);
      if (head!=NULL){
	head=head->next;
	insertPoint=&head;
      }
    }

      virtual ~SLFifo(){
      SLNode<T>* n=head,*succ;
      while (n!=NULL){
	succ=n->next;
	delete n;
	n=succ;
      }
    }
    
  };
  */

}
#endif
