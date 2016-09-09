#ifndef HE_SETTERCONNREFERENCE
#define HE_SETTERCONNREFERENCE

namespace helium{

  template<typename D,typename T=D>
  class SetterConnReference:public ConnReference<D>,public Callback<D>{
    T& t;
    void operator()(D d){
      t=d;
    }

  public:
    SetterConnReference(T& pt):ConnReference<D>(this),t(pt){
    }
    
  };



}




#endif
