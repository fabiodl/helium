#ifndef HE_EVART
#define HE_EVART

#include <evart/EVaRT2.h>
#include <helium/util/ioUtil.h>


namespace helium{

inline std::ostream& operator<<(std::ostream& o,const tSegmentData& b){
 write(o,b,7);
  return o;  
}

inline std::ostream& operator<<(std::ostream& o,const tMarkerData& b){
  write(o,b,3);
  return o;  
}

inline std::ostream& operator<<(std::ostream& o,const tForceData& b){
  write(o,b,7);
  return o;  
}
}
std::ostream& operator<<(std::ostream& o,const struct sBodyData& b){
  o<<b.szName<<" ";
  helium::sizeWriten(o,b.Markers,b.nMarkers);
  o<<" "<<b.fAvgMarkerResidual<<" ";
  helium::sizeWriten(o,b.Segments,b.nSegments);
  o<<" ";
  helium::sizeWriten(o,b.Dofs,b.nDofs);
  o<<" "<<b.fAvgDofResidual<<" ";
  return o<<b.nIterations;
}

std::ostream& operator<<(std::ostream& o,const struct sFrameOfData& f){
  o<<f.iFrame<<" ";
  helium::sizeWriten(o,f.BodyData,f.nBodies);
  o<<" ";
  helium::sizeWriten(o,f.UnidentifiedMarkers,f.nUnidentifiedMarkers);
  return o;
}

#endif
