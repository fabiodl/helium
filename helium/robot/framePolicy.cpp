/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags ge
 */

#include <helium/robot/framePolicy.h>
#include <helium/math/interpolation.h>
#include <helium/core/nan.h>
//#include <helium/util/ioUtil.h>

using namespace helium;



FramePolicy::FramePolicy(const Motion &pmotion, bool pcubic):
  Policy(),
  motion(pmotion),
  cubic(pcubic)
{
}

/*! check for existance of keyframe and return posture accordingly
 */
double FramePolicy::getPosture(int id, Time t){
  if (motion.isFrame(t) && !isnan(motion[t][id])) 
    return motion.getAngle(id,t); ///<return the motion frame value
  return getInterpolatedPosture(id,t);
}

/*! return posture which is an interpolation of previous keyframe and next keyframe
 */
double FramePolicy::getInterpolatedPosture(int id, Time t){ 
  Time prev = motion.getPrevTime(t,id);
  Time next = motion.getNextTime(t,id);
  return getPostureBetween(t,prev,next,id);
}


double FramePolicy::getPostureBetween(helium::Time t,helium::Time prev, helium::Time next, int id) {
  //std::cout << "getposturebetween " << id << " " << prev << " " << next << " "<< t;
  double val,a,b,c,d;

  if(prev == t && next == t) { //nowhere to interpolate from
    val = motion[prev][id]; //
  }
  else if(prev == next) { //nowhere to interpolate from
    val = motion[prev][id]; //
  }
  else if(isnan(motion[prev][id]) && !isnan(motion[next][id])) {//depending on one of the non nan frame
    //std::cout << "nonnan";
    val = motion[next][id];
  }
  else if(isnan(motion[next][id]) && !isnan(motion[prev][id])) {
    //std::cout << "nonnan";
    val = motion[prev][id];
  }
  else
  if(cubic) { //cubic interpolation between frames
    helium::cubicInterpolation(&a,&b,&c,&d,
			       prev,motion[prev][id],
			       next,motion[next][id]);
    val = helium::getCubicValue(a,b,c,d,t); //could return nan
  }
  else {
    //std::cout << "interpolate ";
    val = (t-prev) / (double)(next-prev) * (motion[next][id] - motion[prev][id]) + motion[prev][id];
  }

  if (isnan(val)){
    //std::cout << "was nan";
    val = 0;
  }


  //std::cout << std::endl;
  return val;
}

void FramePolicy::getPosture(double* p,Time t) {
  for(int i=0;i<motion.getJointSize();i++) {
    p[i] = getPosture(i,t);
  }
}

void FramePolicy::getPosture(Posture &p,Time t) {
  for(int i=0;i<motion.getJointSize();i++) {
    p[i] = getPosture(i,t);
  }
}

Time FramePolicy::getEndTime() {
  /*Time t = 0;
  if(!motion.isEmpty())
    return motion.getEndFrame();
  return t;*/
  return motion.getEndTime();
}


helium::Motion::MotionMap FramePolicy::getInterpolatedMotionMap() {
  helium::Motion mm(motion);
  for(helium::Motion::MotionMap::const_iterator it=mm.getMotion().begin(); it!=mm.getMotion().end(); it++){
    getPosture(it->second.getData(),it->first);
  }
  return mm.getMotion();
}

/*double FramePolicy::getPosture(int id, Time t, bool disabled) { //disabled = true means to assume current frame is disabled (to avoid returning motion value)
  //if it is a frame, simply return the value
  if(motion.isFrame(t) && !disabled) 
    if(!isnan(motion[t][id]))
      return motion[t][id];

  Time prev = motion.getPrevFrame(t,id);
  Time next = motion.getNextFrame(t,id);

  //prev or next is not a frame,
  //prev and next frame is the same, 
  //no value can be returned
  if(prev == next || !motion.isFrame(prev) || !motion.isFrame(next)) {
    if(motion.isFrame(prev) && !isnan(motion[prev][id]))
      return motion[prev][id];
    else if(motion.isFrame(next) && !isnan(motion[next][id]))
      return motion[next][id];
    else 
      return 0;
  }
  //value of prev is same as next, no need interpolation
  if(motion[prev][id] == motion[next][id])
    return motion[prev][id];

  return getPostureBetween(t,prev,next,id);
}
*/
