#ifndef HE_DLLIST
#define HE_DLLIST

namespace helium{

  namespace node{

    template<typename T>
    struct DlAutoDeleteNode{ 
      typedef T value_type;
      T data;
      DlAutoDeleteNode *next,*prev;      
      DlAutoDeleteNode(const T& pdata):data(pdata),next(NULL),prev(NULL){
      }           
      ~DlAutoDeleteNode(){
	delete data;
      }
    };

    template<typename T>
    struct DlPlainNode{  
      typedef T value_type;
      T data;
      DlPlainNode *next,*prev;      
      DlPlainNode(const T& pdata):data(pdata),next(NULL),prev(NULL){
      }           
      
    };
    
  }//node

  template <typename T,typename N=node::DlPlainNode<T> >
  class DlList{

  public:
    typedef N Node;
    typedef T value_type; 
  private:
    Node* first,*last;
  public:
    
    class iterator{
      Node* n;
    public:

      iterator( Node* pn){
	n=pn;
      }

      iterator& operator=(Node* pn){
	n=pn;
	return *this;
      } 
      iterator& operator++ (int){
	
	return *this;
      }

      T& operator*(){
	return n->data;
      }
      
      T* operator->(){
	return &n->data;
      }
      

      bool operator!=(const Node* pn){
	return n!=pn;
      }

      bool operator!=(const iterator& b){
	return b.n!=n;
      }

    };
    

    class const_iterator{
      const Node* n;
    public:

      const_iterator(const Node* pn){
	n=pn;
      }

      const_iterator& operator=(const Node* pn){
	n=pn;
	return *this;
      } 
      const_iterator& operator++ (int){
	
	return *this;
      }

      const T& operator*(){
	return n->data;
      }
      
      const T* operator->(){
	return &n->data;
      }
      

      bool operator!=(const Node* pn){
	return n!=pn;
      }

      bool operator!=(const iterator& b){
	return b.n!=n;
      }

    };
    


    DlList():first(NULL),last(NULL){
    }

    ~DlList(){
      clear();
    }

    Node* begin(){
      return first;
    }

    const Node* begin() const{
      return first;
    }


    Node* back() const{
      return last;
    }

    bool isEmpty(){
      return first==NULL;
    }


    static Node* end(){
      return NULL;
    }


    void push_back(Node* n){
      if (last==NULL){
	first=last=n;
      }else{
	last->next=n;
	n->prev=last;
	last=n;
      }
    }
    
    Node* push_back(const T &data){
      Node* n=new Node(data);
      push_back(n);      
      return n;
    };


    Node* pop_front(){
      Node* n=first;
      if (first!=NULL){
	first=first->next;	
      }
      if (first!=NULL){
	first->prev=NULL;
      }
      return n;
    }

    

    Node* find_front(const T& data,Node* n){
      while(n!=NULL){
	if (n->data==data) return n;
	n=n->next;
      }
      return NULL;
    }
    
    Node* find_front(const T& data){
      find_front(data,first);
    }

    void erase(Node* n){
      if (n==first){
	first=n->next;
      }else if (n==last){
	last=n->prev;
      }
      
      if (n->prev!=NULL) n->prev->next=n->next;
      if (n->next!=NULL) n->next->prev=n->prev;      
      delete n;
    }
    

    void erase(const T& data){
      erase(find_next(data));
    }

    void erase_all(const T& data){
      Node* n=find_front(data);
      while(n!=NULL){
	Node* next=find_front(n->next);
	erase(n);
	n=next;
      }
    }

    void clear(){
      for (Node* n=first;n!=NULL;n=n->next){
	Node* next=n->next;
	delete n;
	n=next;
      }
    }
    
    Node* insert(const Node* start,const Node* end){
      for (const Node* n=start;n!=end;n=n->next){
	push_back(n->data);
      }
      return last;
    };

    DlList& operator=(const DlList& l){
      insert(l.begin(),l.end());
      return *this;
    }
    
    /*std::pair<Node*,bool> push_once(const T& data){
      if ((Node* n=find_front(data)==NULL){
	
      }else{
	Node* nn=new Node(data);
	push_back(n);
	return make_pair(n,true);
      }
      
      }*/

  };



}//helium


#endif
