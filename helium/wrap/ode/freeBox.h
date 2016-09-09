#ifndef HE_FREEBOX
#define HE_FREEBOX

#include <helium/wrap/ode/odeStructure.h>


namespace helium{

  
  class FreeBox{
    Vector3d size;
    Appearance app;
    Element e;
    FreeObject obj;    
  public:    
    FreeBox(const Vector3d& _size=Vector3d::ZERO,const Appearance& _app=Appearance(1,1,1)):
      size(_size),
      app(_app),
      e(&Vector3d::ZERO,&size,0,&app,ENCUMBRANCEINFO,BOXSHAPE),
      obj(&e,true){
    }

    operator FreeObject*(){
      return &obj;

    }
    void setPosition(const Vector3d& p){
      obj.setPosition(p); 
    }

   
    
  };

}



#endif
