/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 */


#include <algorithm>
#include <helium/wrap/ode/odeStructure.h>
#include <helium/wrap/ode/odeMath.h>
#include <helium/destroy/autoDestroy.h>


using namespace std;

#define MOTORANGLELIMIT (M_PI*(1-1E-3))
#define MOTORMAXV       7.18
#define MOTORCOEFF   15
static const dReal MOTMAXFORCE=41*9.81/100;

#define CONTACTPOINTS 10

#define SHOWCOLLISIONS
#undef SHOWCOLLISIONS


namespace helium{
class GeomInfo{
public:
  Element* srcElement;
  ODEPart* odePart; 
  int worldPartID;
  GeomInfo(Element *e,ODEPart* p,ODEWorld* w):srcElement(e),odePart(p){worldPartID=w->getPartWorldId(p);};
};

 
  
  
  inline bool operator<(const ForceContact&a,const ForceContact b){
    return a.dstAppl<b.dstAppl;
  }


  void getAbsoluteRototrasl(double* roto,dGeomID g){
  const dReal * pos=dGeomGetPosition(g);
  const dReal *R=dGeomGetRotation(g);  
  if (dGeomGetClass(g)==dGeomTransformClass){
    dGeomID g2 = dGeomTransformGetGeom (g);
    const dReal *pos2 = dGeomGetPosition (g2);
    const dReal *R2 = dGeomGetRotation (g2);
    dVector3 actual_pos;
    dMatrix3 actual_R;
    dMULTIPLY0_331 (actual_pos,R,pos2);
    actual_pos[0] += pos[0];
    actual_pos[1] += pos[1];
    actual_pos[2] += pos[2];
    dMULTIPLY0_333 (actual_R,R,R2);
    odeRotationToMatrix(roto,actual_R,actual_pos[0],actual_pos[1],actual_pos[2]);
  }else{
    odeRotationToMatrix(roto,R,pos[0],pos[1],pos[2]);
  }
}


 


   void ForceCollisionManager::reset(){
    for (unsigned int i=0;i<contacts.size();i++){
      contacts[i].dstAppl->zero();
      contacts[i].dstForceVector->zero();    
      *contacts[i].dstForce=0;
      delete contacts[i].feedback;
    }
    contacts.clear();    
  }

  void ForceCollisionManager::registerContact(dJointID c,dGeomID o,int body,dVector3 contact,dVector3 normal,dGeomID otherObject){
    GeomInfo* gi=(GeomInfo*)dGeomGetData(o);
    if (gi==NULL||!(gi->srcElement->hasCustomization(FORCESENSORBOXCUSTOMIZATION))) return;
    ForceContact temp;
    Vector3d normV;
    normV.set(normal[0],normal[1],normal[2]);
    if (body==1) normV.negate(); 
    double roto[16];
    getAbsoluteRototrasl(roto,o);
    int f=normalFace(normV,roto);
    temp.joint=c;
    temp.feedback=new dJointFeedback;
    dJointSetFeedback(c,temp.feedback);
    temp.body=body;
    temp.contactPos.set(contact[0],contact[1],contact[2]);    
    temp.normal=normV;
  
    temp.dstAppl=((ForceSensorBoxCustomization*)gi->srcElement->
		  elementCustomizations[FORCESENSORBOXCUSTOMIZATION])->applicationPoint[f];
    temp.dstForceVector=((ForceSensorBoxCustomization*)gi->srcElement->
		   elementCustomizations[FORCESENSORBOXCUSTOMIZATION])->forceVector[f];      
    temp.dstForce=((ForceSensorBoxCustomization*)gi->srcElement->
		   elementCustomizations[FORCESENSORBOXCUSTOMIZATION])->force[f];      
    *(((ForceSensorBoxCustomization*)gi->srcElement->
       elementCustomizations[FORCESENSORBOXCUSTOMIZATION])->withGround[f])=(dGeomGetData(otherObject)==NULL);
    contacts.push_back(temp);
  }//registerContact


