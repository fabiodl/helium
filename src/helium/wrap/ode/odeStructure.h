/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 */

#ifndef HE_ODESTUCTURE_
#define HE_ODESTUCTURE_

#include <helium/robot/motorInterface.h>
#include <helium/simul/structure.h>
#include <helium/thread/thread.h>
#include <helium/thread/mutex.h>
#include <helium/os/time.h>
#include <ode/ode.h>
#include <map>

namespace helium{

class ODEPart{
  friend class ODERobot;
 private:
  int id;
  double totalMass;
 public:
  dBodyID body;
  Vector3d bodyCenter; //because body 0,0,0 is shifted to the center of the mass
  dMass m;
  std::vector<dGeomID> geoms;
  std::vector<Appearance *> apps;  
  std::vector<Element *>srcElems;
  class Part * srcPart;
  ODEPart();
  ~ODEPart();
  ODEPart(dBodyID pbody,Part * psrcPart):body(pbody),srcPart(psrcPart){};
  void addGeom(dGeomID gid,Appearance* a,const Element* srcElement);
  void translate(dReal tx,dReal ty,dReal tz);
  void getPosition(double* pos);
  void getRotation(double* m);
  void setPosition(dReal* p);
  void setRotation(dMatrix3 r);
  void getRototranslation(double* m);
  void setRototranslation(double* m);
  void stop();
  void getAbsoluteLinearVel(double* a);
  void setAbsoluteLinearVel(const double* a);  
  void getAbsoluteAngularVel(double* a);
  void setAbsoluteAngularVel(const double* a);  
  void getRelativeLinearVel(double* a);
  void getElementRelativeRototranslation(double* m,int elemID);
};


class ODEHinge{
 public: 
  dJointID hingeID;
  class SkeletonNode * srcSkeletonNode;
  dJointFeedback feedback;
  bool power;
  double getAngle();
  double getAngleRate();

 ODEHinge(dJointID phingeID,SkeletonNode * psrcSkeletonNode):hingeID(phingeID),srcSkeletonNode(psrcSkeletonNode),power(true){};
  void addTorque(double t);
};

class ResetElement{
  void ** addresses;
  void * srcStartAddress;
  int dsize;
  int n;
 public:
 ResetElement(std::vector<void *> paddresses,void * psrcStartAddress,int psize):
  srcStartAddress(psrcStartAddress),dsize(psize){
    n=(int)paddresses.size();
    addresses=new void*[n];
    for (int i=0;i<n;i++)
      addresses[i]=paddresses[i];
  }
  void reset(){
    for (int i=0;i<n;i++)
      memcpy(addresses[i],srcStartAddress,dsize);
  }
};


class ODERobot{
  friend class ODEWorld;  
 public:
  
  class Robot *srcRobot;
  
  Robot * getSrcRobot(){
    return srcRobot;
  }

  std::vector<ODEPart*> odeParts;
  //double* sensors;  
  ODERobot(class ODEWorld* odeworld,class Robot* r);        
  ~ODERobot();
  void updateMotors();
  void stepSensorReset();
  //using Motors::getPosition;
  //using Motors::rotate;
  //virtual 
  void rotate(int joint,double angle);
  //virtual 
  void rotate(const double * angles);
  void getPosition(double* angles); //returns the potentiometer value, "normal" ID
  double getPosition(int j);
  void getMotorTarget(double* angles);
  double getMotorTarget(int j);
  void translateRobot(double tx,double ty,double tz);
  void rotateRobot(double* m);
  void setRotatePivot(Part *p);
  void getPivotPosition(double* pos);
  void getPivotRototranslation(double* pos);
  void setPivotRototranslation(const double *m);

