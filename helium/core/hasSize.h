/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_HASSIZE
#define HE_HASSIZE


namespace helium{
  template<typename T> struct hasSize:public T{
    
    template<typename X,size_t (X::*m)() const> struct dummy;
    
    template<typename X,typename Y>
    static tt::yes check(dummy<X,&X::size>*,Y);
    template<typename X>
    static tt::no check(...);  
    static const bool value= sizeof(check<T>(NULL,0))==sizeof(tt::yes);
  };
  
  template<typename T> struct hasSize<T*>{
    static const bool value=false;
  };

  template<typename T,int N> struct hasSize<T[N]>{
    static const bool value=false;
  };


}//ns hel


#endif
