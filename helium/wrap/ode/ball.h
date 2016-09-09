#ifndef HE_BALL
#define HE_BALL

namespace helium{

  class Ball{
    Appearance app;
    Vector3d pos;
    Vector3d size;
    Element e;
  public:   
    Ball(double s):app(1,0.5,0),pos(0,0,0),size(s,0,0),
		   e(&pos,&size,0.07,&app,ENCUMBRANCEINFO|WEIGHTINFO,SPHERESHAPE){
    }   

    operator Element*(){ return &e;}
  };
  

}



#endif
