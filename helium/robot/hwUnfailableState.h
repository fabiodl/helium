#ifndef HE_HWUNFAILABLESTATE
#define HE_HWUNFAILABLESTATE

#include <helium/robot/hwStateMonitor.h>

namespace helium{


  class UnfailableGroupState:public TypedCell<HwStateGroupSignature>{
    Array<HWState> allOk;

    
    void getData(Array<HwState>& s){
      s.resize(allOk.size());
      s=allOk;
    }


    void getData(Array<HwState>& s,HsIdList& l){
      s.resize(l.size());
      s=allOk;
    }

    
    UnfailableGroupState(int n):allOk(n){
      setAll(allOk,
    }
    
    
  };
  
  
  HwStateGroupSig

}

#endif
