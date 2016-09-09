#ifndef HE_V4GSTRUCTURE_
#define HE_V4GSTRUCTURE_

#include <helium/simul/structure.h>
#include <helium/destroy/autoDestroy.h>

#define BABYVERSION

namespace helium{

static const double TOTALWEIGHT=3.2;
  static const double BOXWEIGHT=0.035;
  static const double SERVOWEIGHT=0.063;

namespace Rsensor {
  enum sensorNameR { 
    lHipB,lHipF,lKnee,lLegupperL,lLegupperF,lLegupperR,lAnkleF,lLeglowerR,lLeglowerF,lLeglowerL,lFootpalmFL,lFoottopL,lFootpalmBL,lFoottopB,lFootpalmFR,lFoottopR,lAnkleB,lFootpalmBR,rHipF,rHipB,rLegupperL,rLegupperF,rLegupperR,rKnee,rLeglowerR,rLeglowerF,rLeglowerL,rAnkleF,rFootpalmFL,rFoottopL,rFootpalmBL,rAnkleB,rFootpalmFR,rFoottopR,rFootpalmBR,rFoottopB,lShoulderT,lShoulderL,lArmupperF,lElbow,lArmupperB,lHandtopB,lHandtopT,lArmlowerF,lArmlowerB,lHandpalmB,lHandpalmT,rShoulderT,rShoulderR,rArmupperF,rElbow,rArmupperB,rHandtopB,rHandtopT,rArmlowerF,rArmlowerB,rHandpalmB,rHandpalmT,lSideB,lSideC,lSideF,BackupperL,BacklowerL,rSideF,rSideC,rSideB,BacklowerR,BackupperR,ChestL,ChestR,AbdomenL,AbdomenR,AbdomenC,lRib,rRib,ForeheadR,ForeheadL,ChinL,ChinR,rTemple,rJaw,lJaw,lTemple,HeadtopL,HeadtopC,Headback,HeadtopR,lEar,Nape,rEar,
#ifdef ADDITIONAL_SENSORS
lLittlefinger,rLittlefinger,
#endif

REALSENSORNUM
  };

