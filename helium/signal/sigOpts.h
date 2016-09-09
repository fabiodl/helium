#ifndef HE_SIGOPTS
#define HE_SIGOPTS

#include <helium/callback/tsSignal.h>

namespace helium{
  



  namespace sigOpts{
    enum{THREADSAFE=1,EXCHANDLING=2};
    
    template <int flags,bool TS=flags&THREADSAFE,bool EH=flags&EXCHANDLING> struct sig{
      
    };


    
    
  };
  


};




#endif
