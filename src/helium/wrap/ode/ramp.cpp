#include <helium/wrap/ode/ramp.h>
#include <helium/wrap/ode/odeMath.h>


namespace helium{

Ramp::Ramp(double size,double angle,const char* textFilename):
      app(1,1,1),      
      gid(dCreateBox(0,size,size,0.1)),
      obj(gid,&app),
      texture(textFilename,1000,1000)
    {      
      app.texture=&texture;
      double ma[16];
      setAsRotation(ma,Axis::X,-M_PI/180*angle);
      ma[7]=-1;
      dMatrix3 R;
      matrixToODEMatrix(R,ma);
      dGeomSetRotation(gid,R); 
      dGeomSetPosition(gid,ma[3],ma[7],ma[11]);      
    }

}
