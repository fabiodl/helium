/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_TRIPLE
#define HE_TRIPLE

namespace helium{

template<typename A,typename B,typename C> struct triple{
  A first;
  B second;
  C third;
 
  triple(){
  }

  triple(const A& a,const B& b,const C& c):first(a),second(b),third(c){
  }

};

  template <typename A,typename B,typename C> triple<A,B,C> make_triple(const A& a,const B& b,const C&c){
    return triple<A,B,C>(a,b,c);
  }

  


}//helium


#endif
