#ifndef HE_LESSOPERATOR
#define HE_LESSOPERATOR

namespace helium{

  template <typename A,typename B> bool lessOp(const A& a1,
						     const A& a2,						     
						     const B& b1,
						     const B & b2
						     ){
    if (a1<a2) return true;
    if (a1>a2) return false;
    return b1<b2;
  }


  template <typename A,typename B,typename C> bool lessOp(const A& a1,
						const A& a2,						     
						const B& b1,
						const B & b2,

						const C& c1,
						const C& c2


						){
    if (a1<a2) return true;
    if (a1>a2) return false;
    if (b1<b2) return true;
    if (b1>b2) return false;
    return c1<c2;
  }
  


}
#endif
