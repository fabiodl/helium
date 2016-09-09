/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_CIRCULARBUFFER
#define HE_CIRCULARBUFFER

#include <helium/container/slNode.h>

namespace helium{

  template<typename T> class CircularBuffer{
  public:
    typedef node::SlDCNode<T> Node;
  private:
    Node* head;
    int nodes;
  public:
    typedef Node* iterator;
    
  CircularBuffer():head(NULL),nodes(0){
    }
    int size(){
      return nodes;
    }

    void push_back(Node* n){
      nodes++;
      if (head!=NULL){ 
	//std::cout << " head is not null" << std::endl;
	n->next=head->next;
      }else{
	//std::cout << " head is null" << std::endl;
	head = n;
      }
      head->next=n;
    }
    inline  Node* push_back(const T& t){
      Node* n=new Node(t);
      push_back(n);
      return n;
    }

    inline  Node* push_back(){
      Node* n=new Node();
      push_back(n);
      return n;
    }

    void erase(Node* i){
      
      if (i->next==i){
	head=NULL;
      }else{
	Node* n=head;
	while (n->next!=i) n=n->next;
	n->next=i->next;
	if (head==i) head=i->next;
      }      
      delete i;
    }
    
    bool isEmpty(){
      return head=NULL;
    }

    T& get(){
      head=head->next;
      //std::cout<<"returning get"<<&head->data.msg<<std::endl;
      return head->data;
    }
    ~CircularBuffer(){
      if (head!=NULL){
      Node * n=head;
      do{
	Node* succ=n->next;
	delete n;
	n=succ;
      }while(n!=head);//when came back to the head, break;
      }//head!=NULL
    }//~CircularBuffer

  };//CircularBuffer



}


#endif