  static const char* sensorNameRhr[REALSENSORNUM]={ 
    "lHipB","lHipF","lKnee","lLegupperL","lLegupperF","lLegupperR","lAnkleF","lLeglowerR","lLeglowerF","lLeglowerL","lFootpalmFL","lFoottopL","lFootpalmBL","lFoottopB","lFootpalmFR","lFoottopR","lAnkleB","lFootpalmBR","rHipF","rHipB","rLegupperL","rLegupperF","rLegupperR","rKnee","rLeglowerR","rLeglowerF","rLeglowerL","rAnkleF","rFootpalmFL","rFoottopL","rFootpalmBL","rAnkleB","rFootpalmFR","rFoottopR","rFootpalmBR","rFoottopB","lShoulderT","lShoulderL","lArmupperF","lElbow","lArmupperB","lHandtopB","lHandtopT","lArmlowerF","lArmlowerB","lHandpalmB","lHandpalmT","rShoulderT","rShoulderR","rArmupperF","rElbow","rArmupperB","rHandtopB","rHandtopT","rArmlowerF","rArmlowerB","rHandpalmB","rHandpalmT","lSideB","lSideC","lSideF","BackupperL","BacklowerL","rSideF","rSideC","rSideB","BacklowerR","BackupperR","ChestL","ChestR","AbdomenL","AbdomenR","AbdomenC","lRib","rRib","ForeheadR","ForeheadL","ChinL","ChinR","rTemple","rJaw","lJaw","lTemple","HeadtopL","HeadtopC","Headback","HeadtopR","lEar","Nape","rEar",
#ifdef ADDITIONAL_SENSORS
"lLittlefinger","rLittlefinger"
#endif
  };
}

namespace Ssensor {
  enum sensorNameS {
    BodyG,BodyT,BodyB,BodyR,BodyF,BodyL,Un1G,Un1T,Un1B,Un1R,Un1F,Un1L,HeadG,HeadT,HeadB,HeadR,HeadF,HeadL,lShoulderG,lShoulderT,lShoulderB,lShoulderR,lShoulderF,lShoulderL,lArmupperG,lArmupperT,lArmupperB,lArmupperR,lArmupperF,lArmupperL,lArmlowerG,lArmlowerT,lArmlowerB,lArmlowerR,lArmlowerF,lArmlowerL,lHandGG,lHandGT,lHandGB,lHandGR,lHandGF,lHandGL,lHandFG,lHandFT,lHandFB,lHandFR,lHandFF,lHandFL,lHandBG,lHandBT,lHandBB,lHandBR,lHandBF,lHandBL,lHipBG,lHipBT,lHipBB,lHipBR,lHipBF,lHipBL,Un2G,Un2T,Un2B,Un2R,Un2F,Un2L,lHipFG,lHipFT,lHipFB,lHipFR,lHipFF,lHipFL,lLegupperFG,lLegupperFT,lLegupperFB,lLegupperFR,lLegupperFF,lLegupperFL,lLegupperBG,lLegupperBT,lLegupperBB,lLegupperBR,lLegupperBF,lLegupperBL,lKneeG,lKneeT,lKneeB,lKneeR,lKneeF,lKneeL,lLeglowerFG,lLeglowerFT,lLeglowerFB,lLeglowerFR,lLeglowerFF,lLeglowerFL,lLeglowerBG,lLeglowerBT,lLeglowerBB,lLeglowerBR,lLeglowerBF,lLeglowerBL,lAnkleFG,lAnkleFT,lAnkleFB,lAnkleFR,lAnkleFF,lAnkleFL,lAnkleCG,lAnkleCT,lAnkleCB,lAnkleCR,lAnkleCF,lAnkleCL,lAnkleBG,lAnkleBT,lAnkleBB,lAnkleBR,lAnkleBF,lAnkleBL,lFootG,lFootT,lFootB,lFootR,lFootF,lFootL,rShoulderG,rShoulderT,rShoulderB,rShoulderR,rShoulderF,rShoulderL,rArmupperG,rArmupperT,rArmupperB,rArmupperR,rArmupperF,rArmupperL,rArmlowerG,rArmlowerT,rArmlowerB,rArmlowerR,rArmlowerF,rArmlowerL,rHandGG,rHandGT,rHandGB,rHandGR,rHandGF,rHandGL,rHandFG,rHandFT,rHandFB,rHandFR,rHandFF,rHandFL,rHandBG,rHandBT,rHandBB,rHandBR,rHandBF,rHandBL,rHipBG,rHipBT,rHipBB,rHipBR,rHipBF,rHipBL,rHipCG,rHipCT,rHipCB,rHipCR,rHipCF,rHipCL,rHipFG,rHipFT,rHipFB,rHipFR,rHipFF,rHipFL,rLegupperFG,rLegupperFT,rLegupperFB,rLegupperFR,rLegupperFF,rLegupperFL,rLegupperBG,rLegupperBT,rLegupperBB,rLegupperBR,rLegupperBF,rLegupperBL,rKneeG,rKneeT,rKneeB,rKneeR,rKneeF,rKneeL,rLeglowerFG,rLeglowerFT,rLeglowerFB,rLeglowerFR,rLeglowerFF,rLeglowerFL,rLeglowerBG,rLeglowerBT,rLeglowerBB,rLeglowerBR,rLeglowerBF,rLeglowerBL,rAnkleFG,rAnkleFT,rAnkleFB,rAnkleFR,rAnkleFF,rAnkleFL,rAnkleCG,rAnkleCT,rAnkleCB,rAnkleCR,rAnkleCF,rAnkleCL,rAnkleBG,rAnkleBT,rAnkleBB,rAnkleBR,rAnkleBF,rAnkleBL,rFootG,rFootT,rFootB,rFootR,rFootF,rFootL,NeckG,NeckT,NeckB,NeckR,NeckF,NeckL,SIMULATEDSENSORNUM
  };
  
