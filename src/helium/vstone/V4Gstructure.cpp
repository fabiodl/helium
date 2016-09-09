#include <helium/vstone/V4Gstructure.h>


  using namespace std;

namespace helium{


  using namespace V4G;

Vector3d V4Gstructure::servoPos[SERVOSNUM];
Element V4Gstructure::servoBox[SERVOSNUM];

V4Gstructure::PartID V4Gstructure::servoPart[SERVOSNUM];
V4Gstructure::ServoSizeID V4Gstructure::servoSize[SERVOSNUM];

Vector3d V4Gstructure::boxSizes[BOXESNUM];
Vector3d V4Gstructure::boxPos[BOXESNUM];


static const double COORDINATESCALE=0.001;


V4Gstructure::V4Gstructure(double * mot,double* psensors,bool pbinaryForceSensors,bool sTouchSensors):
  sensors(psensors),binaryForceSensors(pbinaryForceSensors),useRealSensorNames(true),
  bHead(NULL)
{

  if (mot==NULL){
    motors=new double[SERVOSNUM];
    for (int i=0;i<SERVOSNUM;i++) motors[i]=0;    
    externMotors=false;
  }
  else{
    motors=mot;
    externMotors=true;
  }

  for (int i=0;i<BOXESNUM;i++)
    eBoxes[i].id=i;
    
  for (int i=0;i<PARTSNUM;i++)
    parts[i].id=i;

  for (int i=0;i<NODESNUM;i++)
    nodes[i].id=i;

  for (int i=0;i<SERVOSNUM;i++)
    servoBox[i].id=-1;
  
  apps[eBody].setColor(0.16f,0.31f,0.65f);

#ifdef BABYVERSION
  apps[eNeck].setColor(0.16f,0.31f,0.65f);
  apps[eNeckBase].setColor(0.98f,0.79f,0.28f);  
#else
  apps[eNeck].setColor(0.98f,0.79f,0.28f);  
#endif
  apps[eHead].setColor(0.92f,0.98f,0.98f);
  apps[eLshoulder].setColor(0.96f,0.80f,0.38f);
  apps[eRshoulder].setColor(0.96f,0.80f,0.38f);
  apps[eLarm].setColor(0.87f,1,0.94f);
  apps[eRarm].setColor(0.87f,1,0.94f);
  apps[eLhand].setColor(0.92f,1,0.98f);
  apps[eLhandTop].setColor(0.92f,1,0.98f);
  apps[eLhandIn].setColor(0.92f,1,0.98f);
  apps[eLhandOut].setColor(0.92f,1,0.98f);
  apps[eRhand].setColor(0.92f,1,0.98f);
  apps[eRhandTop].setColor(0.92f,1,0.98f);
  apps[eRhandIn].setColor(0.92f,1,0.98f);
  apps[eRhandOut].setColor(0.92f,1,0.98f);
  
  apps[eLhip].setColor(0.9f,0.24f,0.21f);
  apps[eRhip].setColor(0.9f,0.24f,0.21f);
  apps[eLhipSquare].setColor(0.33f,0.45f,0.50f);
  apps[eRhipSquare].setColor(0.33f,0.45f,0.50f);  
  apps[eLhipTriangle].setColor(0.98f,0.86f,0.30f);
  apps[eRhipTriangle].setColor(0.98f,0.86f,0.30f);
  apps[eLupperLegFront].setColor(0.92f,1,0.98f);
  apps[eRupperLegFront].setColor(0.92f,1,0.98f);
  apps[eLupperLegBack].setColor(0.22f,0.28f,0.45f);
  apps[eRupperLegBack].setColor(0.22f,0.28f,0.45f);
  apps[eLknee].setColor(0.5f,0.64f,0.7f);
  apps[eRknee].setColor(0.5f,0.64f,0.7f);
  apps[eLlowerLegFront].setColor(0.92f,1,0.98f);
  apps[eRlowerLegFront].setColor(0.92f,1,0.98f);
  apps[eLlowerLegBack].setColor(0.22f,0.28f,0.45f);
  apps[eRlowerLegBack].setColor(0.22f,0.28f,0.45f);
  apps[eLfootTriangle].setColor(0.33f,0.45f,0.50f);
  apps[eRfootTriangle].setColor(0.33f,0.45f,0.50f);
  apps[eLfootSquare].setColor(0.98f,0.86f,0.30f);
  apps[eRfootSquare].setColor(0.98f,0.86f,0.30f);
  apps[eLfootVertical].setColor(0.87f,1,0.96f);
  apps[eRfootVertical].setColor(0.87f,1,0.96f);
  apps[eLfootHorizontal].setColor(0.86f,0.27f,0.22f);
  apps[eRfootHorizontal].setColor(0.86f,0.27f,0.22f);

  //-----MOUSE SENSORS-----
  if (sensors!=NULL){
    for(int i=0;i<BOXESNUM;i++){
      eBoxes[i].setElementCustomization(ad.s=new MouseSensorBoxCustomization(6*i,&sensors[6*i]),
					MOUSESENSORBOXCUSTOMIZATION);
    }
        
    for (int i=0;i<BOXESNUM*6;i++){
      sensors[i]=0;
    }
  }//if sensors!=NULL


//----FORCE SENSORS -----
  for (int i=0;i<BOXESNUM;i++){
    if (binaryForceSensors)
      
      eBoxes[i].setElementCustomization(ad.s=new BinarySensorBoxCustomization(6*i,&contactActivation[6*i]),BINARYSENSORBOXCUSTOMIZATION);
    else{
      eBoxes[i].setElementCustomization(ad.s=new ForceSensorBoxCustomization(6*i,&applicationPoints[6*i],&forceVectors[6*i],&forces[6*i],&withGround[6*i]),FORCESENSORBOXCUSTOMIZATION);      
    }    
  }

  for (int i=0;i<BOXESNUM*6;i++){    
    contactActivation[i]=false;
    forceVectors[i].zero();
    applicationPoints[i].zero();
    forces[i]=0;
    withGround[i]=false;    
  }

  double handFrameSize=10;
  boxSizes[eBody].set(107,95,109);

#ifdef BABYVERSION
  boxSizes[eNeck].set(51.5,23,35);
  boxSizes[eNeckBase].set(40,24,42.4);
#else
  boxSizes[eNeck].set(24,40,64);
#endif

  boxSizes[eHead].set(55,42,49);

  boxSizes[eLshoulder].set(44,44,45);
  boxSizes[eLarm].set(41,53,92);
  boxSizes[eLhand].set(35,53,127-70+handFrameSize);

  boxSizes[eLhandTop].set(10,53-12,handFrameSize);
  boxSizes[eLhandIn].set(10,handFrameSize,70-handFrameSize);
  boxSizes[eLhandOut].set(10,handFrameSize,70-handFrameSize);
  

  boxSizes[eLhip].set(34,52,58.5);
  boxSizes[eLhipSquare].set(43,43,43);
  boxSizes[eLhipTriangle].set(52,59.5,40);
  boxSizes[eLupperLegFront].set(57,25,83.5);
  boxSizes[eLupperLegBack].set(57,12,78);
  boxSizes[eLknee].set(43.5,43.5,47);
  boxSizes[eLlowerLegFront].set(boxSizes[eLupperLegFront]);
  boxSizes[eLlowerLegBack].set(boxSizes[eLupperLegBack]);
  boxSizes[eLfootTriangle].set(boxSizes[eLhipTriangle]);
  boxSizes[eLfootSquare].set(boxSizes[eLhipSquare]);
  boxSizes[eLfootVertical].set(34.5,86,56);
  boxSizes[eLfootHorizontal].set(75,110,6);

  boxSizes[eRshoulder].set(boxSizes[eLshoulder]);
  boxSizes[eRarm].set(boxSizes[eLarm]);
  boxSizes[eRhand].set(boxSizes[eLhand]);
  boxSizes[eRhandTop].set(boxSizes[eLhandTop]);
  boxSizes[eRhandIn].set(boxSizes[eLhandIn]);
  boxSizes[eRhandOut].set(boxSizes[eLhandOut]);
    
  boxSizes[eRhip].set(boxSizes[eLhip]);
  boxSizes[eRhipSquare].set(boxSizes[eLhipSquare]);
  boxSizes[eRhipTriangle].set(boxSizes[eLhipTriangle]);
  boxSizes[eRupperLegFront].set(boxSizes[eLupperLegFront]);
  boxSizes[eRupperLegBack].set(boxSizes[eLupperLegBack]);
  boxSizes[eRknee].set(boxSizes[eLknee]);
  boxSizes[eRlowerLegFront].set(boxSizes[eLlowerLegFront]);
  boxSizes[eRlowerLegBack].set(boxSizes[eLlowerLegBack]);
  boxSizes[eRfootTriangle].set(boxSizes[eLfootTriangle]);
  boxSizes[eRfootSquare].set(boxSizes[eLfootSquare]);
  boxSizes[eRfootVertical].set(boxSizes[eLfootVertical]);
  boxSizes[eRfootHorizontal].set(boxSizes[eLfootHorizontal]);

  boxPos[eBody].set(0,0,0);
#ifdef BABYVERSION
  boxPos[eNeckBase].set(0,10-21,boxSizes[eNeckBase].z()/2+boxSizes[eBody].z()/2);
  boxPos[eNeck].set(0,0,boxSizes[eNeck].z()/2-10);
#else
  boxPos[eNeck].set(0,10-21,32);
#endif
  boxPos[eHead].set(0,44,-5.5);

  boxPos[eLshoulder].set(-22,-boxSizes[eLshoulder].y()/2+10,-boxSizes[eLshoulder].z()/2+10);
  boxPos[eLarm].set(-10.5,26.5,-36);
  boxPos[eLhand].set(-7.5,26.5,-53.5+32);
  boxPos[eLhandTop].set(-7.5-35.0/2+5,26.5,-53.5 -127.0/2-3+handFrameSize );
  boxPos[eLhandIn].set(-7.5 -35.0/2+5,26.5+53/2-3,-53.5 -127.0/2+32 );
  boxPos[eLhandOut].set(-7.5 -35.0/2+5,26.5-53/2+3,-53.5 -127.0/2+32);

  boxPos[eLhip].set(-5,0,-34.25);
  boxPos[eLhipSquare].set(-9.5,21.5,9.5);
  boxPos[eLhipTriangle].set(-26,-2.25,-1);
  boxPos[eLupperLegFront].set(-28.5,1.5,-37.75);
  boxPos[eLupperLegBack].set(-28.5,-1,-35);
  boxPos[eLknee].set(-21.75,-9.75,-11.5);
  boxPos[eLlowerLegFront].set(-28.5,-2.5,-31.75);
  boxPos[eLlowerLegBack].set(-28.5,2,-35);
  boxPos[eLfootTriangle].set(-26,-6.25,-2.5);
  boxPos[eLfootSquare].set(-21.5,-10.5,9.5);
  boxPos[eLfootVertical].set(-7.25,5,-18);
  boxPos[eLfootHorizontal].set(-14.5,32,-46);

  boxPos[eRshoulder].setNegX(boxPos[eLshoulder]);
  boxPos[eRarm].setNegX(boxPos[eLarm]);
  boxPos[eRhand].setNegX(boxPos[eLhand]);
  boxPos[eRhandTop].setNegX(boxPos[eLhandTop]);
  boxPos[eRhandIn].setNegX(boxPos[eLhandIn]);
  boxPos[eRhandOut].setNegX(boxPos[eLhandOut]);

  boxPos[eRhip].setNegX(boxPos[eLhip]);
  boxPos[eRhipSquare].setNegX(boxPos[eLhipSquare]);
  boxPos[eRhipTriangle].setNegX(boxPos[eLhipTriangle]);
  boxPos[eRupperLegFront].setNegX(boxPos[eLupperLegFront]);
  boxPos[eRupperLegBack].setNegX(boxPos[eLupperLegBack]);
  boxPos[eRknee].setNegX(boxPos[eLknee]);
  boxPos[eRlowerLegFront].setNegX(boxPos[eLlowerLegFront]);
  boxPos[eRlowerLegBack].setNegX(boxPos[eLlowerLegBack]);
  boxPos[eRfootTriangle].setNegX(boxPos[eLfootTriangle]);
  boxPos[eRfootSquare].setNegX(boxPos[eLfootSquare]);
  boxPos[eRfootVertical].setNegX(boxPos[eLfootVertical]);
  boxPos[eRfootHorizontal].setNegX(boxPos[eLfootHorizontal]);
    
  PartID elementPart[BOXESNUM];
  elementPart[eBody]=pBody;
#ifdef BABYVERSION
  elementPart[eNeckBase]=pBody;
#endif
  elementPart[eNeck]=pNeck;
  elementPart[eHead]=pHead;
  elementPart[eLshoulder]=pLshoulder;
  elementPart[eLarm]=pLarm;
  elementPart[eLhand]=pLhand;
  elementPart[eLhandTop]=pLhand;
  elementPart[eLhandIn]=pLhand;
  elementPart[eLhandOut]=pLhand;
  elementPart[eLhip]=pLhip;
  elementPart[eLhipSquare]=pLhipSquare;
  elementPart[eLhipTriangle]=pLhipTriangle;
  elementPart[eLupperLegFront]=pLupperLegFront;
  elementPart[eLupperLegBack]=pLupperLegBack;
  elementPart[eLknee]=pLknee;
  elementPart[eLlowerLegFront]=pLlowerLegFront;
  elementPart[eLlowerLegBack]=pLlowerLegBack;
  elementPart[eLfootTriangle]=pLfootTriangle;
  elementPart[eLfootSquare]=pLfootSquare;
  elementPart[eLfootHorizontal]=pLfoot;
  elementPart[eLfootVertical]=pLfoot;

  elementPart[eRshoulder]=pRshoulder;
  elementPart[eRarm]=pRarm;
  elementPart[eRhand]=pRhand;
  elementPart[eRhandTop]=pRhand;
  elementPart[eRhandIn]=pRhand;
  elementPart[eRhandOut]=pRhand;
  elementPart[eRhip]=pRhip;
  elementPart[eRhipSquare]=pRhipSquare;
  elementPart[eRhipTriangle]=pRhipTriangle;
  elementPart[eRupperLegFront]=pRupperLegFront;
  elementPart[eRupperLegBack]=pRupperLegBack;
  elementPart[eRknee]=pRknee;
  elementPart[eRlowerLegFront]=pRlowerLegFront;
  elementPart[eRlowerLegBack]=pRlowerLegBack;
  elementPart[eRfootTriangle]=pRfootTriangle;
  elementPart[eRfootSquare]=pRfootSquare;
  elementPart[eRfootHorizontal]=pRfoot;
  elementPart[eRfootVertical]=pRfoot;

  for (int i=0;i<BOXESNUM;i++){
    parts[elementPart[i]].addElement((Element*)&eBoxes[i]);
  }

  nodePos[nBody].set(0,0,0);
#ifdef BABYVERSION
  nodePos[nNeck].set(boxPos[eNeckBase].x(),boxPos[eNeckBase].y(),boxPos[eNeckBase].z()+boxSizes[eNeckBase].z()/2-11);
  nodePos[nHead].set(0,0,boxSizes[eNeck].z()-11);
#else
  nodePos[nNeck].set(0,-6.5,56.5);
  nodePos[nHead].set(0,1,52);
#endif

  nodePos[nLpreShoulder].set(-53.5,4.5,34.5);
  nodePos[nLshoulder].set(-4,0,0);
  nodePos[nLarm].set(-12,-36.5,-21);
  nodePos[nLhand].set(0,0,-72);
  nodePos[nLhip].set(-38.5,-9.5,-54.5);
  nodePos[nLhipSquare].set(+12,-21.5,-53.5);
  nodePos[nLhipTriangle].set(+16.5,32,19);
  nodePos[nLupperLegFront].set(+2.5,4,-12.5);
  nodePos[nLupperLegBack].set(+2.5,-25,-16);
  nodePos[nLkneeUpperFront].set(-6.75,-1,-69.5);
  nodePos[nLkneeUpperBack].set(0,0,-70);
  nodePos[nLlowerLegFront].set(+6.75,0,-23);
  nodePos[nLlowerLegBack].set(+6.75,-29,-19.5);
  nodePos[nLfootTriangleFront].set(-2.5,-4,-69.5);
  nodePos[nLfootTriangleBack].set(0,0,-70);

  nodePos[nLfootSquare].set(-4.5,-4,-12.5);
  nodePos[nLfoot].set(-11.5,-32,19.5);

  nodePos[nRpreShoulder].setNegX(nodePos[nLpreShoulder]);
  nodePos[nRshoulder].setNegX(nodePos[nLshoulder]);
  nodePos[nRarm].setNegX(nodePos[nLarm]);
  nodePos[nRhand].setNegX(nodePos[nLhand]);
  nodePos[nRhip].setNegX(nodePos[nLhip]);
  nodePos[nRhipSquare].setNegX(nodePos[nLhipSquare]);
  nodePos[nRhipTriangle].setNegX(nodePos[nLhipTriangle]);
  nodePos[nRupperLegFront].setNegX(nodePos[nLupperLegFront]);
  nodePos[nRupperLegBack].setNegX(nodePos[nLupperLegBack]);
  nodePos[nRkneeUpperFront].setNegX(nodePos[nLkneeUpperFront]);
  nodePos[nRkneeUpperBack].setNegX(nodePos[nLkneeUpperBack]);
  nodePos[nRlowerLegFront].setNegX(nodePos[nLlowerLegFront]);
  nodePos[nRlowerLegBack].setNegX(nodePos[nLlowerLegBack]);
  nodePos[nRfootTriangleFront].setNegX(nodePos[nLfootTriangleFront]);
  nodePos[nRfootTriangleBack].setNegX(nodePos[nLfootTriangleBack]);
  nodePos[nRfootSquare].setNegX(nodePos[nLfootSquare]);
  nodePos[nRfoot].setNegX(nodePos[nLfoot]);

  Vector3d* servoSizeX=ad.s=new Vector3d(21,41,41);
  Vector3d* servoSizeY=ad.s=new Vector3d(41,21,41);
  Vector3d* servoSizeZ=ad.s=new Vector3d(41,41,21);
    
  servoSizeX->mulScalar(COORDINATESCALE);
  servoSizeY->mulScalar(COORDINATESCALE);
  servoSizeZ->mulScalar(COORDINATESCALE);

  Appearance* app=ad.s=new Appearance(0,0,0);

  for (int i=0;i<SERVOSNUM;i++){
    servoPos[i].set(0,0,0);
    servoPart[i]=(V4Gstructure::PartID)i;
    servoSize[i]=SSIX;
  }

  static const double HALFBIG=41.0/2;
  static const double HALFLITTLE=21.0/2;
  static const double IDENT=12/2;


#ifdef BABYVERSION
  
  servoPart[V4G::HEAD]=pHead;
  servoPos[V4G::HEAD].set(0,(HALFBIG-IDENT*2),HALFBIG);
  servoSize[V4G::HEAD]=SSIX;

  servoPart[V4G::NECK]=pBody;
  servoPos[V4G::NECK].set(boxPos[eNeckBase].x(),boxPos[eNeckBase].y(),boxPos[eNeckBase].z());
  servoSize[V4G::NECK]=SSIY;

#else
  servoPart[V4G::HEAD]=pNeck;
  servoPos[V4G::HEAD].set(0,(-HALFBIG+IDENT-3),44+HALFLITTLE);
  servoSize[V4G::HEAD]=SSIZ;

  servoPart[V4G::NECK]=pNeck;
  servoPos[V4G::NECK].set(0,-HALFBIG+IDENT,+HALFBIG);
  servoSize[V4G::NECK]=SSIX;
#endif
  servoPart[V4G::SHOULDER+V4G::R]=pRshoulder;
  servoPos[V4G::SHOULDER+V4G::R].set(HALFBIG,0,-(HALFBIG-IDENT));
  servoSize[V4G::SHOULDER+V4G::R]=SSIY;
  
  servoPart[V4G::SHOULDER+V4G::L]=pLshoulder;
  servoPos[V4G::SHOULDER+V4G::L].setNegX(servoPos[V4G::SHOULDER+V4G::R]);
  servoSize[V4G::SHOULDER+V4G::L]=servoSize[V4G::SHOULDER+V4G::R];

  servoPart[V4G::ARM+V4G::R]=pRshoulder;
  servoPos[V4G::ARM+V4G::R].set(HALFBIG,-2*HALFLITTLE,-(HALFBIG-IDENT));
  servoSize[V4G::ARM+V4G::R]=SSIY;
  
  servoPart[V4G::ARM+V4G::L]=pLshoulder;
  servoPos[V4G::ARM+V4G::L].setNegX(servoPos[V4G::ARM+V4G::R]);
  servoSize[V4G::ARM+V4G::L]=servoSize[V4G::ARM+V4G::R];
  
  servoPart[V4G::HAND+V4G::R]=pRarm;
  servoPos[V4G::HAND+V4G::R].set(-IDENT-1+boxSizes[V4G::ARM].x()/2,boxSizes[V4G::ARM].y()/2,-boxSizes[V4G::ARM].z()+2*HALFLITTLE);
  servoSize[V4G::HAND+V4G::R]=SSIZ;
  
  servoPart[V4G::HAND+V4G::L]=pLarm;
  servoPos[V4G::HAND+V4G::L].setNegX(servoPos[V4G::HAND+V4G::R]);
  servoSize[V4G::HAND+V4G::L]=servoSize[V4G::HAND+V4G::R];
  
  servoPart[V4G::HIP+V4G::R]=pBody;
  servoPos[V4G::HIP+V4G::R].set(nodePos[nRhip].x()-HALFBIG+IDENT,nodePos[nRhip].y(),nodePos[nRhip].z()+HALFBIG);
  servoSize[V4G::HIP+V4G::R]=SSIY;

  servoPart[V4G::HIP+V4G::L]=pBody;
  servoPos[V4G::HIP+V4G::L].setNegX(servoPos[V4G::HIP+V4G::R]);
  servoSize[V4G::HIP+V4G::L]=servoSize[V4G::HIP+V4G::R];

  servoPart[V4G::HIPSQUARE+V4G::R]=pRhipSquare;
  servoPos[V4G::HIPSQUARE+V4G::R].set(boxPos[eRhipSquare].x(),boxPos[eRhipSquare].y(),boxPos[eRhipSquare].z()-HALFLITTLE);
  servoSize[V4G::HIPSQUARE+V4G::R]=SSIZ;

  servoPart[V4G::HIPSQUARE+V4G::L]=pLhipSquare;
  servoPos[V4G::HIPSQUARE+V4G::L].setNegX(servoPos[V4G::HIPSQUARE+V4G::R]);
  servoSize[V4G::HIPSQUARE+V4G::L]=servoSize[V4G::HIPSQUARE+V4G::R];

  servoPart[V4G::HIPTRIANGLE+V4G::R]=pRhipSquare;
  servoPos[V4G::HIPTRIANGLE+V4G::R].set(boxPos[eRhipSquare].x(),boxPos[eRhipSquare].y(),boxPos[eRhipSquare].z()+HALFLITTLE);
  servoSize[V4G::HIPTRIANGLE+V4G::R]=SSIZ;

  servoPart[V4G::HIPTRIANGLE+V4G::L]=pLhipSquare;
  servoPos[V4G::HIPTRIANGLE+V4G::L].setNegX(servoPos[V4G::HIPTRIANGLE+V4G::R]);
  servoSize[V4G::HIPTRIANGLE+V4G::L]=servoSize[V4G::HIPTRIANGLE+V4G::R];

  servoPart[V4G::UPPERLEG+V4G::R]=pRknee;
  servoPos[V4G::UPPERLEG+V4G::R].set(boxPos[eRknee].x(),boxPos[eRknee].y(),boxPos[eRknee].z()+HALFLITTLE);
  servoSize[V4G::UPPERLEG+V4G::R]=SSIZ;

  servoPart[V4G::UPPERLEG+V4G::L]=pLknee;
  servoPos[V4G::UPPERLEG+V4G::L].setNegX(servoPos[V4G::UPPERLEG+V4G::R]);
  servoSize[V4G::UPPERLEG+V4G::L]=servoSize[V4G::UPPERLEG+V4G::R];
  
  servoPart[V4G::LOWERLEG+V4G::R]=pRknee;
  servoPos[V4G::LOWERLEG+V4G::R].set(boxPos[eRknee].x(),boxPos[eRknee].y(),boxPos[eRknee].z()-HALFLITTLE);
  servoSize[V4G::LOWERLEG+V4G::R]=SSIZ;

  servoPart[V4G::LOWERLEG+V4G::L]=pLknee;
  servoPos[V4G::LOWERLEG+V4G::L].setNegX(servoPos[V4G::LOWERLEG+V4G::R]);
  servoSize[V4G::LOWERLEG+V4G::L]=servoSize[V4G::LOWERLEG+V4G::R];

  servoPart[V4G::FOOTSQUARE+V4G::R]=pRfootSquare;
  servoPos[V4G::FOOTSQUARE+V4G::R].set(boxPos[eRfootSquare].x(),boxPos[eRfootSquare].y(),boxPos[eRfootSquare].z()-HALFLITTLE);
  servoSize[V4G::FOOTSQUARE+V4G::R]=SSIZ;

  servoPart[V4G::FOOTSQUARE+V4G::L]=pLfootSquare;
  servoPos[V4G::FOOTSQUARE+V4G::L].setNegX(servoPos[V4G::FOOTSQUARE+V4G::R]);
  servoSize[V4G::FOOTSQUARE+V4G::L]=servoSize[V4G::FOOTSQUARE+V4G::R];

  servoPart[V4G::FOOT+V4G::R]=pRfootSquare;
  servoPos[V4G::FOOT+V4G::R].set(boxPos[eRfootSquare].x(),boxPos[eRfootSquare].y(),boxPos[eRfootSquare].z()+HALFLITTLE);
  servoSize[V4G::FOOT+V4G::R]=SSIZ;

  servoPart[V4G::FOOT+V4G::L]=pLfootSquare;
  servoPos[V4G::FOOT+V4G::L].setNegX(servoPos[V4G::FOOT+V4G::R]);
  servoSize[V4G::FOOT+V4G::L]=servoSize[V4G::FOOT+V4G::R];

  for (int i=0;i<SERVOSNUM;i++){
    Vector3d * size;
    switch (servoSize[i]){
    case SSIX:
      size=servoSizeX;
      break;
    case SSIY:
      size=servoSizeY;
      break;
    case SSIZ:
      size=servoSizeZ;
      break;
    }    
    servoPos[i].mulScalar(COORDINATESCALE);
    servoBox[i].set(&servoPos[i],size,SERVOWEIGHT,app,WEIGHTINFO);
    parts[servoPart[i]].addElement(&servoBox[i]);
  }
  
  for (int i=0;i<BOXESNUM;i++){
    boxSizes[i].mulScalar(COORDINATESCALE);
    boxPos[i].mulScalar(COORDINATESCALE);
    eBoxes[i].set(&boxPos[i],&boxSizes[i],BOXWEIGHT,&apps[i],ENCUMBRANCEINFO|WEIGHTINFO);
    
  }

  eBoxes[eBody].m=TOTALWEIGHT-SERVOWEIGHT*SERVOSNUM-BOXWEIGHT*(BOXESNUM-1);

  for (int n=0;n<NODESNUM;n++) 
    nodePos[n].mulScalar(COORDINATESCALE);

  sphereTouchSensors=NULL;
  Appearance *stsApp=ad.s=new Appearance(0,1,1); 
  Vector3d *stsSize=ad.s=new Vector3d(0.004,0.004,0.004);
  Vector3d halfSize(*stsSize);
  halfSize.mulScalar(0.5);
  Vector3d zero(0,0,0);
  Vector3d p;
  if (sTouchSensors){
    sphereTouchSensors=ad.s=new bool[BOXESNUM*8];
    for(int b=0;b<BOXESNUM;b++){
      double vertex[24];
      Element box=eBoxes[b];
      Vector3d size(*box.size);
      size.sum(halfSize);
      getBoxVertex(vertex,size.x(),size.y(),size.z(),true);
      for (int v=0;v<8;v++){      
	sphereTouchSensors[b*8+v]=false;
	Vector3d* pos=ad.s=new Vector3d(vertex[v*3],vertex[v*3+1],vertex[v*3+2]);
	mul(&p,box.rotoTransl,&zero);

	pos->sum(p);
	Element * e=ad.s=new Element(pos,stsSize,
				0,stsApp,ENCUMBRANCEINFO,SPHERESHAPE);
	e->setElementCustomization(ad.s=new BinarySensorCustomization(&sphereTouchSensors[b*8+v]),BINARYSENSORCUSTOMIZATION);
	parts[elementPart[b]].addElement(e);
      }//v
    }//b
  }

  nodes[nBody].set(&nodePos[nBody],REFERENCEROTATION,
		   Axis::X,NULL,1,0,NULL,&parts[pBody]);
#ifdef BABYVERSION

  nodes[nNeck].set(&nodePos[nNeck],SERVOMOTOR,
		   Axis::X,&motors[V4G::HEAD],-1*1,0,&nodes[nBody],&parts[pNeck]);
  nodes[nNeck].setMotId(V4G::HEAD);
  nodes[nHead].set(&nodePos[nHead],SERVOMOTOR,
		   Axis::Z,&motors[V4G::NECK],-1*-1,0,&nodes[nNeck],&parts[pHead]);
  nodes[nHead].setMotId(V4G::NECK);
#else
  nodes[nNeck].set(&nodePos[nNeck],SERVOMOTOR,
		   Axis::Z,&motors[V4G::NECK],-1*-1,0,&nodes[nBody],&parts[pNeck]);
  nodes[nNeck].setMotId(V4G::NECK);
  nodes[nHead].set(&nodePos[nHead],SERVOMOTOR,
		   Axis::X,&motors[V4G::HEAD],-1*1,0,&nodes[nNeck],&parts[pHead]);
  nodes[nHead].setMotId(V4G::HEAD);
#endif


  nodes[nLpreShoulder].set(&nodePos[nLpreShoulder],REFERENCEROTATION,
			   Axis::Y,NULL,-1*1,M_PI/180*10,&nodes[nBody],NULL);
  nodes[nLshoulder].set(&nodePos[nLshoulder],SERVOMOTOR,
			Axis::X,&motors[V4G::SHOULDER+V4G::L],-1*1,M_PI/180*45,&nodes[nLpreShoulder],&parts[pLshoulder]); //2010.10.21 50->45
  nodes[nLshoulder].setMotId(V4G::SHOULDER+V4G::L);
  nodes[nLarm].set(&nodePos[nLarm],SERVOMOTOR,
		   Axis::Y,&motors[V4G::ARM+V4G::L],-1*-1,M_PI/2,&nodes[nLshoulder],&parts[pLarm]);
  nodes[nLarm].setMotId(V4G::ARM+V4G::L);
  nodes[nLhand].set(&nodePos[nLhand],SERVOMOTOR,
		    Axis::Y,&motors[V4G::HAND+V4G::L],-1*-1,-M_PI/180*30,&nodes[nLarm],&parts[pLhand]);//2010.10.21 45->30
  nodes[nLhand].setMotId(V4G::HAND+V4G::L);
  nodes[nLhip].set(&nodePos[nLhip],SERVOMOTOR,
		   Axis::Z,&motors[V4G::HIP+V4G::L],-1*1,0,&nodes[nBody],&parts[pLhip]);
  nodes[nLhip].setMotId(V4G::HIP+V4G::L);
  nodes[nLhipSquare].set(&nodePos[nLhipSquare],SERVOMOTOR,
			 Axis::Y,&motors[V4G::HIPSQUARE+V4G::L],-1*1,0,&nodes[nLhip],&parts[pLhipSquare]);
  nodes[nLhipSquare].setMotId(V4G::HIPSQUARE+V4G::L);
  nodes[nLhipTriangle].set(&nodePos[nLhipTriangle],SERVOMOTOR,
			   Axis::X,&motors[V4G::HIPTRIANGLE+V4G::L],-1*-1,0,&nodes[nLhipSquare],&parts[pLhipTriangle]);
  nodes[nLhipTriangle].setMotId(V4G::HIPTRIANGLE+V4G::L);
  nodes[nLupperLegFront].set(&nodePos[nLupperLegFront],FREEHINGE,
			     Axis::X,NULL,-1*-1,0,&nodes[nLhipTriangle],&parts[pLupperLegFront]);
  nodes[nLupperLegFront].setFakeServo(&motors[V4G::UPPERLEG+V4G::L]);//
  nodes[nLupperLegBack].set(&nodePos[nLupperLegBack],FREEHINGE,
  			       Axis::X,NULL,-1*-1,0,&nodes[nLhipTriangle],&parts[pLupperLegBack]);
  nodes[nLupperLegBack].setFakeServo(&motors[V4G::UPPERLEG+V4G::L]);//
  nodes[nLkneeUpperFront].set(&nodePos[nLkneeUpperFront],SERVOMOTOR,
			      Axis::X,&motors[V4G::UPPERLEG+V4G::L],-1*1,0,&nodes[nLupperLegFront],&parts[pLknee]);
  nodes[nLkneeUpperFront].setMotId(V4G::UPPERLEG+V4G::L);
  nodes[nLkneeUpperBack].set(&nodePos[nLkneeUpperBack],FREEHINGE,
			     Axis::X,NULL,-1*1,0,&nodes[nLupperLegBack],&parts[pLknee]);
  nodes[nLkneeUpperBack].setFakeServo(&motors[V4G::UPPERLEG+V4G::L]);//

  nodes[nLlowerLegFront].set(&nodePos[nLlowerLegFront],SERVOMOTOR,
			     Axis::X,&motors[V4G::LOWERLEG+V4G::L],-1*-1,0,&nodes[nLkneeUpperFront],&parts[pLlowerLegFront]);
  nodes[nLlowerLegFront].setMotId(V4G::LOWERLEG+V4G::L);
  nodes[nLlowerLegBack].set(&nodePos[nLlowerLegBack],FREEHINGE,
  			       Axis::X,NULL,-1*-1,0,&nodes[nLkneeUpperFront],&parts[pLlowerLegBack]);
  nodes[nLlowerLegBack].setFakeServo(&motors[V4G::LOWERLEG+V4G::L]);//
  nodes[nLfootTriangleFront].set(&nodePos[nLfootTriangleFront],FREEHINGE,
				 Axis::X,NULL,-1*1,0,&nodes[nLlowerLegFront],&parts[pLfootTriangle]);
  nodes[nLfootTriangleFront].setFakeServo(&motors[V4G::LOWERLEG+V4G::L]);//
  nodes[nLfootTriangleBack].set(&nodePos[nLfootTriangleFront],FREEHINGE,
				Axis::X,NULL,-1*1,0,&nodes[nLlowerLegBack],&parts[pLfootTriangle]);
  nodes[nLfootTriangleBack].setFakeServo(&motors[V4G::LOWERLEG+V4G::L]);//
  nodes[nLfootSquare].set(&nodePos[nLfootSquare],SERVOMOTOR,
			  Axis::X,&motors[V4G::FOOTSQUARE+V4G::L],-1*1,0,&nodes[nLfootTriangleFront],&parts[pLfootSquare]);
  nodes[nLfootSquare].setMotId(V4G::FOOTSQUARE+V4G::L);
  nodes[nLfoot].set(&nodePos[nLfoot],SERVOMOTOR,
		    Axis::Y,&motors[V4G::FOOT+V4G::L],-1*-1,0,&nodes[nLfootSquare],&parts[pLfoot]);
  nodes[nLfoot].setMotId(V4G::FOOT+V4G::L);
  //----------RIGHT nodes--------------
    
  nodes[nRpreShoulder].set(&nodePos[nRpreShoulder],REFERENCEROTATION,
			   Axis::Y,NULL,-1*1,-M_PI/180*10,&nodes[nBody],NULL);
  nodes[nRshoulder].set(&nodePos[nRshoulder],SERVOMOTOR,
			Axis::X,&motors[V4G::SHOULDER+V4G::R],-1*-1,M_PI/180*45,&nodes[nRpreShoulder],&parts[pRshoulder]); //2010.10.21 50->45
  nodes[nRshoulder].setMotId(V4G::SHOULDER+V4G::R);
  nodes[nRarm].set(&nodePos[nRarm],SERVOMOTOR,
		   Axis::Y,&motors[V4G::ARM+V4G::R],-1*-1,-M_PI/2,&nodes[nRshoulder],&parts[pRarm]);
  nodes[nRarm].setMotId(V4G::ARM+V4G::R);
  nodes[nRhand].set(&nodePos[nRhand],SERVOMOTOR,
		    Axis::Y,&motors[V4G::HAND+V4G::R],-1*-1,M_PI/180*30,&nodes[nRarm],&parts[pRhand]); //2010.10.21 45->30
  nodes[nRhand].setMotId(V4G::HAND+V4G::R);
  nodes[nRhip].set(&nodePos[nRhip],SERVOMOTOR,
		   Axis::Z,&motors[V4G::HIP+V4G::R],-1*1,0,&nodes[nBody],&parts[pRhip]);
  nodes[nRhip].setMotId(V4G::HIP+V4G::R);
  nodes[nRhipSquare].set(&nodePos[nRhipSquare],SERVOMOTOR,
			 Axis::Y,&motors[V4G::HIPSQUARE+V4G::R],-1*1,0,&nodes[nRhip],&parts[pRhipSquare]);
  nodes[nRhipSquare].setMotId(V4G::HIPSQUARE+V4G::R);
  nodes[nRhipTriangle].set(&nodePos[nRhipTriangle],SERVOMOTOR,
			   Axis::X,&motors[V4G::HIPTRIANGLE+V4G::R],-1*1,0,&nodes[nRhipSquare],&parts[pRhipTriangle]);
  nodes[nRhipTriangle].setMotId(V4G::HIPTRIANGLE+V4G::R);
  nodes[nRupperLegFront].set(&nodePos[nRupperLegFront],FREEHINGE,
			     Axis::X,NULL,-1*1,0,&nodes[nRhipTriangle],&parts[pRupperLegFront]);
  nodes[nRupperLegFront].setFakeServo(&motors[V4G::UPPERLEG+V4G::R]);//
  nodes[nRupperLegBack].set(&nodePos[nRupperLegBack],FREEHINGE,
  			       Axis::X,NULL,-1*1,0,&nodes[nRhipTriangle],&parts[pRupperLegBack]);
  nodes[nRupperLegBack].setFakeServo(&motors[V4G::UPPERLEG+V4G::R]);//
  nodes[nRkneeUpperFront].set(&nodePos[nRkneeUpperFront],SERVOMOTOR,
			      Axis::X,&motors[V4G::UPPERLEG+V4G::R],-1*-1,0,&nodes[nRupperLegFront],&parts[pRknee]);
  nodes[nRkneeUpperFront].setMotId(V4G::UPPERLEG+V4G::R);
  nodes[nRkneeUpperBack].set(&nodePos[nRkneeUpperBack],FREEHINGE,
  		      Axis::X,NULL,-1*-1,0,&nodes[nRupperLegBack],&parts[pRknee]);
  nodes[nRkneeUpperBack].setFakeServo(&motors[V4G::UPPERLEG+V4G::R]);//
  nodes[nRlowerLegFront].set(&nodePos[nRlowerLegFront],SERVOMOTOR,
			     Axis::X,&motors[V4G::LOWERLEG+V4G::R],-1*1,0,&nodes[nRkneeUpperFront],&parts[pRlowerLegFront]);
  nodes[nRlowerLegFront].setMotId(V4G::LOWERLEG+V4G::R);
  nodes[nRlowerLegBack].set(&nodePos[nRlowerLegBack],FREEHINGE,
  			       Axis::X,NULL,-1*1,0,&nodes[nRkneeUpperFront],&parts[pRlowerLegBack]);
  nodes[nRlowerLegBack].setFakeServo(&motors[V4G::LOWERLEG+V4G::R]);//
  nodes[nRfootTriangleFront].set(&nodePos[nRfootTriangleFront],FREEHINGE,
				 Axis::X,NULL,-1*-1,0,&nodes[nRlowerLegFront],&parts[pRfootTriangle]);
  nodes[nRfootTriangleFront].setFakeServo(&motors[V4G::LOWERLEG+V4G::R]);//
  nodes[nRfootTriangleBack].set(&nodePos[nRfootTriangleFront],FREEHINGE,
				Axis::X,NULL,-1*-1,0,&nodes[nRlowerLegBack],&parts[pRfootTriangle]);
  nodes[nRfootTriangleBack].setFakeServo(&motors[V4G::LOWERLEG+V4G::R]);//
  nodes[nRfootSquare].set(&nodePos[nRfootSquare],SERVOMOTOR,
			  Axis::X,&motors[V4G::FOOTSQUARE+V4G::R],-1*-1,0,&nodes[nRfootTriangleFront],&parts[pRfootSquare]);
  nodes[nRfootSquare].setMotId(V4G::FOOTSQUARE+V4G::R);
  nodes[nRfoot].set(&nodePos[nRfoot],SERVOMOTOR,
		    Axis::Y,&motors[V4G::FOOT+V4G::R],-1*-1,0,&nodes[nRfootSquare],&parts[pRfoot]);
  nodes[nRfoot].setMotId(V4G::FOOT+V4G::R);
    
  for(int i=0;i<NODESNUM;i++){ 
      addSkeletonNode(&nodes[i]);
  }

  disableCollDet(&parts[pNeck],&parts[pBody]);
  disableCollDet(&parts[pNeck],&parts[pHead]);

  disableCollDet(&parts[pLshoulder],&parts[pBody]);
      
  disableCollDet(&parts[pLarm],&parts[pLshoulder]);
  disableCollDet(&parts[pLarm],&parts[pBody]);
  
  disableCollDet(&parts[pLshoulder],&parts[pLhand]);
  
  disableCollDet(&parts[pLhand],&parts[pLarm]);
  disableCollDet(&parts[pLhand],&parts[pLshoulder]);

  disableCollDet(&parts[pLhip],&parts[pBody]);
  disableCollDet(&parts[pLhipSquare],&parts[pLhip]);
  disableCollDet(&parts[pLhipTriangle],&parts[pLhipSquare]);
  disableCollDet(&parts[pLhipTriangle],&parts[pLhip]);

  disableCollDet(&parts[pLupperLegFront],&parts[pLhipTriangle]);
  disableCollDet(&parts[pLupperLegFront],&parts[pLhipSquare]);
  disableCollDet(&parts[pLupperLegFront],&parts[pLhip]);
  
  disableCollDet(&parts[pLupperLegBack],&parts[pLupperLegFront]);
  disableCollDet(&parts[pLupperLegBack],&parts[pLhipTriangle]);
  disableCollDet(&parts[pLupperLegBack],&parts[pLhipSquare]);
  disableCollDet(&parts[pLupperLegBack],&parts[pLhip]);

  disableCollDet(&parts[pLknee],&parts[pLupperLegFront]);
  disableCollDet(&parts[pLknee],&parts[pLupperLegBack]);
  disableCollDet(&parts[pLknee],&parts[pLupperLegFront]);

  disableCollDet(&parts[pLlowerLegFront],&parts[pLknee]);
  disableCollDet(&parts[pLlowerLegFront],&parts[pLupperLegFront]);
  disableCollDet(&parts[pLlowerLegFront],&parts[pLupperLegBack]);
  disableCollDet(&parts[pLlowerLegBack],&parts[pLlowerLegFront]);
  disableCollDet(&parts[pLlowerLegBack],&parts[pLknee]);
  disableCollDet(&parts[pLlowerLegBack],&parts[pLupperLegBack]);
  disableCollDet(&parts[pLlowerLegBack],&parts[pLupperLegFront]);
  
  
  disableCollDet(&parts[pLfootTriangle],&parts[pLlowerLegFront]);
  disableCollDet(&parts[pLfootTriangle],&parts[pLlowerLegBack]);
  
  disableCollDet(&parts[pLfootSquare],&parts[pLfootTriangle]);
  disableCollDet(&parts[pLfootSquare],&parts[pLlowerLegFront]);
  disableCollDet(&parts[pLfootSquare],&parts[pLlowerLegBack]);
  
  disableCollDet(&parts[pLfoot],&parts[pLfootSquare]);
  disableCollDet(&parts[pLfoot],&parts[pLfootTriangle]);
  disableCollDet(&parts[pLfoot],&parts[pLlowerLegFront]);
  disableCollDet(&parts[pLfoot],&parts[pLlowerLegBack]);

  disableCollDet(&parts[pRshoulder],&parts[pBody]);
  
  disableCollDet(&parts[pRarm],&parts[pRshoulder]);
  disableCollDet(&parts[pRarm],&parts[pBody]);
  
  disableCollDet(&parts[pRhand],&parts[pRarm]);
  disableCollDet(&parts[pRhand],&parts[pLshoulder]);

  disableCollDet(&parts[pRhip],&parts[pBody]);
  disableCollDet(&parts[pRhipSquare],&parts[pRhip]);
  disableCollDet(&parts[pRhipTriangle],&parts[pRhipSquare]);
  disableCollDet(&parts[pRhipTriangle],&parts[pRhip]);
  
  disableCollDet(&parts[pRupperLegFront],&parts[pRhipTriangle]);
  disableCollDet(&parts[pRupperLegFront],&parts[pRhipSquare]);
  disableCollDet(&parts[pRupperLegFront],&parts[pRhip]);

  disableCollDet(&parts[pRupperLegBack],&parts[pRupperLegFront]);
  disableCollDet(&parts[pRupperLegBack],&parts[pRhipTriangle]);
  disableCollDet(&parts[pRupperLegBack],&parts[pRhipSquare]);
  disableCollDet(&parts[pRupperLegBack],&parts[pRhip]);
  
  disableCollDet(&parts[pRknee],&parts[pRupperLegFront]);
  disableCollDet(&parts[pRknee],&parts[pRupperLegBack]);
  disableCollDet(&parts[pRknee],&parts[pRupperLegFront]);

  disableCollDet(&parts[pRlowerLegFront],&parts[pRknee]);
  disableCollDet(&parts[pRlowerLegFront],&parts[pRupperLegFront]);
  disableCollDet(&parts[pRlowerLegFront],&parts[pRupperLegBack]);
  disableCollDet(&parts[pRlowerLegBack],&parts[pRlowerLegFront]);
  disableCollDet(&parts[pRlowerLegBack],&parts[pRknee]);
  disableCollDet(&parts[pRlowerLegBack],&parts[pRupperLegBack]);
  disableCollDet(&parts[pRlowerLegBack],&parts[pRupperLegFront]);
  
  disableCollDet(&parts[pRfootTriangle],&parts[pRlowerLegFront]);
  disableCollDet(&parts[pRfootTriangle],&parts[pRlowerLegBack]);
  
  disableCollDet(&parts[pRfootSquare],&parts[pRfootTriangle]);
  disableCollDet(&parts[pRfootSquare],&parts[pRlowerLegFront]);
  disableCollDet(&parts[pRfootSquare],&parts[pRlowerLegBack]);
  
  disableCollDet(&parts[pRfoot],&parts[pRfootSquare]);
  disableCollDet(&parts[pRfoot],&parts[pRfootTriangle]);
  disableCollDet(&parts[pRfoot],&parts[pRlowerLegFront]);
  disableCollDet(&parts[pRfoot],&parts[pRlowerLegBack]);
  pivot=&parts[pBody];  
#ifdef BABYVERSION  
  bigHead();
#endif
}
  
V4Gstructure::~V4Gstructure(){
  if (!externMotors)
    delete [] motors;
  if (bHead!=NULL) delete bHead;
}


void V4Gstructure::setTexture(GlTexture* tex){
  for (int i=0;i<BOXESNUM;i++){
    apps[i].texture=tex;
  }
}

void V4Gstructure::moveArm(){
  nodePos[nLpreShoulder].y()-=0.03;
  nodePos[nRpreShoulder].y()-=0.03;
}

BigHead::BigHead(Robot* r,Part* parts):
  size(0.116/2,0.116/2,0.127-0.116),eyeApp(0,0,0){
  Element* e=(Element*)parts[V4Gstructure::pHead].elements[0];
  e->shape=CAPPEDCYLINDERSHAPE;
  e->size=&size;
  e->rotoTransl[7]=0;
  e->rotoTransl[11]=0.02;
  r->disableCollDet(&parts[V4Gstructure::pHead],&parts[V4Gstructure::pBody]);
  double es=0.01;
  eyeSize.set(es,es,es);
  reyePos.set(0.03,e->size->x()*0.8,0.02);
  leyePos.setNegX(reyePos);
  reye.set(&reyePos,&eyeSize,0,&eyeApp,ENCUMBRANCEINFO);
  reye.shape=SPHERESHAPE;
  leye.set(&leyePos,&eyeSize,0,&eyeApp,ENCUMBRANCEINFO);
  leye.shape=SPHERESHAPE;
  parts[V4Gstructure::pHead].addElement(&reye);
  parts[V4Gstructure::pHead].addElement(&leye);    
}

void V4Gstructure::bigHead(){

  /*Element* e=(Element*)parts[pHead].elements[0];
  e->shape=CAPPEDCYLINDERSHAPE;
  e->size=new Vector3d(0.116/2,0.116/2,0.127-0.116);
  e->rotoTransl[7]=0;
  e->rotoTransl[11]=0.02;
  disableCollDet(&parts[pHead],&parts[pBody]);
  double es=0.01;
  Vector3d* eyeSize=new Vector3d(es,es,es);
  Vector3d* reyePos=new Vector3d(0.03,e->size->x*0.8,0.02);
  Vector3d* leyePos=new Vector3d();
  leyePos->setNegX(*reyePos);
  Appearance* eyeApp=new Appearance(0,0,0);
  Element* reye=new Element(reyePos,eyeSize,0,eyeApp,ENCUMBRANCEINFO,SPHERESHAPE);
  Element* leye=new Element(leyePos,eyeSize,0,eyeApp,ENCUMBRANCEINFO,SPHERESHAPE);
  parts[pHead].addElement(reye);
  parts[pHead].addElement(leye);*/
  bHead=new BigHead(this,parts);
}


void V4Gstructure::addRoundBack(){

  Vector3d* size=ad.s=new Vector3d(boxSizes[eBody]);
  size->z()-=0.1;
  size->x()/=1.9;
  Vector3d* pos=ad.s=new Vector3d(0,-1*size->x(),0);
  Element* r=ad.s=new Element(pos,size,0,&apps[eBody],ENCUMBRANCEINFO,CAPPEDCYLINDERSHAPE);
  parts[pBody].addElement(r);

  //--------TWO CYLINDERS-------
  /*double rad=0.03;
  Vector3d* size=new Vector3d(rad,rad,boxSizes[eBody].z-2*rad);
  Vector3d* pos=new Vector3d(boxSizes[eBody].x/2*1.5,-boxSizes[eBody].y/2,0.0);
  Vector3d* pos2=new Vector3d;pos2->setNegX(*pos);
  Element* r=new Element(pos,size,0,&apps[eBody],ENCUMBRANCEINFO,CAPPEDCYLINDERSHAPE);
  parts[pBody].addElement(r);
  r=new Element(pos2,size,0,&apps[eBody],ENCUMBRANCEINFO,CAPPEDCYLINDERSHAPE);
  parts[pBody].addElement(r);*/
}


void V4Gstructure::secondaryColor(){
  apps[eBody].setColor(0.4f,0.54f,0.27f);
  apps[eRupperLegFront].setColor(0.67f,0.8f,0.6f);
  apps[eLupperLegBack].setColor(0.4f,0.54f,0.27f); 
  apps[eRupperLegBack].setColor(0.4f,0.54f,0.27f);
  apps[eLknee].setColor(0.22f,0.27f,0.19f);
  apps[eRknee].setColor(0.22f,0.27f,0.19f);
  apps[eLlowerLegFront].setColor(0.67f,0.8f,0.6f);
  apps[eRlowerLegFront].setColor(0.67f,0.8f,0.6f);
  apps[eLlowerLegBack].setColor(0.4f,0.54f,0.27f);
  apps[eRlowerLegBack].setColor(0.4f,0.54f,0.27f);
  apps[eLfootVertical].setColor(1.0f,0.80f,0.80f);
  apps[eRfootVertical].setColor(1.0f,0.80f,0.80f);
}

void V4Gstructure::setSensorNames(bool realSensors){
  useRealSensorNames=realSensors;
}



  /*V4GmotorSymmetries::V4GmotorSymmetries(){
  addSymmetry(SHOULDER+L,SHOULDER+R,true);
  addSymmetry(ARM+L,ARM+R,true);
  addSymmetry(HAND+L,HAND+R,true);
  addSymmetry(HIP+L,HIP+R,true);
  addSymmetry(HIPSQUARE+L,HIPSQUARE+R,true);
  addSymmetry(HIPTRIANGLE+L,HIPTRIANGLE+R,true);
  addSymmetry(UPPERLEG+L,UPPERLEG+R,true);
  addSymmetry(LOWERLEG+L,LOWERLEG+R,true);
  addSymmetry(FOOTSQUARE+L,FOOTSQUARE+R,true);
  addSymmetry(FOOT+L,FOOT+R,true);
  }*/
}//helium
