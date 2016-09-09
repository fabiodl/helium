#include <helium/simul/structure.h> //util

//#include <helium/util/ioUtil.h>
#include <helium/robot/motorInterface.h>


//#include <math/priorityBuffer.h>
//#include "motors.h"


using namespace std;

namespace helium{

void Part::addElement(class Element* b){
  if (b==NULL) {
    cout<<"Dont' add null elements!"<<endl;
  exit(0);
  }
  b->ownerPart=this;
  elements.push_back(b);  

}


void SkeletonNode::getLocalRototranslation(double* m) const{
  double val=0;
    if (type==SERVOMOTOR&&servoValue!=NULL) 
      val= *servoValue;
    else if (type==FREEHINGE&&fakeServo!=NULL)
      val=*fakeServo;

  setAsRotation(m,axis,offset+multiplier*val);
  setTranslation(m,pos);

}

  /*void SkeletonNode::getLocalRototranslationDerivative(double* m) const{
  eye(m);
  double val=0;
    if (type==SERVOMOTOR&&servoValue!=NULL) 
      val= *servoValue;
    else if (type==FREEHINGE&&fakeServo!=NULL)
      val=*fakeServo;

  setAsRotationDerivative(m,axis,offset+multiplier*val);
  //setTranslation(m,pos);

  }*/




int SkeletonNode::getMotorChain(int* ids,double* values){
  int num=0;
  for(SkeletonNode* n=this;n!=NULL;n=n->ref){
    if (n->motId!=-1){
      ids[num]=n->motId;
      switch (n->type){
      case SERVOMOTOR:
	values[num]=*n->servoValue;
	break;
      case FREESLIDER:
      case FREEHINGE:
	values[num]=*n->fakeServo;
	break;
      case REFERENCEROTATION:
	values[num]=NAN;
	break;
      }//switch
      num++;
    }//if
  }//for
  return num;
}




void SkeletonNode::getMotorChain(int* num,double** motors){
  *num=0;
  for(SkeletonNode* n=this;n!=NULL;n=n->ref){
    double *p=NULL;
    switch (n->type){
    case SERVOMOTOR:
      p=n->servoValue;
      break;
    case FREESLIDER:
    case FREEHINGE:
      p=n->fakeServo;
      break;
    case REFERENCEROTATION:
      p=NULL;
      break;
    }//switch
    if (p!=NULL){
      motors[(*num)++]=p;
    }//if
  }//for
}


void SkeletonNode::getAbsoluteRototranslation(double *m) const{
  const SkeletonNode* n=this;
  double r[16];
  double temp[16];
  eye(m);
  for(;n!=NULL;n=n->ref){
    n->getLocalRototranslation(temp);
    mul(r,temp,m);
    memcpy(m,r,16*sizeof(double));
  }
}


void SkeletonNode::getAbsoluteCoordinate(Vector3d* res,Vector3d* pos){
  double m[16];
  getAbsoluteRototranslation(m);
  mul(res,m,pos);
}


void Robot::addSkeletonNode(const class SkeletonNode* s){
  if (s==NULL){
    cout<<"Dont' add null nodes!"<<endl;
    exit(0);
  }
  skeletonNodes.push_back(s);
}


MouseSensorBoxCustomization::MouseSensorBoxCustomization(int startID,double * startLocation){
  for (int i=0;i<6;i++){
    id[i]=startID+i;
    mouseActivation[i]=&(startLocation[i]);
    active[i]=true;
  }
}


BinarySensorBoxCustomization::BinarySensorBoxCustomization(int startID,bool * startLocation){
  for (int i=0;i<6;i++){
    id[i]=startID+i;
    contactActivation[i]=&(startLocation[i]);
  }
}


ForceSensorBoxCustomization::ForceSensorBoxCustomization(int startID,Vector3d * apStartLocation,Vector3d* fvStartLocation,double* pforce,bool* pwithGround){
  for (int i=0;i<6;i++){
    id[i]=startID+i;
    applicationPoint[i]=&(apStartLocation[i]);
    forceVector[i]=&(fvStartLocation[i]);
    force[i]=&(pforce[i]);
    withGround[i]=&(pwithGround[i]);
  }
}


void Robot::disableCollDet(const class Part* i,const class Part* j){
  noCollParts.push_back(i);
  noCollParts.push_back(j);  
}


int Robot::getNodesNum(){
  return (int)skeletonNodes.size();
}


int Robot::getPartsNum(){
  int c=0;
  for (unsigned int i=0;i<skeletonNodes.size();i++){
    if (skeletonNodes[i]->part!=NULL) c++;
  }
  return c;
}


int Robot::getElementsNum(){
  int c=0;
  for (unsigned int i=0;i<skeletonNodes.size();i++){
    if (skeletonNodes[i]->part!=NULL) c+=(int)skeletonNodes[i]->part->elements.size();
  }
  return c;
}


int Robot::getMaxNodeID(){
  int m=-1;
  for (unsigned int i=0;i<skeletonNodes.size();i++)
    if (skeletonNodes[i]->id>m) m=skeletonNodes[i]->id; 
  return m; 
}


int Robot::getMaxPartID(){
  int m=-1;
  for (unsigned int i=0;i<skeletonNodes.size();i++)
    if (skeletonNodes[i]->part!=NULL&&skeletonNodes[i]->part->id>m) 
      m=skeletonNodes[i]->part->id; 
  return m; 
}


int Robot::getMaxElementID(){
  int m=-1;
  for (unsigned int i=0;i<skeletonNodes.size();i++)
    if (skeletonNodes[i]->part!=NULL)
      for (unsigned int j=0;j<skeletonNodes[i]->part->elements.size();j++)
	m=max(m,skeletonNodes[i]->part->elements[j]->id); 
  return m; 
}


void Robot::printRobot(){
  for (unsigned int n=0;n<skeletonNodes.size();n++){
    cout<<"node "<<n<<":"<<(skeletonNodes[n]->id)<<"("<< skeletonNodes[n]<<  ")" <<endl;
    if (skeletonNodes[n]->part==NULL) 
      cout<<"no part"<<endl;
    else{
      cout<<"part"<<skeletonNodes[n]->part->id<<"("<<skeletonNodes[n]->part<<"),"<<skeletonNodes[n]->part->elements.size()<<" elements" <<endl;
      for (unsigned int e=0;e<skeletonNodes[n]->part->elements.size();e++)
	cout<<" element"<<e<<":"<<skeletonNodes[n]->part->elements[e]->id<<"("<<skeletonNodes[n]->part->elements[e]<<")"<<endl;
    }
  }//for n
}//print


void Robot::describe(ostream& out){
  for (unsigned int i=0;i<skeletonNodes.size();i++){
    out<<"Joint "<<i<<" is a ";
    switch(skeletonNodes[i]->type){
    case FREEHINGE:
      out<<"Free hinge"<<endl;
      break;
    case FREESLIDER:
      out<<"Free slider"<<endl;
      break;
    case SERVOMOTOR:
      out<<"servomotor whose id is "<<skeletonNodes[i]->motId;//<<" and human readable name is "<<JointNames[skeletonNodes[i]->motId]<<endl;
      break;
    case REFERENCEROTATION:
      out<<"a fixed angle joint(not a proper joint...)"<<endl;
      break;
    }
    out<<"aligned along the ";
    switch(skeletonNodes[i]->axis){
    case Axis::X:
      out<<"X";
      break;
    case Axis::Y:
      out<<"Y";
      break;
    case Axis::Z:
      out<<"Z";
      break;
    case Axis::NEGX:
      out<<"NEGX";
      break;
    case Axis::NEGY:
      out<<"NEGY";
      break;
    case Axis::NEGZ:
      out<<"NEGZ";
      break;

    }
    out<<" axis";
    if (skeletonNodes[i]->ref!=NULL){
      out<<" and placed w.r.t. to joint "<<skeletonNodes[i]->ref->id<<" using the translation"<<*skeletonNodes[i]->pos;
    }
    out<<endl;
    if (skeletonNodes[i]->part!=NULL){
       out<<"Part "<<skeletonNodes[i]->part->id<<" is attached after joint "<<i<<endl;
       out<<"Part "<<skeletonNodes[i]->part->id<<" is composed of the following geometries:"<<endl;
      for (unsigned int g=0;g<skeletonNodes[i]->part->elements.size();g++){		
	out<<i<<"."<<g;
	out<<"- a ";
	switch(skeletonNodes[i]->part->elements[g]->shape){
	case BOXSHAPE:
	  out<<"parallelepiped of size "<<skeletonNodes[i]->part->elements[g]->size->x()<<"x"<<skeletonNodes[i]->part->elements[g]->size->y()<<"x"<<skeletonNodes[i]->part->elements[g]->size->z()<<endl;
	    break;
	case SPHERESHAPE:
	  out<<"sphere of radius "<<skeletonNodes[i]->part->elements[g]->size->x()<<endl;
	  break;
	case CAPPEDCYLINDERSHAPE:
	  out<<"capped cylinder of radius "<<skeletonNodes[i]->part->elements[g]->size->x()<<" and height (ignoring the semispheres)" <<skeletonNodes[i]->part->elements[g]->size->y()<<endl;
	  break;
	}
	out<<" that weights "<<skeletonNodes[i]->part->elements[g]->m;
	out<<"and is placed w.r.t. the joint "<<i<<" using the rototranslation matrix"<<endl;
	print((double*)skeletonNodes[i]->part->elements[g]->rotoTransl,out);
      }
    }

  }

}

string Robot::getJointName(int joint) const{
  stringstream ss;
  ss<<"Joint "<<joint;
  return ss.str();
}

string Robot::getSensorName(int sid) const{
  stringstream ss;
  ss<<"Sensor "<<sid;
  return ss.str();
}



SkeletonNode* Robot::getSkeletonNode(int motID){
  for (unsigned int s=0;s<skeletonNodes.size();s++){
    if (skeletonNodes[s]->motId==motID){
      return (SkeletonNode*) skeletonNodes[s];
    }
  }//for s
  return NULL;
}



const Element* Robot::getElement(int elementID){
  for (unsigned int s=0;s<skeletonNodes.size();s++){
    if (skeletonNodes[s]->part!=NULL){
      for (unsigned int g=0;g<skeletonNodes[s]->part->elements.size();g++){
	if (skeletonNodes[s]->part->elements[g]->id==elementID)
	  return skeletonNodes[s]->part->elements[g];
      }

    }
  }
  return NULL;
}

void Robot::setServoParam(ServoParam* sp){
  for (size_t s=0;s<skeletonNodes.size();s++){
    ((SkeletonNode*)skeletonNodes[s])->setServoParam(sp);
  }//for s
}



Cog::Cog(Robot* pr):r(pr){
  mPos=new Vector3d[pr->skeletonNodes.size()];
  absm=new double[pr->skeletonNodes.size()][16];
  m=new double[pr->skeletonNodes.size()];
  double M=0; //total mass  
  for (size_t i=0;i<pr->skeletonNodes.size();i++){
    m[i]=0;
    mPos[i].zero();
    if (pr->skeletonNodes[i]->part!=NULL){
      for (size_t e=0;e<pr->skeletonNodes[i]->part->elements.size();e++){
	const Element &el=*(pr->skeletonNodes[i]->part->elements[e]);
	if (el.type&WEIGHTINFO){
	  Vector3d locp;
	  getTranslation(&locp,el.rotoTransl);
	  locp.mulScalar(el.m);
	  mPos[i].sum(locp);
	  m[i]+=el.m;
	}	  
      }//for e
    }//if p!=NULL
    if (m[i]!=0){
      mPos[i].mulScalar(1.0/m[i]);//weighting betrween elements
    }
    M+=m[i];
  }//for i
  for (size_t i=0;i<pr->skeletonNodes.size();i++){
    m[i]/=M;
  }
}

Cog::~Cog(){
  delete[] mPos;
  delete[] absm;
  delete[] m;
}

void Cog::get(Vector3d& c){
  c.zero();
  double locm[16];
  Vector3d ldcog;
  for (size_t i=0;i<r->skeletonNodes.size();i++){      
    r->skeletonNodes[i]->getLocalRototranslation(locm);    
    if (r->skeletonNodes[i]->ref!=NULL)
      mul(absm[i],absm[r->skeletonNodes[i]->ref->id],locm);
    else{
      copyMatrix(absm[i],locm);
    }
    mul(&ldcog,absm[i],&mPos[i]);
    ldcog.mulScalar(m[i]);
    c.sum(ldcog);
  }


}

SensorToElement::SensorToElement(Robot* r){
  int ids=-1;
  for (size_t i=0;i<r->skeletonNodes.size();i++){
    if (r->skeletonNodes[i]->part!=NULL){
      for (size_t e=0;e<r->skeletonNodes[i]->part->elements.size();e++){
	if (r->skeletonNodes[i]->part->elements[e]->hasCustomization(FORCESENSORBOXCUSTOMIZATION)){
	  ForceSensorBoxCustomization* fc=(ForceSensorBoxCustomization*)r->skeletonNodes[i]->part->elements[e]->elementCustomizations[FORCESENSORBOXCUSTOMIZATION];
	  ids=(std::max)(ids,fc->id[CUBEFACENUM-1]);	  
	}//forceCustomization
      }//for e
    }//if
  }//for i
  ids++;
  info.resize(ids);
   for (size_t i=0;i<r->skeletonNodes.size();i++){
    if (r->skeletonNodes[i]->part!=NULL){
      for (size_t e=0;e<r->skeletonNodes[i]->part->elements.size();e++){
	if (r->skeletonNodes[i]->part->elements[e]->hasCustomization(FORCESENSORBOXCUSTOMIZATION)){
	  ForceSensorBoxCustomization* fc=(ForceSensorBoxCustomization*)r->skeletonNodes[i]->part->elements[e]->elementCustomizations[FORCESENSORBOXCUSTOMIZATION];
	  for (int f=0;f<CUBEFACENUM;f++){
	    const Element* el=r->skeletonNodes[i]->part->elements[e];
	    info[fc->id[f]].sn=r->skeletonNodes[i];
	    info[fc->id[f]].el=el;
	    double tmp[24];
	    getBoxVertex(tmp,el->size->x(),el->size->y(),el->size->z(),true);
	    mulPoints(info[fc->id[f]].vtx,el->rotoTransl,tmp,CUBEVERTEXNUM);
	  }//for face
	}//if sensorcustomization
      }//for e
    }//if part
   }//for i

}//SensorToElement::SensorToElement
}
