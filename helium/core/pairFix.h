#ifndef HE_PAIR
#define HE_PAIR

namespace helium{
  

  template<typename F,typename S>
  struct pair{
    F& first;
    S& second;
    pair(const F& f,const S& s):first(f),second(s){}
    pair(){}

    void operator=(const pair& p){
    }
  };


  template<typename F,typename S>
  struct pair<F&,S>{
    F& first;
    S& second;
    pair(F& f,S& s):first(f),second(s){}
    pair(){}
  };

  template<typename F,typename S>
  struct pair<F,S&>{
    F& first;
    S& second;
    pair(F& f,S& s):first(f),second(s){}
    pair(){}
  };

  template<typename F,typename S>
  struct pair<F&,S&>{
    F& first;
    S& second;
    pair(F& f,S& s):first(f),second(s){}
    pair(){}
  };

}



#endif