  double getMinZ();
  ODEWorld* world;
  ODEWorld* getWorld(){
    return world;
  }
  void resetMotors();
  void stopVelocity();
  void reset();
  ODEPart* pivot;
  ODEPart* getODEPart(int structureId);
  ODEHinge* getMotorHinge(int motorID);
  int getODEPartID(int structureID);
  void getCompleteRototrasl(std::vector<double*> &r);
  void setCompleteRototrasl(const std::vector<double*> cr);
  void getState(std::vector<double*> &r,std::vector<double*> &vels,std::vector<double*> &anglevels);
  void setState(const std::vector<double*> &r,const std::vector<double*> &vels,const std::vector<double*> &anglevels);
  void resetPosition();
  void storeInitialPosition();
  //using Motors::getPowerState;
  //using Motors::setPowerState;
  bool getPowerState(int j){return sortedMotors[j]->power;}
  void setPowerState(int j,bool state);
  void getEnergies(double* en);
  void getTorques(double* torques);//norm of the torque vector
  void getTorques(Vector3d* torques);//norm of the torque vector
  double getTorque(int j);
  double getMotorAxisTorque(int j); //projection over the motor axis of the torque vector
  void getMotorAxisTorques(double* torques);//projection over the motor axis of the torque vector
  double getTotalTorque();
  double getTotalEnergy();
  void coldRotate();
  int getMotorNum();
  bool isMotorWorking();
 private:  
  std::vector<ODEHinge*> motors;
  std::map<int,ODEHinge*> sortedMotors; //pointer given the motor ID
  std::vector<ResetElement*> resetElements;//used for resetting binary sensors
  std::vector<double*> initialPosition; //each element is rototransl matrix
};


class FreeObject{
 public:
  Appearance * app;
  dGeomID geom;
  bool solid;
 FreeObject(dGeomID g,Appearance* papp,bool so=true):app(papp),geom(g),solid(so){}
  FreeObject(Element* e,bool so=true);
  void setPosition(const Vector3d& pos);
};


enum RunState {RUNNING,STOPPING,STOPPED};


class ODEWorldCallback{
  friend class ODEWorld;
 protected:
  const int everyN;
 public:
 ODEWorldCallback(int n):everyN(n){
    //std::cout<<"OWC freq" <<n<<std::endl;
    if (n<=0)
      std::cerr<<"error in setting an ODEWorldCallback frequency"<<std::endl;
  }
  
  virtual void odeWorldCallback()=0;
  
};

  
  class ForceContact{
  public:
    dJointID joint;
    dJointFeedback* feedback;
    int body; //the body of the joint to be used, 0 or 1
    Vector3d contactPos;
    Vector3d* dstAppl;
  Vector3d* dstForceVector;
    double* dstForce;
    Vector3d normal;
  };
  
   class ForceCollisionManager{
  public:
    std::vector<ForceContact> contacts;
    void reset();
    void registerContact(dJointID c,dGeomID o,int body,dVector3 contact,dVector3 normal,dGeomID otherObject);
    void updateSensors();
         
   };//ForceCollisionManager

//TODO different collision spaces,make more stuff private
  class ODEWorld:public Thread{
  friend class ODERobot;
  std::vector<ODEWorldCallback*> callbacks;
  int stepCounter;
    friend void nearCallback(void *data, dGeomID o1, dGeomID o2);
    ForceCollisionManager forceCollisionManager;
 public:
  ODEWorld();
  RunState runstate;
  RunState getRunState(){return runstate;}
  void resetTime();
    ODERobot* addRobot(class Robot* r,double* rotoTra=NULL);  //the returned oderobot is atomatically deleted when the world is deleted
  void draw();
  void step(double dt,bool lock=true);
  void run(double dt);
  void run (double dt,int steps);
  void stop();
  void lock(){
    waiting=true;worldMutex.lock();
  }
  void unlock(){
    waiting=false;worldMutex.unlock();
  }
  void pause(bool p=true);
  virtual ~ODEWorld();
  bool canCollide(dGeomID a,dGeomID b);
  dWorldID world;
  dJointGroupID contactgroup;  
  std::vector<ODERobot*> robots;
  std::vector<FreeObject*> freeObjects;
  void addFreeObject(FreeObject* f){
    if (f->solid)
      dSpaceAdd(worldSpace,f->geom);
    freeObjects.push_back(f);
  }
  bool waiting;
  void reset();
  void letDraw(){
    while (waiting) milliSleep(1);    
  }
  double dt;  
  bool paused;
  dGeomID ground;
  double t;
  long timeZero; //for displaying the speed vs real time
  double erp,cfm,mu,mu2,bounce,bounce_vel;
  dSpaceID worldSpace;
  void setGravity(double g);

  void addCallback(ODEWorldCallback* cb){
    callbacks.push_back(cb);
  }

  void removeCallback(const ODEWorldCallback* cb){
    for (unsigned int i=0;i<callbacks.size();i++){
      if (callbacks[i]==cb){
	callbacks.erase(callbacks.begin()+i);
	break;
      }
    }
  }

  void updateCollMatrix();
 private:
    void run();
  bool* noCollParts;
  int noCollPartsSize;

  std::vector<ODEPart*> odeParts;
  friend class GeomInfo;
  int getPartWorldId(ODEPart* p);
  int getMaxPartId();

  void registerRobot(class ODERobot* r);

  CountMutex worldMutex;
};


class ODEWorldCamera{
 public:
  ODEWorld* world;
  bool avoidResizing;
  virtual void getCameraRototranslation(double* m){eye(m);};
 ODEWorldCamera():world(NULL),avoidResizing(true){};
};

}//helium

#endif
