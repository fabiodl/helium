#ifndef HE_STEREOBLOBCLIENT
#define HE_STEREOBLOBCLIENT

#include <helium/system/dynamicClient.h>
#include <helium/cv/stereoBlob.h>

namespace helium{

  class StereoBlobClient:public DynamicClient{
  public: 
    ValueProxy<RefSign<BlobInfo> > r,l;
    
    StereoBlobClient(){
      dexport(mem,r,RIGHTBLOB);
      dexport(mem,l,LEFTBLOB);
    }
    


  };

}

#endif