  void ForceCollisionManager::updateSensors(){
    if (contacts.size()==0) {
      return;
    }
    static Vector3d forceVector;
    double sum=0;
    double norm;

    sort(contacts.begin(),contacts.end());
    int n=(int)contacts.size();
    //several contacts can belong to the same element.
    //we average their positions and sum their forces
    for (int i=0;i<n;i++){
      dJointFeedback* feedback=dJointGetFeedback(contacts[i].joint);
      forceVector.set(contacts[i].body==0?feedback->f1:feedback->f2);
      contacts[i].dstForceVector->sum(forceVector);
      norm=forceVector.norm();
      contacts[i].contactPos.mulScalar(norm);
      contacts[i].dstAppl->sum(contacts[i].contactPos);	
      sum+=norm;
      if (i==n-1||contacts[i].dstAppl!=contacts[i+1].dstAppl){	
	if (sum) contacts[i].dstAppl->divScalar(sum);
	sum=0;
      }
      *contacts[i].dstForce+=contacts[i].dstForceVector->dot(contacts[i].normal);
    }//for    
  }//updateSensors  

void ODEPart::addGeom(dGeomID gid,Appearance* a,const Element *srcElement){
  geoms.push_back(gid);
  apps.push_back(a);
  srcElems.push_back((Element *)srcElement);
}


void ODEPart::translate(dReal tx,dReal ty,dReal tz){
  dReal pos[3];
  const dReal* apos=dBodyGetPosition(body);
  pos[0]=apos[0]+tx;
  pos[1]=apos[1]+ty;
  pos[2]=apos[2]+tz;
  dBodySetPosition(body,pos[0],pos[1],pos[2]);  
}


void ODEPart::getPosition(double* pos){
  const dReal* apos=dBodyGetPosition(body);
  for (int i=0;i<3;i++)
    pos[i]=apos[i];
}


void ODEPart::getRotation(double* rot){
  const dReal* br=dBodyGetRotation(body);
  odeRotationToMatrix(rot,br);
}


void ODEPart::setPosition(dReal* pos){
  dBodySetPosition(body,pos[0],pos[1],pos[2]);
}


void ODEPart::setRotation(dMatrix3 rot){
  dBodySetRotation(body,rot);
}


void ODEPart::stop(){
  dBodySetLinearVel (body,0,0,0);
  dBodySetAngularVel(body,0,0,0);
}


void ODEPart::getRototranslation(double* rot){
  const dReal* br=dBodyGetRotation(body);
  const dReal* pos=dBodyGetPosition(body);  
  odeRotationToMatrix(rot,br,pos[0],pos[1],pos[2]);  
}


void ODEPart::getElementRelativeRototranslation(double* rot,int elemID){
  int gid=-1;
  for (unsigned int g=0;g<srcElems.size();g++){
    if (srcElems[g]->id==elemID){
      gid=g;
      break;
    }
  }
  if (gid==-1){
    for (int i=0;i<16;i++)
      rot[i]=NAN;
    return;
  }
  dGeomID g=dGeomTransformGetGeom(geoms[gid]);
  const dReal* br=dGeomGetRotation(g); 
  const dReal* pos=dGeomGetPosition(g); 
  odeRotationToMatrix(rot,br,pos[0],pos[1],pos[2]);  
}


void ODEPart::setRototranslation(double* rot){
  dMatrix3 R;
  matrixToODEMatrix(R,rot);
  dBodySetRotation(body,R);
  dBodySetPosition(body,rot[3],rot[7],rot[11]);
}


void ODEPart::getAbsoluteLinearVel(double* a){
  const dReal* r;
  r=dBodyGetLinearVel(body);
  for (int i=0;i<3;i++)
    a[i]=r[i];
}


void ODEPart::getRelativeLinearVel(double* a){
  const dReal* r=dBodyGetLinearVel(body);
  dVector3 result;
  dBodyVectorFromWorld(body,r[0],r[1],r[2],result);
  for (int i=0;i<3;i++)
    a[i]=result[i];
}


void ODEPart::setAbsoluteLinearVel(const double* a){
  dBodySetLinearVel(body,a[0],a[1],a[2]);
}


void ODEPart::getAbsoluteAngularVel(double* a){
  const dReal* r;
  r=dBodyGetAngularVel(body);
  for (int i=0;i<3;i++)
    a[i]=r[i];
}


void ODEPart::setAbsoluteAngularVel(const double* a){
  dBodySetAngularVel(body,a[0],a[1],a[2]);
}


//TODO : reorder skeletonNodes so that the referenced always come after the referred
//       implement a better refIndex and bodyIDfromPart   

int refIndex(class Robot* r,const class SkeletonNode* ref){  
  for (unsigned int i=0;i<r->skeletonNodes.size();i++)
    if (ref==r->skeletonNodes[i]){
      return i;
    }
  cerr<<"Error: unknown reference "<<endl;
  return -1;
} 


dBodyID bodyIDfromPart(vector <ODEPart*> parts,const class Part * part){
  for (unsigned int i=0;i<parts.size();i++){
    if (parts[i]->srcPart==part) return parts[i]->body;
  }
  return NULL;
}


int odePartIDfromPart(vector <ODEPart*> parts,const class Part * part){
  for (unsigned int i=0;i<parts.size();i++){
    if (parts[i]->srcPart==part) return i;
  }
  cerr<<"Unknown part"<<endl;
  return -1;
}


ODEPart * odePartFromPart(vector <ODEPart*> parts,const class Part * part){
  for (unsigned int i=0;i<parts.size();i++){
    if (parts[i]->srcPart==part) return (ODEPart*)parts[i];
  }
  cerr<<"Unknown part"<<endl;
  return NULL;

}


int ODERobot::getMotorNum(){
  return (int)sortedMotors.size();
}

bool ODERobot::isMotorWorking(){
  return srcRobot!=NULL;
}
void ODERobot::getPivotPosition(double* pos){
  if (pivot!=NULL){
    pivot->getPosition(pos);        
  }
  else{
    cerr<<"No pivot set"<<endl;
    pos[0]=pos[1]=pos[2]=0;
  }
}


void ODERobot::getPivotRototranslation(double* pos){
  if (pivot!=NULL){
    pivot->getRototranslation(pos);    
    mulNegativeTranslationMatrix(pos,pivot->bodyCenter);
  }
  else{
    cerr<<"No pivot set"<<endl;
    eye(pos);
  }
}


void ODERobot::stopVelocity(){
  for (unsigned int i=0;i<odeParts.size();i++) odeParts[i]->stop();
}


void ODERobot::reset(){
  setCompleteRototrasl(initialPosition);
  stopVelocity();
}


void ODERobot::getCompleteRototrasl(vector<double*> &r){
  if (r.size()==0){
    for (unsigned int i=0;i<odeParts.size();i++){
      double* m=new double[16];
      odeParts[i]->getRototranslation(m);
      r.push_back(m);
    }
  }
  else{
    for (unsigned int i=0;i<odeParts.size();i++){
      odeParts[i]->getRototranslation(r[i]);
    }
  }
}


void ODERobot::setCompleteRototrasl(vector<double*> cr){
  for (unsigned int i=0;i<odeParts.size();i++){
    odeParts[i]->setRototranslation(cr[i]);
  }
}


void ODERobot::getState(vector<double*> &r,vector<double*> &vels,vector<double*> &angleVels){
  getCompleteRototrasl(r);
  if (vels.size()==0){
    for (unsigned int i=0;i<odeParts.size();i++){
      double* v=new double[3];
      odeParts[i]->getAbsoluteLinearVel(v);
      vels.push_back(v);
      double* av=new double[3];
      odeParts[i]->getAbsoluteAngularVel(av);
      angleVels.push_back(av);
    }
  }else{
    for (unsigned int i=0;i<odeParts.size();i++){
      odeParts[i]->getAbsoluteLinearVel(vels[i]);
      odeParts[i]->getAbsoluteAngularVel(angleVels[i]);
    }
  }

}


void ODERobot::setState(const vector<double*> &r,const vector<double*> &vels,const vector<double*> &angleVels){
  setCompleteRototrasl(r);
  for (unsigned int i=0;i<odeParts.size();i++){
    odeParts[i]->setAbsoluteLinearVel(vels[i]);
    odeParts[i]->setAbsoluteAngularVel(angleVels[i]);
  }
}


typedef double Matrix4x4[4][4];

ServoParam  defaultServoParam(MOTORANGLELIMIT,MOTORMAXV,MOTORCOEFF,MOTMAXFORCE);

ODERobot::ODERobot(class ODEWorld* odeworld,class Robot* r):
  srcRobot(r),
  //sensors(NULL),
  world(odeworld),  
  pivot(NULL)
  {
    //std::cout<<"CREATED NEW ROBOT"<<this<<std::endl;
 
  Matrix4x4 *pos= new Matrix4x4[r->skeletonNodes.size()];
  vector <class Part *>parts;
  vector <class SkeletonNode *>partSN;
  vector <void *> binarySensorAddresses;
  double temp[16];
  //cout<<"Analyzing "<<r->skeletonNodes.size()<<"nodes"<<endl;
  for (unsigned int sn=0;sn< r->skeletonNodes.size();sn++){
    const SkeletonNode * s=r->skeletonNodes[sn];
    if (s->ref==NULL){      
      setAsRotation((double *)&pos[sn],s->axis,-s->offset);      
      setTranslation((double *)&pos[sn],s->pos);
    }
    else{
      setAsRotation(temp,s->axis,s->offset);      
      setTranslation(temp,s->pos);
      mul(pos[sn][0],pos[refIndex(r,s->ref)][0],temp);
    }	
    if (s->part!=NULL){
      bool alreadyInserted=false;
      for (unsigned int i=0;i<parts.size();i++)
	if (parts[i]==s->part){
	  alreadyInserted=true;
	  break;
	}
      if (!alreadyInserted){
	parts.push_back(s->part);
	partSN.push_back((SkeletonNode*)s);
      }
    }
  }
 
  for (unsigned int p=0;p<parts.size();p++){
    //cout<<"creating part"<<endl;
    dBodyID body=dBodyCreate(odeworld->world);
    ODEPart* op=new ODEPart(body,(Part*)parts[p]);
    if (parts[p]==r->pivot)
      pivot=op;

    //std::cout<<"the part is "<<parts[p]<<" and the pivot is "<<r->pivot<<std::endl;

    dMassSetZero(&(op->m));
    dMass tm;
    op->totalMass=0;
    //std::cout<<"part "<<p<<std::endl;
    for (unsigned int b=0;b<parts[p]->elements.size();b++){
      //std::cout<<"element "<<b<<std::endl;
      Element* be=(Element*)parts[p]->elements[b];
      if (be->type&WEIGHTINFO){
	if (be->m<=0){
	  cerr<<"Negative mass:"<<be->m<<endl;
	}
	op->totalMass+=be->m;
	switch (be->shape){
	case BOXSHAPE:
	  dMassSetBoxTotal(&tm,be->m,be->size->x(),be->size->y(),be->size->z());	  
	  break;
	case SPHERESHAPE:
	  dMassSetSphereTotal(&tm,be->m,be->size->x());	
	  break;
	case CAPPEDCYLINDERSHAPE:
	  //3 is to say the direction is the z axis
	  dMassSetCapsuleTotal(&tm, be->m, 3,be->size->x(),be->size->z());
	  //dMassSetCappedCylinderTotal (&tm, be->m, 3,be->size->x,be->size->z); DEPRECATED from 0.11
	  break;
	}
	dMatrix3 R;
	matrixToODEMatrix(R,be->rotoTransl);
	dMassRotate (&tm,R);
	dMassTranslate (&tm,be->rotoTransl[3],be->rotoTransl[7],be->rotoTransl[11]);
	dMassAdd(&(op->m),&tm);
      }
      if (be->type&ENCUMBRANCEINFO){
	//the first 0 is necessary because object linked through a GeomTransform must not belong to any space	
	dGeomID box=NULL;
	switch(be->shape){
	case BOXSHAPE:
	  box=dCreateBox(0,be->size->x(),be->size->y(),be->size->z());
	  break;
	case SPHERESHAPE:
	  box=dCreateSphere(0,be->size->x());
	  break;
	case CAPPEDCYLINDERSHAPE:
	  box=dCreateCCylinder(0,be->size->x(),be->size->z());
	  break;
	}
	dGeomID t=dCreateGeomTransform(odeworld->worldSpace);
	//destroy the contained box when destroying the containing transform
	dGeomTransformSetCleanup (t,1);
	dGeomTransformSetGeom (t,box);
	dGeomTransformSetInfo(t,1);
	dMatrix3 R;
	matrixToODEMatrix(R,be->rotoTransl);
	dGeomSetRotation (box,R);
	dGeomSetPosition(box,be->rotoTransl[3],be->rotoTransl[7],be->rotoTransl[11]);
	dGeomSetBody(t,body);			 
	op->addGeom(t,be->app,be);
	GeomInfo * info=new GeomInfo(be,op,world);
	dGeomSetData(t,info);
	dGeomSetData(box,info);
      }
      if (be->customizationType&(1<<BINARYSENSORCUSTOMIZATION)){
	binarySensorAddresses.push_back(((BinarySensorCustomization*)be->elementCustomizations[BINARYSENSORCUSTOMIZATION])->colliding);
      }
      if (be->customizationType&(1<<BINARYSENSORBOXCUSTOMIZATION)){
	for (int i=0;i<6;i++)
	  binarySensorAddresses.push_back(((BinarySensorBoxCustomization*)be->elementCustomizations[BINARYSENSORBOXCUSTOMIZATION])->contactActivation[i]);
      }
      
	
    }//for b
    Vector3d tr(op->m.c[0],op->m.c[1],op->m.c[2]);
    dMassTranslate (&op->m,-tr.x(),-tr.y(),-tr.z());
    dBodySetMass(body,&op->m);
    for (unsigned int g=0;g<op->geoms.size();g++){
      dGeomID box=dGeomTransformGetGeom(op->geoms[g]);      
      const dReal* currPos=dGeomGetPosition(box);
      dGeomSetPosition(box,currPos[0]-tr.x(),currPos[1]-tr.y(),currPos[2]-tr.z());      
      currPos=dGeomGetPosition(box);
    }
    
    Vector3d bodyPos;
    int posIndex=refIndex(r,partSN[p]);
    mul(&bodyPos,(double*)pos[posIndex],&tr);
    dBodySetPosition(op->body,bodyPos.x(),bodyPos.y(),bodyPos.z());
    op->bodyCenter.set(tr);

    dMatrix3 m;
    matrixToODEMatrix(m,(double*)pos[posIndex]);
    dBodySetRotation(op->body,m);        
    odeParts.push_back(op);
  }//for parts
  

  for (unsigned int sn=0;sn< r->skeletonNodes.size();sn++){
    SkeletonNode * s=(SkeletonNode *)(r->skeletonNodes[sn]);
    if (s->type==FREEHINGE||s->type==SERVOMOTOR||s->type==FREESLIDER){
	
	dJointID hingeID=s->type==FREEHINGE||s->type==SERVOMOTOR?	  
	  dJointCreateHinge(odeworld->world,0):
	  dJointCreateSlider(odeworld->world,0);


      SkeletonNode * partParent=s->ref;
      //TODO insert fake bodies for chains of refs when needed
      while (partParent->part==NULL&&partParent->ref!=NULL) 
	partParent=partParent->ref;      
      
      Vector3d axis,axist;
      double * refM;
      if (s->ref==NULL){
	refM= new double[16];
	eye(refM);
	
      }else{
	refM=(double*) &pos[refIndex(r,s->ref)][0];
      }      
      getAxis(&axist,s->axis);
      mulAxis(&axis,refM,&axist);                  
      //ODE wants the hinge to be attached before setting its axis and anchor
      dJointAttach(hingeID,bodyIDfromPart(odeParts,partParent->part),bodyIDfromPart(odeParts,s->part));

      if (s->type==FREEHINGE||s->type==SERVOMOTOR){	
	dJointSetHingeAnchor(hingeID,pos[sn][0][3],pos[sn][1][3],pos[sn][2][3]);
	dJointSetHingeAxis(hingeID,axis.x(),axis.y(),axis.z());
      }else{
	dJointSetSliderAxis (hingeID, axis.x(),axis.y(), axis.z());
      }
         
      if (s->type==SERVOMOTOR){
	ODEHinge *oh=new ODEHinge(hingeID,s);
	motors.push_back(oh);
	if (s->motId>=0){
	  sortedMotors[s->motId]=oh;
	}
	dJointSetFeedback(hingeID,& (motors[motors.size()-1]->feedback));
	if (s->servoParam==NULL)
	  s->servoParam=&defaultServoParam;
      }
    }
  }

  static bool zeroBool=false; //resetElements use this address to copy a false over the things to reset
  if (binarySensorAddresses.size()>0)
    resetElements.push_back(new ResetElement(binarySensorAddresses,&zeroBool,sizeof(bool)));  
  getCompleteRototrasl(initialPosition);
  odeworld->registerRobot(this);    
  delete[] pos;     
}

void ODERobot::storeInitialPosition(){
  getCompleteRototrasl(initialPosition);
}

