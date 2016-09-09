/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.1
 * Release_flags ge
 */

#include <helium/robot/motionParser.h>
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;
using std::stringstream;
using std::string;
using namespace helium;

MotionParser::MotionParser(const std::string &r):robot(r) {}

void MotionParser::openMotion(helium::Motion &motion, const char* filename)  {
  XmlLoader xml;
  Memory inmem;
  xml.loadDataAndModel(inmem,filename);
  dimport(motion,inmem);
  if(strcmp(robot.c_str(),motion.robot.c_str()) != 0)
    std::cerr << "warning: robot name is not the same, setting to " << robot<< " was:" << motion.robot<< std::endl;
  
}

void MotionParser::saveMotion(const helium::Motion &motion, const char* filename) {
  helium::Memory outmem;    
  helium::XmlLoader out;
  helium::dexport(outmem,const_cast<helium::Motion&>(motion));
  out.save(filename,outmem);

  //check robot
}

void LinkExport::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
  //cout << "linkexport" << endl;
  if(obj > 0) members.push_back(fscalarMapping<USEDATACELL>(obj,"link"));
}

void AngleExport::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
  //cout << "angleexport" << endl;
  if(obj.size()) members.push_back(fscalarMapping<USEDATACELL>(obj,"angle"));
}
void NameExport::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
  //cout << "nameexport" << endl;
  if(obj.size()) members.push_back(fscalarMapping<USEDATACELL>(obj,"name"));
}
void DefDescr<Motion>::import(Memory& mem,const IDPath &path){
  //cout << "defmot : import" << endl;
  Description<Motion>::import(mem,path); //calls getMapMembers, call onInitializationFinished
  for(size_t i=0;i<time.size();i++) {
    if(angle[i].size()) {
      if(name[i].size()) {
	obj.insertFrame(time[i],angle[i],name[i]);
      }
      else {
	obj.insertFrame(time[i],angle[i]);
      }
    }
    else
      obj.linkFrame(link[i],time[i]);
  }
}
void DefDescr<Motion>::exportRef(Memory& mem,const IDPath &path){
  //cout << "defmot : exportref" << endl;
  for (std::map<Time,NDRef<Posture> >::iterator it=obj.motion.begin();it!=obj.motion.end();it++){
    time.push_back(it->first);
    
    //check for link here
    helium::Time li = it->first;
    std::set<helium::Time> linked;
    obj.getLink(linked,it->first);
    
    if(linked.size()>1)
      li = *linked.begin();
    
    if(li == it->first) { //either not linked or the first link
      angle.push_back(it->second.getData());
      link.push_back(-1);
      name.push_back(it->second.getData().name);
    }
    else {//the 2nd and so forth in the link
      angle.push_back(Posture(0));
      link.push_back(li);
      name.push_back("");
    }
    
    
  }
  Description<Motion>::exportRef(mem,path);
}
void DefDescr<Motion>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members) {
  //cout << "defmot : getmap" << endl;
  members.push_back(scalarMapping(d(obj.robot),IDPath("motion.robot.type",".")));
  
  //export: time -> xml
  if(time.size()) {
    members.push_back(fvectMapping<USEDATACELL|USEDATACELLFORSIZE>(time,IDPath("motion.postures.posture","."),"time",IDPath("motion.postures.num",".")));
    members.push_back(fpsvectMapping<USEDATACELL|USEDATACELLFORSIZE>(cvd<LinkExport>(link),"postures","posture","motion"));
    members.push_back(fpsvectMapping<USEDATACELL|USEDATACELLFORSIZE>(cvd<AngleExport>(angle),"postures","posture","motion"));
      members.push_back(fpsvectMapping<USEDATACELL|USEDATACELLFORSIZE>(cvd<NameExport>(name),"postures","posture","motion"));
  }
  //import: xml -> time
  else {
    members.push_back(vectMapping(vd(time),IDPath("motion.postures.posture","."),"time",IDPath("motion.postures.num",".")));
    members.push_back(fvectMapping<SILENTFAIL>(link,IDPath("motion.postures.posture","."),"link",IDPath("motion.postures.num",".")));
    members.push_back(fvectMapping<SILENTFAIL>(angle,IDPath("motion.postures.posture","."),"angle",IDPath("motion.postures.num",".")));
    members.push_back(fvectMapping<SILENTFAIL>(name,IDPath("motion.postures.posture","."),"name",IDPath("motion.postures.num",".")));
  }
}
void DefDescr<Motion>::onInitializationFinished(helium::Memory& mem) {
  //cout << "defmot : oninitfinished" << endl;
}