  static const char* sensorNameShr[SIMULATEDSENSORNUM]={"BodyG","BodyT","BodyB","BodyR","BodyF","BodyL","Un1G","Un1T","Un1B","Un1R","Un1F","Un1L","HeadG","HeadT","HeadB","HeadR","HeadF","HeadL","lShoulderG","lShoulderT","lShoulderB","lShoulderR","lShoulderF","lShoulderL","lArmupperG","lArmupperT","lArmupperB","lArmupperR","lArmupperF","lArmupperL","lArmlowerG","lArmlowerT","lArmlowerB","lArmlowerR","lArmlowerF","lArmlowerL","lHandGG","lHandGT","lHandGB","lHandGR","lHandGF","lHandGL","lHandFG","lHandFT","lHandFB","lHandFR","lHandFF","lHandFL","lHandBG","lHandBT","lHandBB","lHandBR","lHandBF","lHandBL","lHipBG","lHipBT","lHipBB","lHipBR","lHipBF","lHipBL","Un2G","Un2T","Un2B","Un2R","Un2F","Un2L","lHipFG","lHipFT","lHipFB","lHipFR","lHipFF","lHipFL","lLegupperFG","lLegupperFT","lLegupperFB","lLegupperFR","lLegupperFF","lLegupperFL","lLegupperBG","lLegupperBT","lLegupperBB","lLegupperBR","lLegupperBF","lLegupperBL","lKneeG","lKneeT","lKneeB","lKneeR","lKneeF","lKneeL","lLeglowerFG","lLeglowerFT","lLeglowerFB","lLeglowerFR","lLeglowerFF","lLeglowerFL","lLeglowerBG","lLeglowerBT","lLeglowerBB","lLeglowerBR","lLeglowerBF","lLeglowerBL","lAnkleFG","lAnkleFT","lAnkleFB","lAnkleFR","lAnkleFF","lAnkleFL","lAnkleCG","lAnkleCT","lAnkleCB","lAnkleCR","lAnkleCF","lAnkleCL","lAnkleBG","lAnkleBT","lAnkleBB","lAnkleBR","lAnkleBF","lAnkleBL","lFootG","lFootT","lFootB","lFootR","lFootF","lFootL","rShoulderG","rShoulderT","rShoulderB","rShoulderR","rShoulderF","rShoulderL","rArmupperG","rArmupperT","rArmupperB","rArmupperR","rArmupperF","rArmupperL","rArmlowerG","rArmlowerT","rArmlowerB","rArmlowerR","rArmlowerF","rArmlowerL","rHandGG","rHandGT","rHandGB","rHandGR","rHandGF","rHandGL","rHandFG","rHandFT","rHandFB","rHandFR","rHandFF","rHandFL","rHandBG","rHandBT","rHandBB","rHandBR","rHandBF","rHandBL","rHipBG","rHipBT","rHipBB","rHipBR","rHipBF","rHipBL","rHipCG","rHipCT","rHipCB","rHipCR","rHipCF","rHipCL","rHipFG","rHipFT","rHipFB","rHipFR","rHipFF","rHipFL","rLegupperFG","rLegupperFT","rLegupperFB","rLegupperFR","rLegupperFF","rLegupperFL","rLegupperBG","rLegupperBT","rLegupperBB","rLegupperBR","rLegupperBF","rLegupperBL","rKneeG","rKneeT","rKneeB","rKneeR","rKneeF","rKneeL","rLeglowerFG","rLeglowerFT","rLeglowerFB","rLeglowerFR","rLeglowerFF","rLeglowerFL","rLeglowerBG","rLeglowerBT","rLeglowerBB","rLeglowerBR","rLeglowerBF","rLeglowerBL","rAnkleFG","rAnkleFT","rAnkleFB","rAnkleFR","rAnkleFF","rAnkleFL","rAnkleCG","rAnkleCT","rAnkleCB","rAnkleCR","rAnkleCF","rAnkleCL","rAnkleBG","rAnkleBT","rAnkleBB","rAnkleBR","rAnkleBF","rAnkleBL","rFootG","rFootT","rFootB","rFootR","rFootF","rFootL","NeckG","NeckT","NeckB","NeckR","NeckF","NeckL"
  };

}


/*static const char* JointNames[22]={"head","neck",
				     "rshoulder","lshoulder",
				     "rarm","larm",
				     "rhand","lhand",
				     "rhip","lhip",
				     "rhipsquare","lhipsquare",
				     "rhiptriangle","lhiptriangle",
				     "rupperleg","lupperleg",
				     "rlowerleg","llowerleg",
				     "rfootsquare","lfootsquare",
				     "rfoot","lfoot"};*/

static const char* JointNames[22]={"neckP","neckY",
				     "rShoulderP","lShoulderP",
				     "rShoulderR","lShoulderR",
				     "rElbowR","lElbowR",
				     "rHipY","lHipY",
				     "rHipR","lHipR",
				     "rHipP","lHipP",
				     "rKneeUP","lKneeUP",
				     "rKneeLP","lKneeLP",
				     "rAnkeP","lAnkleP",
				     "rAnkleR","lAnkleR"};


namespace V4G{
  static const int R=0;
  static const int L=1;

