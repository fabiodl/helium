#include <helium/device/sonyJog.h>
#include <helium/itaro/rototra.h>
#include <helium/itaro/rototraToMotors.h>
namespace helium{
  
  class JogControl{
    
    SonyJog sj;
    SonyJog::StickFilter stickx,sticky;
    
    
    RotoTranslation yaw,pitch,roll;
    double h;
    double mot[4];
    
    Mutex access;
    
    void update(){
      access.lock();
      RotoTranslation r,rtmp;
      mul(rtmp,pitch,roll);
      mul(r,yaw,rtmp);
      r[RT_TZ]=h;
      rototraToMotors(mot,r);    
      access.unlock();
    };

    
    void changeRoll(double v){
      setRotation(roll,Axis::X,M_PI/6*v/6);
      update();
    }
    
    void changePitch(double v){
      setRotation(pitch,Axis::Y,M_PI/6*-v/6);
      update();
    }
    
    
    void changeYaw(int v){
      setRotation(yaw,Axis::Z,M_PI/2*-v/8);
      update();
    }
    
  void changeh(int x){
    h=limit(-0.01,0.01,h+ 0.01/5*x);
    update();
  }
    
    
    StaticConnReference<double,JogControl,double,&JogControl::changeRoll> rollconn;
    StaticConnReference<double,JogControl,double,&JogControl::changePitch> pitchconn;
    StaticConnReference<int,JogControl,int,&JogControl::changeYaw> yawconn;
    StaticConnReference<int,JogControl,int,&JogControl::changeh> hconn;
    
  public:  

    void execute(){
      sj.execute();
    }
    
    void start(){
      sj.start();
    }
    
    JogControl(const char* dev):sj(dev),h(0),access("acc"),
				rollconn(this),pitchconn(this),
				yawconn(this),hconn(this)
  {    
    sj.stickx.connect(stickx);
    sj.sticky.connect(sticky);
    eye(roll);
    eye(pitch);
    eye(roll);
    stickx.value.connect(rollconn);
    sticky.value.connect(pitchconn);
    sj.jog.connect(yawconn);
    sj.wheel.connect(hconn);
  }
    
    void get(double* m){
      access.lock();
      assignn<4>(m,mot);
      access.unlock();
    }
    
  
  };
  
  
}