  int contacts;

void ODERobot::stepSensorReset(){
  contacts=0;
  for (unsigned int i=0;i<resetElements.size();i++)
    resetElements[i]->reset();
}

#define SIGNCONVENTION -1


//offset is sufficient at object construction time,no need to add it here
double ODEHinge::getAngle(){  
  return dJointGetHingeAngle(hingeID)/SIGNCONVENTION*srcSkeletonNode->multiplier;
}

  double ODEHinge::getAngleRate(){
    return SIGNCONVENTION*dJointGetHingeAngleRate (hingeID);    
  }

  void ODEHinge::addTorque(double t){
    dJointAddHingeTorque(hingeID,SIGNCONVENTION*t);
  }


void ODERobot::updateMotors(){
  for (unsigned int i=0;i<motors.size();i++){
    SkeletonNode* src=motors[i]->srcSkeletonNode;
    if (!motors[i]->power){
      dJointSetHingeParam (motors[i]->hingeID,dParamVel,0);
      dJointSetHingeParam (motors[i]->hingeID,    dParamFMax, 0);            
    }else{    
      ServoParam* srvPar=src->servoParam;
      double angle=src->multiplier* (*(src->servoValue));   
      if (angle<-srvPar->motorAngleLimit) angle=-srvPar->motorAngleLimit;
      else if (angle>srvPar->motorAngleLimit) angle=srvPar->motorAngleLimit;
      angle=-angle;
      dReal vel=srvPar->motorCoeff*(angle-dJointGetHingeAngle(motors[i]->hingeID));
      if (vel<-srvPar->motorMaxV){
	vel=-srvPar->motorMaxV;      
      }
      else if (vel>srvPar->motorMaxV){
	vel=srvPar->motorMaxV;
      }
      dJointSetHingeParam (motors[i]->hingeID,dParamVel,vel);
      dJointSetHingeParam (motors[i]->hingeID,dParamFMax,srvPar->motorMaxForce);          
    }
  }
}




void ODERobot::getPosition(double* angles){
  for (map<int,ODEHinge*>::iterator it=sortedMotors.begin();it!=sortedMotors.end();it++){
    angles[it->first]=it->second->getAngle();
  }

  /*for (unsigned int i=0;i<motors.size();i++){
    int id=motors[i]->srcSkeletonNode->motId;
    if (id>=0){
      angles[id]=motors[i]->getAngle();
    }
    }//for*/
}


double ODERobot::getPosition(int j){
  return sortedMotors[j]->getAngle();
  /*for (unsigned int i=0;i<motors.size();i++){
    int id=motors[i]->srcSkeletonNode->motId;
    if (id==j){
      return motors[i]->getAngle();
    }
  }//for
  return INVALIDPOS;*/
}

