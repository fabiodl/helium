#ifndef HE_TSLLIST
#define HE_TSLLIST


#include "slList.h"

namespace helium{



  
  template <typename T,typename N=node::SlPlainNode<T> >
  class TsSlList{
  public:
    typedef N Node;
    typedef T value_type; 
  private:
    Mutex m;
    Node* head;
    Node* last;


    void unsafe_push_back(Node* n){
      if (last==NULL){
	head=last=n;
      }else{
	last->next=n;
	last=n;
      }
    }

  public:
    TsSlList():m("list"),head(NULL),last(NULL){
    }

    ~TsSlList(){
      Node* n=head;
      while (n!=NULL){
	Node* m=n;
	n=n->next;
	delete m;
      }
      
    }


    void push_back(T* data,int n){

      m.lock();
      for (int i=0;i<n;i++){
	unsafe_push_back(new Node(data[i]));
      }
      m.unlock();
      
    }

    /*
    bool isFirstPush_back(const T& data){
      Node* n=new Node(data);
      m.lock();
      bool isLast=(last==NULL);
      unsafe_push_back(n);
      m.unlock();
      return isLast;
      };*/



    void push_back(const T& data){
      Node* n=new Node(data);
      m.lock();
      unsafe_push_back(n);
      m.unlock();
    };

    Node* popNode(){
      m.lock();
      Node* n=head;
      if (head!=NULL) head=head->next;
      m.unlock();
      return n;
    }

    Node* popList(){
      m.lock();
      Node* n=head;
      head=last=NULL;
      m.unlock();
    }
    
  };




};

#endif
