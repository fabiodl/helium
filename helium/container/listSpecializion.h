#ifndef HE_LISTSPECIALIZATION
#define HE_LISTSPECIALIZATION

namespace helium{
  
  template <typename L,typename A>
  class ListSpecialization:public L{
    typedef typename L::value_type value_type;
    typedef typename L::Node SNode;

    struct Node:public SNode{
      A info;
      Node(const SNode::value_type& t,const A& pinfo):SNode(t),info(pinfo){
      }
    };


    Node* push_back(const T& data,const Add& info){
      Node* n=new Node(data,info);
      L::push_back(n);
      return n;
    }

    std::pair<Node*,bool> push_once_data(const T& data,const A& info){
      SNode* sn=L::find_front(data);
      if (sn==NULL){
	Node* n=new Node(data,info);
	push_back
      }
    }
    
  };

}


#endif
