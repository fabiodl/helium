/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \author  Fransiska Basoeki
 * \date    2012.07.01
 * \version 0.2
 * Release_flags ge
 */

#include <helium/robot/frame.h>
#include <helium/vector/operations.h>
#include <cmath>

#include <ostream>
#include <iterator>
#include <sstream>
//#include <helium/util/mathUtil.h>
#include <helium/util/ioUtil.h>

using helium::Posture;
using helium::Time;
using helium::Motion;
using helium::NDRef;
using std::cerr;
using std::cout;
using std::endl;
using std::pair;
using std::set;

#ifdef __APPLE__
using std::isnan;
#endif


Posture::Posture(const Posture& p):Array<double>(p),name(p.name){
  //std::cout<<"new posture c @"<<this<<std::endl;
}

Posture::Posture(const Posture& p,std::string n):Array<double>(p),name(n) {
  //std::cout<<"new posture c n @"<<this<<std::endl;
}

Posture::Posture(helium::Array<helium::FrameValue> frameValue):
  Array<double>(frameValue.size())
{
  for(size_t i=0;i<this->size();i++) {    
    if(frameValue[i].first == helium::VALUEFRAME)
      data[i] = frameValue[i].second;
    else
      data[i] = NAN;
  }
}

void Posture::getFrameValue(helium::Array<helium::FrameValue> &frameValue) const{
  for(size_t i=0;i<this->size();i++)
    frameValue[i] = std::make_pair(isnan(data[i])?helium::INTERPOLATEFRAME:helium::VALUEFRAME,data[i]);
}

/*
Posture& Posture::operator >> (std::stringstream &s) {
  for(size_t i=0;i<this->size();i++) {
    std::string str;
    s >> str;
    convert(data[i],str," invalid motor angle value");
  }
  return *this;
}
*/


/*
Posture& Posture::getDeg() {
  Posture *p = new Posture(this->size());
  for(size_t i=0;i<this->size();i++) {
    p->data[i] = getRadtoDeg(data[i]);
  } 
  return *p;
}
*/

/*
void Posture::setRad() {
  for(size_t i=0;i<this->size();i++) {
    degToRad(data[i]);
  } 
}
*/

/*
bool Posture::isAllEnable() {
  for(size_t i=0;i<this->size();i++) {
    if(isnan(data[i]))
      return false;
  }
  return true;
}
*/

void Motion::setJointSize(size_t pn){
  n=pn;
  NANPosture.resize(n);
  motion.find(0)->second.getData().resize(n);
  helium::setAll(motion.find(0)->second.getData(),0);
  //insertZeroFrame();
  //cout << "setjointsize " << motion.size() << endl;
}


//empty motion
Motion::Motion(size_t pn):
  NANPosture(pn),
  n(pn)
  //mp(pmp)
  //iters(1),startTime(0),playSpeed(1)  
{
  //setJointSize(n);
  n=pn;
  NANPosture.resize(n);
  helium::setAll(NANPosture,NAN);
  insertZeroFrame();
  //cout << "motion created"<< endl;
}

//20140714 revoked //20131230 deleted copy constructor, should be able to do it automatically?
//replacing motion
Motion::Motion(const Motion &m):
  NANPosture(m.getJointSize()),
  n(m.getJointSize())
  //n(m.mp.getJointSize()),
  //iters(m.mp.iters),startTime(m.mp.startTime),playSpeed(m.playSpeed)
{
  helium::setAll(NANPosture,NAN);
  replace(m);
}


void Motion::setRobotName(const std::string &r) {
  robot = r;
}

int Motion::getJointSize() const { return n; }
std::string Motion::getRobotName() const { return robot; }


Motion& Motion::operator=(const Motion& m){
  replace(m);
  return *this;
}

void Motion::modifyFrame(Time t,int i, double d) {
  (*motion.find(t)->second)[i] = d;
}

//getPosture avoide getting a false time
void Motion::getPosture(Time t, Posture &p) const {
  std::map<Time,NDRef<Posture> >::const_iterator it=motion.find(t);
  if (it==motion.end()){
    p = NANPosture;
  }else{
    p=*it->second;
  }
}
const Posture& Motion::getPosture(Time t) const {
  std::map<Time,NDRef<Posture> >::const_iterator it=motion.find(t);
  if (it==motion.end()){
    return NANPosture;
  }else{
    return *it->second;
  }
}

/// adjust time t by dt and insert a frame
void Motion::insertShiftFrame(Time t, Time dt, const Posture &p) {
  if(t != getEndTime())
    adjustTime(t, dt);
  insertFrame(t+dt, p);
}

void Motion::insertFrame(Time t,const NDRef<Posture> &p) {
  if(!isFrame(t)) {
    motion.insert(pair<Time,NDRef<Posture> >(t,p)); //not overwriting old value
  }
  else {
    (*motion.find(t)->second) = p.getData();
  }
}

void Motion::insertFrame(Time t,const Posture &p) {
  NDRef<Posture> np(p);
  insertFrame(t,np);
}

void Motion::insertFrame(Time t,const Posture &p,std::string name) {
  insertFrame(t,p);
  rename(t,name);
}

void Motion::insertZeroFrame() {
  helium::Posture zeroPost(getJointSize());
  helium::setAll(zeroPost,0);
  insertFrame(0,zeroPost);
}

void Motion::deleteFrame(Time t) {
  motion.erase(t);
  if(t==0) 
    insertZeroFrame();
}

void Motion::clear() {
  motion.clear();
  insertZeroFrame();
}

void Motion::linkFrame(Time from, Time to) {
  NDRef<Posture> a = motion.find(from)->second;
  motion.erase(to); //reinsert frame with link
  insertFrame(to,a);
}

