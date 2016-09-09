#include <helium/vstone/neonySensorMapping.h>
#include <helium/vstone/V4Gstructure.h>

namespace helium{

  NeonySensorMapping::NeonySensorMapping(): SensorListMaxComputation(Rsensor::REALSENSORNUM,Ssensor::SIMULATEDSENSORNUM){
  add(Rsensor::ForeheadR,Ssensor::HeadF);
  add(Rsensor::ForeheadL,Ssensor::HeadF);
  add(Rsensor::ChinR,Ssensor::HeadF);
  add(Rsensor::ChinL,Ssensor::HeadF);
  add(Rsensor::rTemple,Ssensor::HeadR);
  add(Rsensor::rJaw,Ssensor::HeadR);
  add(Rsensor::lJaw,Ssensor::HeadL);
  add(Rsensor::lTemple,Ssensor::HeadL);
  add(Rsensor::HeadtopL,Ssensor::HeadT);
  add(Rsensor::HeadtopC,Ssensor::HeadT);
  add(Rsensor::Headback,Ssensor::HeadB);
  add(Rsensor::HeadtopR,Ssensor::HeadT);
  add(Rsensor::lEar,Ssensor::HeadL);
  add(Rsensor::Nape,Ssensor::HeadB);
  add(Rsensor::rEar,Ssensor::HeadR);

    add(Rsensor::ChestL,Ssensor::BodyF);
    add(Rsensor::ChestR,Ssensor::BodyF);
    add(Rsensor::AbdomenL,Ssensor::BodyF);
    add(Rsensor::AbdomenR,Ssensor::BodyF);
    add(Rsensor::AbdomenC,Ssensor::BodyF);
    add(Rsensor::lRib,Ssensor::BodyF);
    add(Rsensor::rRib,Ssensor::BodyF);
    add(Rsensor::BackupperL,Ssensor::BodyB);
    add(Rsensor::BacklowerL,Ssensor::BodyB);
    add(Rsensor::BackupperR,Ssensor::BodyB);
    add(Rsensor::BacklowerR,Ssensor::BodyB);
    add(Rsensor::lSideF,Ssensor::BodyL);
    add(Rsensor::lSideB,Ssensor::BodyL);
    add(Rsensor::lSideC,Ssensor::BodyL);
    add(Rsensor::rSideF,Ssensor::BodyR);
    add(Rsensor::rSideB,Ssensor::BodyR);
    add(Rsensor::rSideC,Ssensor::BodyR);
 
    add(Rsensor::lShoulderT,Ssensor::lShoulderT);
    add(Rsensor::lShoulderL,Ssensor::lShoulderL);
    add(Rsensor::lArmupperF,Ssensor::lArmupperF);
    add(Rsensor::lArmupperB,Ssensor::lArmupperB);
    add(Rsensor::lElbow,Ssensor::lArmupperL);
    add(Rsensor::lArmlowerF,Ssensor::lArmlowerF);
    add(Rsensor::lArmlowerB,Ssensor::lArmlowerB);
    add(Rsensor::lHandpalmT,Ssensor::lHandFR); add(Rsensor::lHandpalmT,Ssensor::lArmlowerR); add(Rsensor::lHandpalmT,Ssensor::lHandBR);
    add(Rsensor::lHandpalmB,Ssensor::lHandGR);
    add(Rsensor::lHandtopT,Ssensor::lHandFL); add(Rsensor::lHandtopT,Ssensor::lHandBL); add(Rsensor::lHandtopT,Ssensor::lArmlowerL); 
    add(Rsensor::lHandtopB,Ssensor::lHandGL);

    add(Rsensor::lHipB,Ssensor::lHipBB);
    add(Rsensor::lHipF,Ssensor::lHipFF);
    add(Rsensor::lKnee,Ssensor::lKneeF);
    add(Rsensor::lLegupperL,Ssensor::lLegupperFL); //add(lLegupperL,lLegupperBL);
    add(Rsensor::lLegupperF,Ssensor::lLegupperFF);
    add(Rsensor::lLegupperR,Ssensor::lLegupperFR); //add(lLegupperR,lLegupperBR);
    add(Rsensor::lAnkleF,Ssensor::lAnkleBF);
    add(Rsensor::lAnkleB,Ssensor::lAnkleBB);
    add(Rsensor::lLeglowerR,Ssensor::lLeglowerFR); //add(lLeglowerR,lLeglowerBR);
    add(Rsensor::lLeglowerF,Ssensor::lLeglowerFF);
    add(Rsensor::lLeglowerL,Ssensor::lLeglowerFL); //add(lLeglowerL,lLeglowerBL);
    add(Rsensor::lFootpalmFL,Ssensor::lFootG);
    add(Rsensor::lFootpalmBL,Ssensor::lFootG);
    add(Rsensor::lFootpalmFR,Ssensor::lFootG);
    add(Rsensor::lFootpalmBR,Ssensor::lFootG);
    add(Rsensor::lFoottopL,Ssensor::lFootT);
    add(Rsensor::lFoottopR,Ssensor::lFootT);
    add(Rsensor::lFoottopB,Ssensor::lFootT);

    add(Rsensor::rShoulderT,Ssensor::rShoulderT);
    add(Rsensor::rShoulderR,Ssensor::rShoulderR);
    add(Rsensor::rArmupperF,Ssensor::rArmupperF);
    add(Rsensor::rArmupperB,Ssensor::rArmupperB);
    add(Rsensor::rElbow,Ssensor::rArmupperR);
    add(Rsensor::rArmlowerF,Ssensor::rArmlowerF);
    add(Rsensor::rArmlowerB,Ssensor::rArmlowerB);
    add(Rsensor::rHandpalmT,Ssensor::rHandFL); add(Rsensor::rHandpalmT,Ssensor::rArmlowerL); add(Rsensor::rHandpalmT,Ssensor::rHandBL);
    add(Rsensor::rHandpalmB,Ssensor::rHandGL);
    add(Rsensor::rHandtopT,Ssensor::rHandFR); add(Rsensor::rHandtopT,Ssensor::rHandBR); add(Rsensor::rHandtopT,Ssensor::rArmlowerR); 
    add(Rsensor::rHandtopB,Ssensor::rHandGR);


    add(Rsensor::rHipB,Ssensor::rHipBB);
    add(Rsensor::rHipF,Ssensor::rHipFF);
    add(Rsensor::rKnee,Ssensor::rKneeF);
    add(Rsensor::rLegupperL,Ssensor::rLegupperFL); //add(Rsensor::lLegupperL,Ssensor::lLegupperBL);
    add(Rsensor::rLegupperF,Ssensor::rLegupperFF);
    add(Rsensor::rLegupperR,Ssensor::rLegupperFR); //add(Rsensor::lLegupperR,Ssensor::lLegupperBR);
    add(Rsensor::rAnkleF,Ssensor::rAnkleBF);
    add(Rsensor::rLeglowerR,Ssensor::rLeglowerFR); //add(Rsensor::lLeglowerR,Ssensor::lLeglowerBR);
    add(Rsensor::rLeglowerF,Ssensor::rLeglowerFF);
    add(Rsensor::rLeglowerL,Ssensor::rLeglowerFL); //add(Rsensor::lLeglowerL,Ssensor::lLeglowerBL);
    add(Rsensor::rFootpalmFL,Ssensor::rFootG);
    add(Rsensor::rFootpalmBL,Ssensor::rFootG);
    add(Rsensor::rFootpalmFR,Ssensor::rFootG);
    add(Rsensor::rFootpalmBR,Ssensor::rFootG);
    add(Rsensor::rFoottopL,Ssensor::rFootT);
    add(Rsensor::rFoottopR,Ssensor::rFootT);
    add(Rsensor::rFoottopB,Ssensor::rFootT);
    add(Rsensor::rAnkleB,Ssensor::rAnkleBB);

#ifdef HANDADDITIONALSENSORS
    add(Rsensor::lLittlefinger,Ssensor::lHandBB);
    add(Rsensor::rLittlefinger,Ssensor::rHandBB);
#endif


  }
  
  
}