  ODEHinge* ODERobot::getMotorHinge(int motorID){
    return sortedMotors[motorID];
  }


void ODERobot::getMotorTarget(double* angles){
  for (unsigned int i=0;i<motors.size();i++){
    int id=motors[i]->srcSkeletonNode->motId;
    if (id>=0){
      angles[id]=*(motors[i]->srcSkeletonNode->servoValue);      
    }
  }//for
}

double ODERobot::getMotorTarget(int j){
  return *sortedMotors[j]->srcSkeletonNode->servoValue;
}

void ODERobot::rotate(int joint,double angle){  
  *(sortedMotors[joint]->srcSkeletonNode->servoValue)=angle;
  //  *(motors[joint]->srcSkeletonNode->servoValue)=angle;
}

  void ODERobot::rotate(const double* angles){
    for (size_t j=0;j<sortedMotors.size();++j){
      rotate(j,angles[j]);
    }
  }

void ODERobot::setPowerState(int joint,bool state){
  if (joint!=-1){
    sortedMotors[joint]->power=state;
  }else{
    for (map<int,ODEHinge*>::iterator it=sortedMotors.begin();it!=sortedMotors.end();it++){
      it->second->power=state;
    }
  }
}//setPowerstate




void ODERobot::resetMotors(){
  for (unsigned int i=0;i<motors.size();i++)
    *(motors[i]->srcSkeletonNode->servoValue)=0;
}


void ODERobot::translateRobot(double tx,double ty,double tz){
  for (unsigned int i=0;i<odeParts.size();i++)
    odeParts[i]->translate(tx,ty,tz);
}


void ODERobot::setRotatePivot(Part* p){
  pivot=NULL;
  for (unsigned int i=0;i<odeParts.size();i++){
    if (odeParts[i]->srcPart==p){
      pivot=odeParts[i];
      break;
    }//if
  }//for
}//setRotatePivot


void ODERobot::rotateRobot(double* m){  
  double pivotp[3];
  if (pivot==NULL) return;
  pivot->getPosition(pivotp);
  dMatrix3 d3m;
  matrixToODEMatrix(d3m,m);
  double p[16];
  double r[16];
  double tr[16],trb[16];  
  eye(tr);eye(trb);
  tr[3]=-pivotp[0];
  tr[7]=-pivotp[1];
  tr[11]=-pivotp[2];
  trb[3]=pivotp[0];
  trb[7]=pivotp[1];
  trb[11]=pivotp[2];  
  for (unsigned int i=0;i<odeParts.size();i++){    
    odeParts[i]->getRototranslation(p);
    p[3]-=pivotp[0];
    p[7]-=pivotp[1];
    p[11]-=pivotp[2];
    mul(r,m,p);
    r[3]+=pivotp[0];
    r[7]+=pivotp[1];
    r[11]+=pivotp[2];
    odeParts[i]->setRototranslation(r);
  }
}

double ODERobot::getMinZ(){
  double minZ=NAN;
  for (unsigned int i=0;i<odeParts.size();i++){   
    double pos[3];
    odeParts[i]->getPosition(pos);
    if (isnan(minZ)||minZ>pos[2]){
      minZ=pos[2];
    }
  }
  return minZ;
}


void ODERobot::setPivotRototranslation(const double *m){ 
  double curr[16],icurr[16];
  getPivotRototranslation(curr);
  invert(icurr,curr);
  mul(curr,m,icurr);
  rotateRobot(curr);
  double pos[3]; 
  pivot->getPosition(pos);
  translateRobot(m[3]-pos[0],m[7]-pos[1],m[11]-pos[2]);
}


ODEPart* ODERobot::getODEPart(int partId){
  for(unsigned int i=0;i<odeParts.size();i++){
    if (odeParts[i]->srcPart->id==partId)
      return odeParts[i];
  }
  return NULL;
}


int ODERobot::getODEPartID(int structureID){
  for(unsigned int i=0;i<odeParts.size();i++){
    if (odeParts[i]->srcPart->id==structureID)
      return i;
  }
  return -1;
}


void getMotorTorque(Vector3d* torque,const ODEHinge* motor){
  dVector3 jointPos;    
    dJointGetHingeAnchor(motor->hingeID,jointPos);
    //if (motor->srcSkeletonNode->motId==15){
    

    const dReal* pos1=dBodyGetPosition(dJointGetBody(motor->hingeID,0));
    //the commented instructions should give the same result
    //const dReal* pos2=dBodyGetPosition(dJointGetBody(motors[i]->hingeID,1));

    Vector3d v1(motor->feedback.t1[0],motor->feedback.t1[1],motor->feedback.t1[2]);   
    //Vector3d v2(motor->feedback.t2[0],motor->feedback.t2[1],motor->feedback.t2[2]);
    
    Vector3d f1(motor->feedback.f1[0],motor->feedback.f1[1],motor->feedback.f1[2]);   
    //Vector3d f2(motor->feedback.f2[0],motor->feedback.f2[1],motor->feedback.f2[2]);

    Vector3d p1(jointPos[0],jointPos[1],jointPos[2]);
    //Vector3d p2(jointPos[0],jointPos[1],jointPos[2]);

    Vector3d p1s(pos1[0],pos1[1],pos1[2]);
    //Vector3d p2s(pos2[0],pos2[1],pos2[2]);

    p1.sub(p1s);
    //p2.sub(&p2s);
   
    Vector3d c1;c1.cross(f1,p1);
    //Vector3d c2;c2.cross(f2,p2);
    torque->sum(v1,c1);
    //v2.sum(c2);
}

void ODERobot::getTorques(Vector3d* torques){
  for (unsigned int i=0;i<motors.size();i++){     
    getMotorTorque(&torques[motors[i]->srcSkeletonNode->motId],motors[i]);
  }
}


void ODERobot::getTorques(double* torques){
  Vector3d v;
  for (unsigned int i=0;i<motors.size();i++){
    getMotorTorque(&v,motors[i]);
    torques[motors[i]->srcSkeletonNode->motId]=v.norm();
  }
}

double ODERobot::getTorque(int joint){
  Vector3d v;
  getMotorTorque(&v,sortedMotors[joint]);
  return v.norm();
}

double ODERobot::getMotorAxisTorque(int joint){
  dVector3 jointAxis;    
  dJointGetHingeAxis(sortedMotors[joint]->hingeID,jointAxis);
  Vector3d v;
  getMotorTorque(&v,sortedMotors[joint]);
  Vector3d dir(jointAxis[0],jointAxis[1],jointAxis[2]);
  return dir.dot(v);
}

void ODERobot::getMotorAxisTorques(double* torques){
  dVector3 jointAxis;    
  Vector3d v,dir; 
  for (unsigned int i=0;i<motors.size();i++){
    getMotorTorque(&v,motors[i]);
    dJointGetHingeAxis(motors[i]->hingeID,jointAxis);
    dir.set(jointAxis[0],jointAxis[1],jointAxis[2]);
    torques[motors[i]->srcSkeletonNode->motId]=dir.dot(v)*motors[i]->srcSkeletonNode->multiplier;
  }
}



double ODERobot::getTotalTorque(){
  double t=0;
  Array<double> torques(motors.size());
  getTorques(torques.data);
  for (unsigned int i=0;i<motors.size();i++){
    t+=torques[i];
  }
  return t;
}


void ODERobot::getEnergies(double* en){
  Array<double> torques(motors.size());
  getTorques(torques.data);
  for (unsigned int i=0;i<motors.size();i++){    
    en[motors[i]->srcSkeletonNode->motId]=
      torques[motors[i]->srcSkeletonNode->motId]*2*M_PI*
      fabs(dJointGetHingeAngleRate(motors[i]->hingeID));
  }
  
}


double ODERobot::getTotalEnergy(){
  double e=0;
  Array<double> torques(motors.size());
  getTorques(torques.data);
  for (unsigned int i=0;i<motors.size();i++){    
    e+=torques[i]*2*M_PI*fabs(dJointGetHingeAngleRate(motors[i]->hingeID));
  }
  return e;

}

void ODERobot::coldRotate(){
  static vector<double*> m;
  static vector<int> idx; //for each part, the corresponding matrix;
  if (m.size()==0){
    for (unsigned int i=0;i<odeParts.size();i++){
      idx.push_back(-1);
    }
    for (unsigned int i=0;i<srcRobot->skeletonNodes.size();i++){
      if (srcRobot->skeletonNodes[i]->id!=(int)i){
	cerr<<"Unexpected skeleton ID"<<endl;
      }
      m.push_back(new double[16]);
      if (srcRobot->skeletonNodes[i]->part!=NULL){
	int wht=getODEPartID(srcRobot->skeletonNodes[i]->part->id);
	if (idx[wht]==-1)
	  idx[wht]=i;
      }
    }    
  }
  double self[16],root[16];
  getPivotRototranslation(root);

  double tr[16];
  eye(tr);
  //Vector3d bc(pivot->bodyCenter);
  //bc.negate();
  //setTranslation(tr,&bc);
  mul(m[0],root,tr);
  for (unsigned int i=0;i<srcRobot->skeletonNodes.size();i++){
    if (srcRobot->skeletonNodes[i]->ref==NULL){
    }else{
      srcRobot->skeletonNodes[i]->getLocalRototranslation(self);
      mul(m[i],m[srcRobot->skeletonNodes[i]->ref->id],self);
    }        
  }
  double pm[16];
  for (unsigned int i=0;i<idx.size();i++){
    setTranslation(tr,&odeParts[i]->bodyCenter);
    mul(pm,m[idx[i]],tr);
    odeParts[i]->setRototranslation(pm);
  }
}

static bool inited=false;




ODEWorld::ODEWorld():
  stepCounter(0),runstate(STOPPED),
  worldMutex("ODEWorld::worldMutex"){
  t=0;
  erp=0.2;
  cfm=2E-2;
  mu=0.7;
  mu2=0.7;
  bounce=1E-4;
  bounce_vel=1E-4;
  if (!inited){ 
    dInitODE();
    inited=true;
  }
  world = dWorldCreate();
  worldSpace = dHashSpaceCreate (0);
  contactgroup = dJointGroupCreate (0);
  dWorldSetGravity (world,0,0,1*-9.81);
  ground = dCreatePlane (worldSpace,0,0,1,0);
  dGeomSetData(ground,NULL);
  noCollParts=NULL;
  noCollPartsSize=0;  
  paused=false;
  waiting=false;
  timeZero=getSystemMillis();
}


int ODEWorld::getPartWorldId(ODEPart *p){
  int id=-1;
  for (unsigned int i=0;i<odeParts.size()&&id==-1;i++){
    if (odeParts[i]==p) id=i;
  }
  if (id==-1){
    id=(int)odeParts.size();
    odeParts.push_back(p);
  }
  return id;
}


int ODEWorld::getMaxPartId(){
  int maxId=0;
  for (unsigned int r=0;r<robots.size();r++)
    for (unsigned int p=0;p<robots[r]->odeParts.size();p++){
      maxId=max(maxId,getPartWorldId(robots[r]->odeParts[p]));
    }
  return maxId;
}

void ODEWorld::registerRobot(class ODERobot* r){
  robots.push_back(r);  
  updateCollMatrix();
}

ODERobot* ODEWorld::addRobot(class Robot* r,double* rotoTra){
  double stdRoto[16];
  if (rotoTra==NULL){
    eye(stdRoto);
    setTranslation(stdRoto,0,0,0.8);    
    rotoTra=stdRoto;
  }
  ODERobot* oder=new ODERobot(this,r);  
  //std::cout<<"setting----------------"<<std::endl;
  //print(rotoTra);

  oder->setPivotRototranslation(rotoTra);
  return oder;
  //registerRobot();  is done by the constructor
}

void ODEWorld::updateCollMatrix(){
  noCollPartsSize=getMaxPartId()+1;  
  bool* ncp=new bool[noCollPartsSize*noCollPartsSize];
  for (int i=0;i<noCollPartsSize*noCollPartsSize;i++){
    ncp[i]=false;
  }//for i
  for (unsigned int k=0;k<robots.size();k++){
    ODERobot* r=robots[k];
    for (unsigned int i=0;i<r->srcRobot->noCollParts.size();i=i+2){    
      int id1,id2;
      id1=getPartWorldId(odePartFromPart(r->odeParts,r->srcRobot->noCollParts[i]));
      id2=getPartWorldId(odePartFromPart(r->odeParts,r->srcRobot->noCollParts[i+1]));
      //std::cout<<"No collision between "<<id1<<" & "<<id2<<std::endl;
      ncp[id1*noCollPartsSize+id2]=ncp[id2*noCollPartsSize+id1]=true;
    }//for robot->noCollParts
  }//for k
  if (noCollParts!=NULL){
    delete[] noCollParts;
  }
  noCollParts=ncp;  
}



bool ODEWorld::canCollide(dGeomID a,dGeomID b){  
  GeomInfo* ainfo=(GeomInfo*)dGeomGetData(a);
  GeomInfo* binfo=(GeomInfo*)dGeomGetData(b);

  if (ainfo==NULL||binfo==NULL) return true;

  return !noCollParts[ainfo->worldPartID*noCollPartsSize+binfo->worldPartID];
}


ODEPart* getODEPart(ODEWorld* w,dGeomID ge,int *tp){
  for (unsigned int r=0;r<w->robots.size();r++)
    for (unsigned int p=0;p<w->robots[r]->odeParts.size();p++){
      for (unsigned int g=0;g<w->robots[r]->odeParts[p]->geoms.size();g++){
	if (w->robots[r]->odeParts[p]->geoms[g]==ge){
	  *tp=p;
	  return w->robots[r]->odeParts[p];
	}
      }
    }
  *tp=-1;
  return NULL;
}


void colour(ODEWorld* w,dGeomID ge1,dGeomID ge2){
  int n1;int n2;
  ODEPart* p1=getODEPart(w,ge1,&n1);
  ODEPart* p2=getODEPart(w,ge2,&n2);
  int col=n1+n2;
  float r=(float)sin((double)col);
  float g=(float)cos((double)col);
  float b=(float)sin((double)2*col);
  if (p1!=NULL)
    for (unsigned int i=0;i<p1->apps.size();i++)
      p1->apps[i]->setColor(r,g,b);

  if (p2!=NULL)
    for (unsigned int i=0;i<p2->apps.size();i++)
      p2->apps[i]->setColor(r,g,b);
}


void resetColour(ODEWorld* w){
  for (unsigned int r=0;r<w->robots.size();r++)
    for (unsigned int p=0;p<w->robots[r]->odeParts.size();p++){
      for (unsigned int g=0;g<w->robots[r]->odeParts[p]->geoms.size();g++){
	w->robots[r]->odeParts[p]->apps[g]->setColor(0,0,0);
      }
    }    
}




void setBinaryCollision(dGeomID g,dVector3 contact){
  GeomInfo* gi=(GeomInfo*)dGeomGetData(g);
  if (gi==NULL||!(gi->srcElement->customizationType&(1<<BINARYSENSORBOXCUSTOMIZATION)))
    return;
  static Vector3d cont;
  cont.set(contact[0],contact[1],contact[2]);
  double roto[16];
  getAbsoluteRototrasl(roto,g);
  int faces[6];
  int nfaces=nearFaces(faces,cont,roto,gi->srcElement->size->x(),gi->srcElement->size->y(),gi->srcElement->size->z());      
  for (int i=0;i<nfaces;i++){
    *(((BinarySensorBoxCustomization*)gi->srcElement->elementCustomizations[BINARYSENSORBOXCUSTOMIZATION])
      ->contactActivation[faces[i]])=true;    
  }
}//setBinaryCollision




