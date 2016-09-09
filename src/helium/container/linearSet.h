#ifndef HE_LINEARSET
#define HE_LINEARSET



class LinearSet{
  
  std::list<int> l;
  std::list<int>::iterator* acc;
  
  LinearSet(int n){
    acc=new std::list<int>::iterator[n];
  }

  ~LinearSet(){
    delete[] acc;
  }

  void insert(int n){
  }

  size_t size(){
    return l.size;
  }

};


#endif
