/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.1
 * Release_flags ge
 */

#ifndef MOTIONPARSER
#define MOTIONPARSER

#include <tinyxml/tinyxml.h>
#include <helium/robot/frame.h>

#include <helium/system/cellDescription.h>
#include <helium/xml/xmlLoader.h>

namespace helium{


  typedef helium::Time Link;
  typedef helium::Posture Angle;
  typedef std::string Name;

class LinkExport:public Description<Link>{
public:
  LinkExport(Link& s):Description<Link>(s){}
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
};

class AngleExport:public Description<Angle>{
public:
  AngleExport(Angle& s):Description<Angle>(s){}
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
};

class NameExport:public Description<Name>{
public:
  NameExport(Name& s):Description<Name>(s){}
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
};

template<> 
class DefDescr<Motion>:public Description<Motion>{

  std::vector<helium::Time> time;
  std::vector<Link> link;
  std::vector<Angle> angle;
  std::vector<std::string> name;
  
public:

  DefDescr(Motion& s):Description<Motion>(s){}    
  void import(Memory& mem,const IDPath &path);
  void exportRef(Memory& mem,const IDPath &path);


  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  void onInitializationFinished(helium::Memory& mem);
};


class MotionParser {
  const std::string &robot;
 public:
  MotionParser(const std::string &r);
  void openMotion(helium::Motion &motion, const char* filename);
  void saveMotion(const helium::Motion &motion, const char* filename);
};

}//helium
#endif
