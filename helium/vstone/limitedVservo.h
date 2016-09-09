#ifndef HE_LIMITEDVSERVO
#define HE_LIMITEDVSERVO

#include <helium/robot/motorStructures.h>
#include <helium/vstone/vServo.h>

namespace helium{
 
  class LimitedVservo:public HW{
    PosLimiter<int,int,Vservo::VservoConsts> limiter;
    
    void rawRotate(int j,typename motor::Raw<int,int>::Position val){
      std::cout<<"limiting "<<val<<"@"<<j<<std::endl;
      limiter.limit(val,j);
      HW::rawRotate(j,val);
    }

    motor::IfaceId getIfaceId(){
      return "limited"+HW::getIfaceId();      
    }

    using HW::rawRotate;
  public:
    
    LimitedVservo(motor::Calib<PS,Consts>& calib):limiter(calib){
    }
  };

}

#endif
