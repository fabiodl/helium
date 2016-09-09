#include <helium/debug/lifeTime.h>

namespace helium{
  
  LifeTime::LifeTime(const std::string& ps):s(ps+toString(rand())),zeroT(getSystemMillis()){
    std::cout<<"{"<<s<<"["<<std::endl;
  }

  LifeTime::~LifeTime(){
    std::cout<<"]"<<s<<"} (destroyed  after "<<getElapsedMillis(zeroT)<<")"<<std::endl;
    
  }


}
