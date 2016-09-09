#ifndef HE_STRUCTURE_
#define HE_STRUCTURE_

#include <vector>
#include <helium/vector/array.h>
#include <helium/math/algebra.h>
#include <helium/simul/simulMath.h>

namespace helium{

enum TypeFlags{ENCUMBRANCEINFO=1,WEIGHTINFO=2};

class GlTexture;

class Appearance{
 public:
  float r,g,b,alpha;
  GlTexture* texture;
  bool useTexture;  
 Appearance():r(0),g(0),b(0),alpha(1),texture(NULL),useTexture(true){}
 Appearance(float pr,float pg,float pb):r(pr),g(pg),b(pb),alpha(1),texture(NULL),useTexture(true){};
  void setColor(float pr,float pg,float pb){r=pr;g=pg;b=pb;}
};

class MouseSensorBoxCustomization{
 public:
  double* mouseActivation[6];
  int id[6];
  MouseSensorBoxCustomization(int startID,double * startLocation);
  bool active[6]; //tells if the sensor should be enabled or no. by default,active is true
};

class BinarySensorBoxCustomization{
 public:
  bool* contactActivation[6];
  int id[6];
  BinarySensorBoxCustomization(int startID,bool * startLocation);
};

class ForceSensorBoxCustomization{
 public:
  int id[6];
  Vector3d* applicationPoint[6];
  Vector3d* forceVector[6];
  double* force[6];
  ForceSensorBoxCustomization(int startID,Vector3d * applicPoint,Vector3d* fVector,double*force,bool* withGround);
  bool* withGround[6];
};
  
  
class BinarySensorCustomization{
 public:
  bool* colliding;
 BinarySensorCustomization(bool *c):colliding(c){};
};

enum ElementShape {BOXSHAPE,SPHERESHAPE,CAPPEDCYLINDERSHAPE};

enum ElementCustomizationType{NULLCUSTOMIZATION=0,MOUSESENSORBOXCUSTOMIZATION=0,BINARYSENSORCUSTOMIZATION,BINARYSENSORBOXCUSTOMIZATION,FORCESENSORBOXCUSTOMIZATION};

const int CUSTOMIZATIONNUM=4;


class Part;

class Element{
  friend class Part;
 private:
  Part* ownerPart;
 public:
  int id;
  const Vector3d *size;
  int type;
  double m;
  ElementShape shape;
  void* elementCustomizations[CUSTOMIZATIONNUM];
  double rotoTransl[16];
  int customizationType;
  class Appearance * app;
  void setElementCustomization(void * custom,ElementCustomizationType custType){
    customizationType|=(1<<custType);
    elementCustomizations[custType]=custom;
  }

  bool hasCustomization(ElementCustomizationType custType) const{
    return (customizationType&(1<<custType))!=0;
  }
 Element(const Vector3d* ppos,const Vector3d* psize,double pm,class Appearance* papp,int flags,ElementShape bs=BOXSHAPE):
  ownerPart(NULL),id(-1),size(psize),type(flags),m(pm),shape(bs),customizationType(NULLCUSTOMIZATION),app(papp){
    eye(rotoTransl);
    if (ppos!=NULL) setTranslation(rotoTransl,ppos);
    for (int i=0;i<CUSTOMIZATIONNUM;i++)
      elementCustomizations[i]=NULL;      
  };  
 Element():ownerPart(NULL),id(-1),size(NULL),type(0),m(-1),shape(BOXSHAPE),customizationType(NULLCUSTOMIZATION),app(NULL){
    eye(rotoTransl);
    for (int i=0;i<CUSTOMIZATIONNUM;i++)elementCustomizations[i]=NULL;  
  };  
  void set(Vector3d* ppos,Vector3d* psize,double pm,class Appearance* papp,int flags){
    setTranslation(rotoTransl,ppos);size=psize;m=pm;app=papp;type=flags;
  }    
};


class Part{  
 public:
  int id;
  void addElement(class Element* b);
  std::vector<const class Element*> elements;  
};


