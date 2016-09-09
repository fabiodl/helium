#ifndef HE_TYPELIST
#define HE_TYPELIST

#include <helium/core/isSame.h>


/*usage example::

enum SType{At,Bt,Ct};

class A{};

class B{};

class C{};


template<> struct getClass<SType,At>{
  typedef A value;
};

template<> struct getClass<SType,Bt>{
  typedef B value;
};

template<> struct getClass<SType,Ct>{
  typedef C value;
};

std::cout<<"type of B is "<<getTypeId<SType,C>::value<<std::endl;

*/


namespace helium{

  template<typename SubType,SubType type> struct getClass{
  };
  
 
  template<typename SubType,typename searchClass,
	   SubType searchType=SubType(0),
	   bool found=isSame<searchClass,typename getClass<SubType,searchType>::value>::value > struct getTypeId{
	 };


  template<typename SubType,typename searchClass,
	   SubType searchType
	   > struct getTypeId< SubType,searchClass,searchType,true>{
    static const SubType value=searchType;
  };
  
  
  template<typename SubType,typename searchClass,
	   SubType searchType
	   > struct getTypeId< SubType,searchClass,searchType,false>{
    static const SubType value=getTypeId<SubType,searchClass,(SubType)(searchType+1)>::value;
    
  };


}




#endif
