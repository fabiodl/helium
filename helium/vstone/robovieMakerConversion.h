#ifndef HE_ROBOVIEMAKERCONVERSIONS
#define HE_ROBOVIEMAKERCONVERSIONS

#include <helium/robot/frame.h>
#include <helium/robot/motorStructures.h>
#include <helium/vstone/robovieMakerFile.h>
#include <helium/vstone/vServoSign.h>

namespace helium{

  void importMotion(Motion& mot,RMPoseFile& src,RMCalibFile& cal,const IdMapper& idMap,LinearPosNormalizer<VServoSign>& normalizer);

   void importMotion(Motion& mot,RMPoseFile& src,RMCalibFile& cal,Memory& mem);

  void exportMotion(RM2File& f,Motion& mot,RMCalibFile& cal,const IdMapper& idMap,LinearPosNormalizer<VServoSign>& normalizer,bool loopy=false);
  
  void exportMotion(RM2File& f,Motion& mot,RMCalibFile& cal,Memory& mem,bool loopy=false);

}


#endif