  static const int HEAD=0;
  static const int NECK=1;
  static const int SHOULDER=2;
  static const int ARM=4;
  static const int HAND=6;

  static const int HIP=8;
  static const int HIPSQUARE=10;
  static const int HIPTRIANGLE=12;
  static const int UPPERLEG=14;
  static const int LOWERLEG=16;

  static const int FOOTSQUARE=18;
  static const int FOOT=20;
  static const int NUMJOINTS=22;
};


class BigHead{
  Vector3d size;
  Vector3d eyeSize;
  Vector3d reyePos;
  Vector3d leyePos;
  Element reye;
  Element leye;
  Appearance eyeApp;

 public:
  BigHead(Robot* r,Part* parts);
};

class V4Gstructure: public Robot
{
  AutoDestroy ad;
 public:
  double * sensors; //mouse sensors to be set for displaying
private:
  double * motors;
  bool externMotors;
  bool binaryForceSensors;
 public:
  static const int SERVOSNUM=22;
  enum PartID {pBody,
	       pNeck,pHead,
		 pLshoulder,pLarm,pLhand,pLhip,pLhipSquare,pLhipTriangle,pLupperLegFront,pLupperLegBack,
		 pLknee,pLlowerLegFront,pLlowerLegBack,pLfootTriangle,pLfootSquare,pLfoot,
		 pRshoulder,pRarm,pRhand,pRhip,pRhipSquare,pRhipTriangle,pRupperLegFront,pRupperLegBack,
	       pRknee,pRlowerLegFront,pRlowerLegBack,pRfootTriangle,pRfootSquare,pRfoot,PARTSNUM
  };
  enum BoxIDs {eBody,
	       eNeck,eHead,
	       eLshoulder,eLarm,eLhand,eLhandTop,eLhandIn,eLhandOut,
	       eLhip,eLhipSquare,eLhipTriangle,eLupperLegFront,eLupperLegBack,
	       eLknee,eLlowerLegFront,eLlowerLegBack,eLfootTriangle,eLfootSquare,eLfootVertical,eLfootHorizontal,

