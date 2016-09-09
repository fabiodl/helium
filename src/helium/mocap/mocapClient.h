#include <helium/system/dynamicClient.h>
#include <helium/mocap/mocap.h>

namespace helium{
  class MocapClient:public DynamicClient{
  public:
    ValueProxy<SingleBodySignature> body;
    ValueProxy<UnrecBodySignature> unrec;
    ValueProxy<WholeMarkerSignature> whole;
    
    MocapClient(){
      dexport(mem,body,BODIESLOCATION);
      dexport(mem,unrec,UNRECOGNIZEDLOCATION);
      dexport(mem,whole,WHOLELOCATION);
    }        
  };


}
