#ifndef _SLLIST_
#define _SLLIST_

template<typename T>
class SLList<T>{
  class SLNode<T>{
    T data;
    SLNode* next;
  };

  SLNode* last;
  SLNode* first;
  
  LWmutex mutex;

  void push_back(const &T data){
  }
}


#endif
