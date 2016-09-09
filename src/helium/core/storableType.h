/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_STORABLE
#define HE_STORABLE

namespace helium{
  

  template<typename X> struct storableType{
    typedef X value;
  };


  template<typename X> struct storableType<X&>{
    typedef X value;
  };


  template<typename X> struct storableType<const X&>{
    typedef X value;
  };

  
  template<typename X> struct storableType<const X*>{
    //typedef X value; have an error,let the programmer decide
  };

  template<typename X> struct storableType<X*>{
    //typedef X value; have an error,let the programmer decide
  };
 

};



#endif
