/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SLLIST
#define HE_SLLIST

#include <helium/container/slNode.h>

namespace helium{

  
  


  template <typename T,typename N=node::SlPlainNode<T> >
  class SlList{   
  public:
    typedef N Node;
    typedef T value_type; 
    
  private:
    Node *first,*last; //note: last is invalid when first is NULL
  public:
    
    SlList():first(NULL),last(NULL){
    }

    ~SlList(){
      Node* n=first;
      while (n!=NULL){
	Node* m=n;
	n=n->next;
	delete m;

      }
      
    }

    void push_back(const T &data){
      Node* n=new Node(data);
      if (first==NULL){
	first=last=n;
	//std::cout<<"pushed "<<data<<"in the list"<<std::endl;
      }else{
	last->next=n;
	last=n;
      }
    };

    Node* peek_last(){
      return last;
    }

    /*void erase_next(Node* n){
      if (n==NULL)
      }*/
    
    Node* pop_front(){
      Node* n=first;
      //std::cout<<this<<"==returning=="<<(n!=NULL?**n:"NULL") <<std::endl;
      if (first!=NULL){
	first=first->next;
      }
      return n;
    }

    Node* pop_all(){
      Node* n=first;
      first=NULL;
    }

    Node* find(const T& data,Node* n){
      while(n!=NULL){
	if (n->data==data) return n;
	n=n->next;
      }
      return NULL;
    }
    
    Node* find(const T& data){
      return find(data,first);
    }




  };


  template<typename N>
  void destroyList(N* n){
    while(n!=NULL){
      N* next=n->next;
      delete n;
      n=next;
    }
  }


};//helium


#endif
