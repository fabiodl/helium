#ifndef HE_CELLCONNREFERENCE
#define HE_CELLCONNREFERENCE

#include <helium/core/isBaseOf.h>
#include <helium/core/selectByValue.h>
#include <helium/system/cell.h>
#include <helium/system/cellSignature.h>
#include <helium/callback/callback.h>

namespace helium{

  template<typename SignOrUT,typename C,typename P,void (C::*f)(P),typename UT=
	   selectFirstOnTrue<typename SignOrUT::UT,
			     SignOrUT,			     
			     is_base_of<CellSignature,SignOrUT>::value>::value>

  class CellConnReference:public Callback<Cell::CellSignalData>,
			  public ConnReference<Cell::CellSignalData>{

    
    C* c;
  public:
    CellConnReference(C* pc):c(pc){
    }
    
    void operator()(Cell::CellSignalData p){
      (c->*f)((UT*)(p));
    }
    
  }

  
};



#endif