	       eRshoulder,eRarm,eRhand,eRhandTop,eRhandIn,eRhandOut,
	       eRhip,eRhipSquare,eRhipTriangle,eRupperLegFront,eRupperLegBack,
	       eRknee,eRlowerLegFront,eRlowerLegBack,eRfootTriangle,eRfootSquare,eRfootVertical,eRfootHorizontal,
#ifdef BABYVERSION
	       eNeckBase,
#endif	       
	       BOXESNUM
  };

  enum NodeIDs {nBody,nNeck,nHead,
		 nLpreShoulder, nLshoulder,nLarm,nLhand,nLhip,nLhipSquare,nLhipTriangle,
		 nLupperLegFront,nLupperLegBack,nLkneeUpperFront,nLkneeUpperBack,
		 nLlowerLegFront,nLlowerLegBack,nLfootTriangleFront,nLfootTriangleBack,nLfootSquare,nLfoot,
		 nRpreShoulder, nRshoulder,nRarm,nRhand,nRhip,nRhipSquare,nRhipTriangle,
		 nRupperLegFront,nRupperLegBack,nRkneeUpperFront,nRkneeUpperBack,
		nRlowerLegFront,nRlowerLegBack,nRfootTriangleFront,nRfootTriangleBack,nRfootSquare,nRfoot,NODESNUM
  };

  enum ServoSizeID {SSIX,SSIY,SSIZ};

  static Vector3d servoPos[SERVOSNUM];  
  static Element servoBox[SERVOSNUM];  
  static PartID servoPart[SERVOSNUM];
  static ServoSizeID servoSize[SERVOSNUM];

  Element eBoxes[BOXESNUM];
  static Vector3d boxSizes[BOXESNUM];
  static Vector3d boxPos[BOXESNUM];

  Appearance apps[BOXESNUM];
  Part parts[PARTSNUM];  
  Vector3d nodePos[NODESNUM];
  SkeletonNode nodes[NODESNUM];  
  bool useRealSensorNames;
  BigHead* bHead;
public:
  //sensors are mouse sensors
  V4Gstructure(double * mot,double *mouseSensors,bool pbinaryForceSensors=false,bool sphereTouchSensors=false);
  virtual ~V4Gstructure();
  bool * sphereTouchSensors;
  bool contactActivation[BOXESNUM*6];
  Vector3d forceVectors[BOXESNUM*6];
  Vector3d applicationPoints[BOXESNUM*6];
  double forces[BOXESNUM*6]; //dot product of the force and the normal
  bool withGround[BOXESNUM*6]; 
  void addRoundBack();
  void bigHead();
  void moveArm();
  void setTexture(GlTexture* tex);
  void secondaryColor();
  std::string getJointName(int joint) const{
    return JointNames[joint];
  }
  void setSensorNames(bool realSensors);
  
  std::string getSensorName(int sid) const{
    if (useRealSensorNames) 
      return Rsensor::sensorNameRhr[sid];
    else
      return Ssensor::sensorNameShr[sid];

  }
};


static const char* V4GelementNames[V4Gstructure::BOXESNUM]={
  "Body",
  "Neck","Head",
  "Lshoulder","Larm","Lhand","LhandTop","LhandIn","LhandOut",
  "Lhip","LhipSquare","LhipTriangle","LupperLegFront","LupperLegBack",
  "Lknee","LlowerLegFront","LlowerLegBack","LfootTriangle","LfootSquare","LfootVertical","LfootHorizontal",  

  "Rshoulder","Rarm","Rhand","RhandTop","RhandIn","RhandOut",
  "Rhip","RhipSquare","RhipTriangle","RupperLegFront","RupperLegBack",
  "Rknee","RlowerLegFront","RlowerLegBack","RfootTriangle","RfootSquare","RfootVertical","RfootHorizontal"
#ifdef BABYVERSION  
  ,"NeckBase"
#endif

};

inline std::string getElementName(int id){
  return V4GelementNames[id];
}

/*

class V4GmotorSymmetries:public MotorSymmetries{
 public:
  V4GmotorSymmetries();
};
*/
}
#endif