  //ForceCollisionManager  forceCollisionManager;


void nearCallback (void *data, dGeomID o1, dGeomID o2)
{  
  int i;
  ODEWorld* odeWorld=(ODEWorld*) data;
  dBodyID b1 = dGeomGetBody(o1);
  dBodyID b2 = dGeomGetBody(o2);
  if (b1 && b2 && dAreConnectedExcluding (b1,b2,dJointTypeContact)) return;    
  if (!odeWorld->canCollide(o1,o2)){
    return;
  }


  

  dContact contact[CONTACTPOINTS]; 

  for (i=0; i<CONTACTPOINTS; i++) {
    contact[i].surface.mode = dContactBounce | dContactSoftCFM | dContactSoftERP|dContactApprox1;
    contact[i].surface.mu = odeWorld->mu;
    contact[i].surface.mu2 = odeWorld->mu2;
    contact[i].surface.bounce = odeWorld->bounce;
    contact[i].surface.bounce_vel = odeWorld->bounce_vel;
    if (dGeomGetCategoryBits(o1)==1||dGeomGetCategoryBits(o2)==1){
      contact[i].surface.mu = odeWorld->mu*10;
    contact[i].surface.mu2 = odeWorld->mu2*10;
      contact[i].surface.soft_cfm =4E-2;
    }else{
      contact[i].surface.soft_cfm = odeWorld->cfm;
    }
    contact[i].surface.soft_erp = odeWorld->erp;
  }
  if (int numc = dCollide (o1,o2,CONTACTPOINTS,&contact[0].geom,
			   sizeof(dContact))) {
    
 #ifdef SHOWCOLLISIONS    
    colour(odeWorld,o1,o2);    
 #endif
    for (i=0; i<numc; i++) {      
      dJointID c = dJointCreateContact (odeWorld->world,odeWorld->contactgroup,contact+i);
    
      if (contacts++>100) return;//HACK
      dJointAttach (c,b1,b2);      
      setBinaryCollision(o1,contact[i].geom.pos);
      setBinaryCollision(o2,contact[i].geom.pos);
      odeWorld->forceCollisionManager.registerContact(c,o1,0,contact[i].geom.pos,contact[i].geom.normal,o2);
      odeWorld->forceCollisionManager.registerContact(c,o2,1,contact[i].geom.pos,contact[i].geom.normal,o1);      
    }//for i    
  }
}

inline void ODEWorld::step(double pdt,bool lock){
  //cout<<"world step"<<endl;
  if (lock) {
    worldMutex.lock();    
  }
  dt=pdt;
  t+=dt;  
  //std::cout<<"~t="<<t<<std::endl;
  for (unsigned int r=0;r<robots.size();r++){
    robots[r]->updateMotors();
    robots[r]->stepSensorReset();
  } 
#ifdef SHOWCOLLISIONS
  resetColour(this);
#endif
  forceCollisionManager.reset();
  dSpaceCollide (worldSpace,this,&nearCallback);  
  dWorldStep (world,dt);
  forceCollisionManager.updateSensors();
  dJointGroupEmpty (contactgroup);
  if (lock) {
    worldMutex.unlock();    
  }
  for (unsigned int i=0;i<callbacks.size();i++){
    if (stepCounter%callbacks[i]->everyN==0){
      callbacks[i]->odeWorldCallback();
    }
  }
  stepCounter++;
}




void ODEWorld::pause(bool p){
  waiting=true;
  if (p){            
    worldMutex.lock();    
    paused=true;    
    timeZero=timeZero-getSystemMillis();
  }
  else{    
    worldMutex.unlock();
    paused=false;
    timeZero=getSystemMillis()+timeZero;
  }
  waiting=false;
}


ODEWorld::~ODEWorld(){
  runstate=STOPPING;
  join();
  for (unsigned int r=0;r<robots.size();r++){
    for (unsigned int p=0;p<robots[r]->odeParts.size();p++){
      for (unsigned int g=0;g<robots[r]->odeParts[p]->geoms.size();g++){
	dGeomDestroy(robots[r]->odeParts[p]->geoms[g]);
      }//for g
    }//for p    
  }//for r
  if (noCollParts) delete[] noCollParts;
  destroyS(robots);
  dGeomDestroy(ground);
  dJointGroupDestroy (contactgroup);
  dSpaceDestroy (worldSpace);
  dWorldDestroy (world);

}

