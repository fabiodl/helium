#ifndef HE_RAMP
#define HE_RAMP

#include <helium/wrap/ode/odeStructure.h>
#include <helium/graphics/ppm.h>

namespace helium{

  
  class Ramp{
    Appearance app;
    dGeomID gid;
    FreeObject obj;
    PpmTexture texture;
  public:    
    Ramp(double size,double angle,const char* textFilename="textures/ground.ppm");

    operator FreeObject*(){
      return &obj;
    }
       
};

}


#endif
