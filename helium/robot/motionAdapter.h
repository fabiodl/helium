/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_MOTIONADAPTER
#define HE_MOTIONADAPTER


#include <helium/core/nan.h>
#include <helium/robot/frame.h>
#include <helium/robot/motorFramePlayMode.h>

namespace helium{
  
  //the target must be set to the correct size
  inline void setMotionData(framePlay::Target& t,const Motion& m, const helium::MotionParameter & pmp=MotionParameter()){
    //std::cout << "setmotiondata" << std::endl;
    t.mp=pmp;    
    t.clear();
    const Motion::MotionMap& motion=m.getMotion();
    for(Motion::MotionMap::const_iterator it=motion.begin(); it!=motion.end(); ++it){
      //it->first
      t.ticks.push_back(it->first);
      for(int j=0;j<m.getJointSize();j++){
	const Posture& r=it->second.getData();
	if (!isnan(r[j])){
	  t.frames[j].push_back(std::make_pair((int)t.ticks.size()-1,r[j]));
	}
      }
      
    }//for it
  }//setMotionData


}//helium



#endif
