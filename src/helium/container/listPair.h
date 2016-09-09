#ifndef HE_LISTPAIR
#define HE_LISTPAIR

namespace helium{
template <typename L1,typename L2> class ListPair:public L1,public L2{
  
  typedef typename L1::value_type T1;
  typedef typename L2::value_type T2;
  typedef typename L1::Node N1;
  typedef typename L2::Node N2;

public:  
  class Node:public N1,public N2{
    Node(const T1& t1,const T2& t2):N1(t1),N2(t2){
    }
    typename N1::value_type& first(){
      return N1::data;
    }

    typename N2::value_type& second(){
      return N2::data;
    }

  };
  

  Node* push_back(const T1& t1, const T2& t2){
    Node* n=new Node(t1,t2);
    L1::push_back(n);
    L2::push_back(n);
    return n;
  }

  void erase(Node* n){
    L1::erase(n);
    L2::erase(n);
  }

  std::pair<Node*,bool> push_once_first(const T1& t1,const T2& t2){
    Node* n=static_cast<Node*>(L1::find_front(t1));
    if (n!=NULL){
      return make_pair(n,false);
    }else{
      return make_pair(push_back(t1,t2),true);
    }
  }

  bool isEmpty(){
      return L1::isEmpty();
   }

};

}//helium

#endif
