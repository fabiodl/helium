/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_INDEXABLEPROXY
#define HE_INDEXABLEPROXY

#include <helium/vector/array.h>

namespace helium{

  template<typename A> struct IsResizable{
    static const bool value=true;
  };
  
  template<typename T,int N,typename A> struct IsResizable<Array<T,N,A> >{
    static const bool value=(N==array::RESIZABLE);

  };



  

  //the bool decides whether to add the resize member or no
  template <typename A,bool resizable=IsResizable<A>::value> 
  class IndexableProxy;

  template <typename A>
  class IndexableProxy<A,false>{

  public:
    A& t;
    IndexableProxy(A& pt):t(pt){
    }

    virtual NodeMapping& operator[](int i)=0;
    virtual A& getProxied(){
      return t;
    }
    virtual size_t size()=0;
  };

  template<typename A> 
  class IndexableProxy<A,true>{
  
  public:
    A& t;
    IndexableProxy(A& pt):t(pt){
    }
    
    virtual NodeMapping& operator[](int i)=0;
    virtual A& getProxied(){
      return t;
    }

    virtual size_t size()=0;
    virtual void resize(size_t n)=0;

  };

  template<typename A>
  struct IsResizable<IndexableProxy<A,false> >{
    static const bool value=false;
  };
 
  template<typename A>
  struct IsResizable<IndexableProxy<A,true> >{
    static const bool value=true;
  };
 


  template <typename T>
  struct realVector{
    typedef T value_type;
    static T& get(T& t){
      return t;
    }
  };

  template <typename A>
  struct realVector<IndexableProxy<A> >{
    typedef A value_type;
    static A& get(IndexableProxy<A>& a){
      return a.getProxied();
    }
  };
  




}//helium

#endif
