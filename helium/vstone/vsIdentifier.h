#ifndef HE_VSIDENTIFIER
#define HE_VSIDENTIFIER

#include <helium/os/serial.h>
#include <helium/core/enum.h>

namespace helium{
  class VsIdentifier{
    Serial& s;
    
  public:
    enum HwType{VSSV410,VSSS110,VSSV3310,UNSPECIFIED,VSSS110Kindy};
    enum Protocol{VSDEFAULT,VSCRC};

    struct Info{
      HwType hw;
      Protocol protocol;    
      int fwVersion;
    };

    
    VsIdentifier(Serial& s);
    Info getInfo(int id);    
  };

  

  template<>  struct EnumStrings<VsIdentifier::HwType>{
    static const int NUM=5;
    static const char* strings[NUM];       
  };

  template<>  struct EnumStrings<VsIdentifier::Protocol>{
    static const int NUM=2;
    static const char* strings[NUM];       
  };



}




#endif
