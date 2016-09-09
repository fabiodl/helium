/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_MINIMALLIST
#define HE_MINIMALLIST


namespace helium{
  template<typename Node> class MinimalList{
     Node *first,*last;
  public:    
    MinimalList():first(NULL),last(NULL){}

    ~MinimalList(){
      for (Node *n=first;n!=NULL;){
	Node* next=n->next;
	delete n;
	n=next;
      }
    }


    void push_back(Node* n){
      n->next=NULL;
      if (last==NULL){
	first=last=n;
      }else{
	last->next=n;
	last=n;
      }
    };

    
    inline Node* peek_front(){
      return first;
    }

    inline Node* peek_front() const{
      return first;
    }

    Node* pop_front(){
      Node* n=first;
      if (first!=NULL) first=first->next;
      if (n==last){
	first=last=NULL;
      }
      return n;
    }

    Node* pop_list(){
      Node* n=first;
      first=last=NULL;
      return n;      
    }


    void push_list(Node* n){
      if (last==NULL){
	first=last=n;
      }else{
	last->next=n;
	while(last->next!=NULL){
	  last=last->next;
	}
      }
    };

    void push_list(Node* n,Node* nlast){
      if (last==NULL){
	first=last=n;
      }else{
	last->next=n;
	last=nlast;
      }
    };

    /*Node* unsafe_pop_front(){
      Node* n=first;
      first=first->next;
      return n;
      }*/

  };
  template<typename Node>
  
  std::ostream& operator<<(std::ostream& o,const MinimalList<Node>& l){
    for (Node* n=l.peek_front();n!=NULL;n=n->next){
      o<<" "<<n;
    }
    return o;
  }

}

#endif