  ODERobot::~ODERobot(){
    destroyS(odeParts);
    destroyS(motors);
    destroyA(initialPosition);
  }

  ODEPart::~ODEPart(){
    for (size_t i=0;i<geoms.size();i++){
      GeomInfo* gi=(GeomInfo*)dGeomGetData(geoms[i]);
      if (gi!=NULL) delete gi;
    }
    
  }
  

  void ODEWorld::run(){
    while (runstate==RUNNING){
      if (!waiting) 
	step(dt);
      //milliSleep(100);
    }
    runstate=STOPPED;
  }
  

void ODEWorld::run(double pdt){
  if (runstate==RUNNING){
    cerr<<"Already running"<<endl;
  }
  runstate=RUNNING;
  dt=pdt;
  start();
}

void ODEWorld::run(double dt,int steps){
  for (int i=0;i<steps;i++)
    step(dt,false);
}

void ODEWorld::stop(){
  while (runstate!=STOPPED){
    runstate=STOPPING;
    milliSleep(1);
  }
  //cout<<"World stopped"<<endl;
}


void ODEWorld::resetTime(){
  t=0;
  timeZero=getSystemMillis();
}


void ODEWorld::reset(){
  resetTime();
  for (unsigned int i=0;i<robots.size();i++)
    robots[i]->reset();
}


void ODEWorld::setGravity(double g){
  dWorldSetGravity (world,0,0,g);
}


FreeObject::FreeObject(Element* e,bool so):
  app(e->app),solid(so)
{
  switch(e->shape){
  case BOXSHAPE:
    geom=dCreateBox(0,e->size->x(),e->size->y(),e->size->z());
    break;
  case SPHERESHAPE:
    geom=dCreateSphere(0,e->size->x());
    break;
  case CAPPEDCYLINDERSHAPE:
    geom=dCreateCCylinder(0,e->size->x(),e->size->z());
    break;
  }
  dGeomSetPosition(geom,e->rotoTransl[3],e->rotoTransl[7] ,e->rotoTransl[11]);
  dMatrix3 m;
  matrixToODEMatrix(m,e->rotoTransl);
  dGeomSetRotation(geom,m);
  dGeomSetCategoryBits(geom,1);
  dGeomSetCollideBits(geom,~dGeomGetCategoryBits(geom));
}  

void FreeObject::setPosition(const Vector3d& pos){
  dGeomSetPosition(geom,pos[0],pos[1],pos[2]);
}


}//helium