void Motion::unlinkFrame(Time t) {
  Posture p = *motion.find(t)->second; 
  motion.erase(t);//deleteFrame(t);
  insertFrame(t,p);
}

bool Motion::isLinked(helium::Time t) const {
  std::set<helium::Time> link;  
  getLink(link,t);
  return link.size()>1;
}

void Motion::getLink(std::set<helium::Time> &link,helium::Time t) const{     
  link.clear();
  std::map<Time,NDRef<Posture> >::const_iterator ite=motion.find(t);
  if (ite==motion.end()) return;
  const Posture* me=&ite->second;
  for (std::map<Time,NDRef<Posture> >::const_iterator it=motion.begin();
       it!=motion.end();
       it++){
    if (&it->second==me) 
      link.insert(it->first);
  }
}

bool Motion::isFrame(Time t) const {
  return motion.find(t) != motion.end();
}


Time Motion::getEndTime() const {
  std::map<Time,NDRef<Posture> >::const_iterator it=motion.end();
  it--;
  return it->first;
}

bool Motion::isEmpty() const {
  return motion.size()==0;

}

Time Motion::getNextTime(Time t, int id) const { ///< next time with a joint value not nan
  Time next = 0;
  //Time nonnan = 0;
  for(std::map<Time,NDRef<Posture> >::const_iterator it=motion.begin();it!=motion.end();it++) {
    next = (*it).first;   //next is *it
    //if(!isnan((*motion.find(next)->second)[id]))
    //nonnan = next; 


    if(next > t) {        //if it's already bigger than t, try to break
      if(id<0 || (!isnan((*motion.find(next)->second)[id]))) {         
      //if(id<0 || (isFrame(next) && !isnan((*motion.find(next)->second)[id]))) {         
	//if id is not specified, return rightaway
	break; ///<if id is specified, must get the next non nan frame
      }
    }
  }
  //if(isnan((*motion.find(next)->second)[id])) //if the next frame found is nan, get the previous
    //next = nonnan;

  return next;
}

Time Motion::getPrevTime(Time t, int id) const { ///< previous time with a joint value not nan
  Time prev = 0;
  //Time nonnan = 0;
  for(std::map<Time,NDRef<Posture> >::const_reverse_iterator it=motion.rbegin();it!=motion.rend();it++) {
    //start from end of motion
    prev = (*it).first;  //prev is *it
    //if(!isnan((*motion.find(prev)->second)[id]))
    //nonnan = prev; 


    if(prev < t)         //if prev is smaller than t, try to break 
      //if(id<0 || (isFrame(prev) && !isnan((*motion.find(prev)->second)[id])))
      if(id<0 || (!isnan((*motion.find(prev)->second)[id])))
	break;
  }

  //if(isnan((*motion.find(prev)->second)[id])) 
  //prev = nonnan;

  if(prev > t)
    prev = 0;

  return prev;
}

/*
frame notEnd
0      0      0 
0      1      0
1      0      0
1      1      1
*/

/** \brief adjust time at time <B>t</B> with <B>dt</B>
 */
void Motion::adjustTime(Time t, Time dt) {
  ///check feasibility of adjust time
  if(motion.size()<2) 
    throw helium::exc::InvalidDataValue("Cannot adjust time of motion with 1 frame");
  if(dt == 0)
    throw helium::exc::InvalidDataValue("Cannot adjust time of 0");
  ///gets prev and next frame off the given time <B>t</B>, prev is self if t is frame

  std::map<Time,NDRef<Posture> >::iterator it = motion.end();
  it--;
  if(dt>0) { //+delta -> go from behing
    Time nt = getNextTime(t);
    Time ct = it->first;

    while(1) {
      linkFrame(ct,ct+dt);
      motion.erase(ct);//deleteFrame(ct);
      if(ct == nt) break;
      ct = getPrevTime(ct);
    }
  }
  else { //-delta -> go from front
    Time nt = (*it).first;
    Time ct = getEndTime()==t?getPrevTime(t):t;

    if(t+dt <= getPrevTime(t)) {
      throw helium::exc::InvalidDataValue("Overlapping existing frame time");//return false;
    }

    while(ct < nt) {
      ct = getNextTime(ct);
      linkFrame(ct,ct+dt);
      motion.erase(ct);//deleteFrame(ct);
    }
  }
}

void Motion::replace(const Motion &m) {
  motion.clear();

  for (std::map<Time,NDRef<Posture> >::const_iterator it=m.motion.begin(); it!=m.motion.end(); it++){
    const Posture* me=it->second.getPointer(); //find this posture's link
    helium::Time link = -1;
    for (std::map<Time,NDRef<Posture> >::const_iterator mit=m.motion.begin(); mit!=it; mit++){
      if (mit->second.getPointer()==me) {
	link = mit->first;
	break;
      }
    }
    
    if (link < 0)  {
      insertFrame(it->first,it->second.getData());   //no motion found
    }
    else {
      std::map<Time,NDRef<Posture> >::iterator ite=motion.find(link);
      insertFrame(it->first,ite->second); // found link, insert same NDREF
    }
  }
}

void Motion::rename(Time t,std::string s) {
  motion.find(t)->second.getData().name = s;
}

namespace helium {
  std::ostream& operator<<(std::ostream& o,const helium::Motion& m) {
    o << "--" << endl;
    for (Motion::MotionMap::const_iterator it=m.motion.begin(); it!=m.motion.end(); ++it) {
      helium::Posture p = it->second.getData();
      o << it->first <<":";
      o << helium::en::io << p;
    }
    return o;    
  }
};
