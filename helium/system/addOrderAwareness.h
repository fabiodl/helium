#ifndef HE_ADDORDERAWARENESS
#define HE_ADDORDERAWARENESS


namespace helium{
  
  template<typename VP> class AddOrderAwareness:public VP{
    Mutex valLock;
    std::vector<VP::V::ST> vals;
    
    void set(const typename VP::V::ST& s){
      bool wasFirst=vals.size()==0;
      vals.push_back(s);
      if (wasFirst){
	while(vals.size())
	  VP::set(s);
      }
      
    }

    
  }


}




#endif
