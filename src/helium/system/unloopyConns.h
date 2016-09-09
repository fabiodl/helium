#ifndef HE_UNLOOPYCONNS
#define HE_UNLOOPYCONNS

#include <helium/system/valueHub.h>
namespace helium{


  template<typename T>
  class UnloopyConnReference:public Callback<T>,public ConnReference<T>{
    ValueHub<T>& destination;
    UnloopyConnReference& nastyFriend;
  public:
    UnloopyConnReference(ValueHub<T>& pdestination,  UnloopyConnReference& pnastyFriend):
      ConnReference<T>(this),destination(pdestination),nastyFriend(pnastyFriend){    
    }
    
    void operator()(T n){
      destination.assureExcept(n,nastyFriend);    
    }

  };//UnloopyConnReference


  template<typename T>
  struct UnloopyConnReferences{  
    UnloopyConnReference<T> firstConn;
    UnloopyConnReference<T> secondConn;

    UnloopyConnReferences(ValueHub<T>& firstDest,ValueHub<T>& secondDest):
      firstConn(firstDest,secondConn),
      secondConn(secondDest,firstConn){    
    }

    UnloopyConnReferences(std::pair<ValueHub<T>*,ValueHub<T>*> dests):
      firstConn(*dests.first,secondConn),
      secondConn(*dests.second,firstConn){    
    }
  
  };//UnloopyConnReferences


};


#endif
