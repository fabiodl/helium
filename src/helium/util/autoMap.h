#ifndef _HE_AUTOMAP_
#define _HE_AUTOMAP_

#include <map>


namespace helium{



template<typename K,typename V> V* autoInsertPtr(std::map<K,V>& m,const K &k){
  std::map<K,V>::iterator it=m.find(k);
  if (it==std::map<K,V>::end){
    V* v=new V(k);
    m.insert(std::make_pair(k,v));
    return v;
  }
  return it->second;
}



template<typename K,typename V> V* autoInsertPtrParam(std::map<K,V>& m,const K &k){
  std::map<K,V>::iterator it=m.find(k);
  if (it==std::map<K,V>::end){
    V* v=new V(k);
    m.insert(std::make_pair(k,v));
    return v;
  }
  return it->second;
}


}
#endif