  enum ConnectionType {FREEHINGE,FREESLIDER,SERVOMOTOR,REFERENCEROTATION};


class ServoParam{
 public:
  double motorAngleLimit;
  double motorMaxV;
  double motorCoeff;
  double motorMaxForce;
 ServoParam(double alimit,double maxv,double coeff,double maxf):motorAngleLimit(alimit),motorMaxV(maxv),motorCoeff(coeff),motorMaxForce(maxf){}
};


class SkeletonNode{
  typedef Axis::Type TurnAxis;
  public:
  int id;
  int motId;
  Vector3d* pos;
  enum ConnectionType type;
  TurnAxis axis;
  double* servoValue; 
  double* fakeServo;
  ServoParam* servoParam;
  double multiplier;
  double offset;  
  SkeletonNode* ref;
  class Part* part;
 SkeletonNode():motId(-1),pos(NULL),servoValue(NULL),fakeServo(NULL),servoParam(NULL),multiplier(NAN),offset(NAN),ref(NULL),part(NULL){};

 SkeletonNode(Vector3d* ppos,enum ConnectionType ptype,TurnAxis paxis,
	      double* pservoValue,double pmultiplier,double poffset,class SkeletonNode* pref,class Part* ppart):
  motId(-1),pos(ppos),type(ptype),axis(paxis),servoValue(pservoValue),fakeServo(NULL),servoParam(NULL),multiplier(pmultiplier),offset(poffset),
    ref(pref),part(ppart){};

  SkeletonNode(Vector3d* ppos,enum ConnectionType ptype,TurnAxis paxis,
	       double* pservoValue,double pmultiplier,double poffset,class SkeletonNode* pref):
  motId(-1),pos(ppos),type(ptype),axis(paxis),servoValue(pservoValue),fakeServo(NULL),servoParam(NULL),multiplier(pmultiplier),offset(poffset),
    ref(pref),part(NULL){};

  void set(Vector3d* ppos,enum ConnectionType ptype,TurnAxis paxis,
	   double* pservoValue,double pmultiplier,double poffset,class SkeletonNode* pref,class Part* ppart =NULL){
    pos=ppos;type=ptype;axis=paxis;servoValue=pservoValue;multiplier=pmultiplier;offset=poffset;ref=pref;part=ppart;
  }
  void setFakeServo(double* fakeMotor){fakeServo=fakeMotor;}
  void getLocalRototranslation(double *m) const;
  //void getLocalRototranslationDerivative(double *m) const;
  void getAbsoluteRototranslation(double *m) const;
  void getAbsoluteCoordinate(Vector3d* r,Vector3d* pos);
  void getMotorChain(int* num,double** motors);
  int getMotorChain(int* ids,double* values); //return the numbers of ids
  void setMotId(int sid){
    motId=sid;
  }
  void setServoParam(ServoParam* sp){
    servoParam=sp; 
  }
};

class Robot{
 public:
 Robot():pivot(NULL){};
  Part* pivot;   
  void addSkeletonNode(const class SkeletonNode* s);
  void disableCollDet(const class Part* i,const class Part* j);
  std::vector<const SkeletonNode*> skeletonNodes;
  std::vector<const Part*> noCollParts;
  void printRobot();
  int getNodesNum();
  int getPartsNum();  
  int getElementsNum();
  const Element* getElement(int elementID);
  int getMaxNodeID();
  int getMaxPartID();
  int getMaxElementID();
  void describe(std::ostream& out);  
  virtual std::string getJointName(int joint) const;
  virtual std::string getSensorName(int joint) const;
  SkeletonNode* getSkeletonNode(int motID);
  void setServoParam(ServoParam* sp);
};


class Cog{
  Vector3d * mPos;
  const Robot* r;
  double (*absm)[16];
  double* m;
 public:
  Cog(Robot* r);  
  virtual ~Cog();
  void get(Vector3d& c);  
};


class SensorToElement{
 public:
  class SensorInfo{
  public:
    const SkeletonNode* sn;
    const Element* el;    
    double vtx[24];
  };

  Array<SensorInfo> info;
  SensorToElement(Robot* r);
  
};

}//helium

#endif

