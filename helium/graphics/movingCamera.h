#ifndef HE_MOVINGCAMERA_
#define HE_MOVINGCAMERA_
#include <helium/math/algebra.h>
#include <helium/graphics/glUtil.h>

namespace helium{
class MovingCamera{   
  //tmp vars
  Vector3d update;
  Vector3d para;
  Vector3d dir;

  double stdz,radius;
  Vector3d z;
  double targetSpeed,radiusSpeed,rotationSpeed;

  Vector3d ctr;//camera translation
  Vector3d target; //low pass filtered target
 
 public:
 MovingCamera():stdz(0.3),radius(0.8),z(0,0,1),targetSpeed(0.001),radiusSpeed(0.001),rotationSpeed(0.001),ctr(0,-1,stdz),target(0,0,0){    
  }

  void setDefaultCameraHeight(double h){
    stdz=h;
  }


  void setSpeeds(double target,double radius,double rotation){
    targetSpeed=target;
    radiusSpeed=radius;
    rotationSpeed=rotation;
  }

  void setCamera(const double* view){
    double rc[16];
    invert(rc,view);
    //print(view);
    getTranslation(&ctr,rc);    
    getRow(&dir,view,2);    
    dir.mulScalar(radius);
    target.sub(ctr,dir);
    //std::cout<<"reconstructed target"<<target<<" and camera "<<ctr<<std::endl;
  }

  void getCamera(double* view,const double* rtrp){
    Vector3d rtr(rtrp);
    update.sub(rtr,target);
    update.mulScalar(targetSpeed);
    target.sum(update);

    
    update.sub(ctr,rtr);
    update.normalize();
    update.mulScalar(radius);
    update.sum(rtr);
    update.z()=stdz;
    update.sub(ctr);
    update.mulScalar(radiusSpeed);

    dir.sub(ctr,rtr);
    para.cross(dir,z);
    //std::cout<<para.norm()<<" "<<std::endl;
    if (para.norm()>1E-2){

      //para.normalize();
      para.mulScalar(rotationSpeed);
    }else{
      para.set(1E-2,0,0);
    }
    //std::cout<<para<<" "<<pn<<std::endl;
    update.sum(para);
    ctr.sum(update);
    //std::cout<<"using target"<<target<<"and camera"<<ctr<<std::endl;
    getLookAt(view,target,ctr); 
    //std::cout<<"determinant"<<getDeterminant(view)<<std::endl;
    //print(view);
  }
  
};
}//helium
#endif
