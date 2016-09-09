/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \author  Fransiska Basoeki
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include "jointParser.h"

#include "stdio.h"
#include "string.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <helium/destroy/autoDestroy.h>
#include <tinyxml/tinyxml.h>
#include <cstring>
using namespace std;
using namespace helium::convex;

JointInfo::JointInfo() {
    jointRot[0]=1;jointRot[1]=0;jointRot[2]=0;jointRot[3]=0;
    jointTran[0]=0;jointTran[1]=0;jointTran[2]=0;
  }
JointInfo::JointInfo(const std::string& name):jointName(name)     {
    jointRot[0]=1;jointRot[1]=0;jointRot[2]=0;jointRot[3]=0;
    jointTran[0]=0;jointTran[1]=0;jointTran[2]=0;
  }
JointInfo::JointInfo(const std::string& name,const double* rot,const double* tran):jointName(name) {

    memcpy(jointRot,rot,sizeof(double)*4);
    memcpy(jointTran,tran,sizeof(double)*3);
  }
  std::string JointInfo::getName() {
    return jointName;
  }
void JointInfo::setName(const std::string& name) {
    jointName = name;
  }


JointParser::JointParser():mirrorPointR(NULL),mirrorPointL(NULL) {    
}

JointParser::~JointParser() {
  destroyA(jointPoint);
  destroyA(jointCoord);
  if (mirrorPointR!=NULL)
    delete mirrorPointR;
  if (mirrorPointL!=NULL)
  delete mirrorPointL;
  
}

// reading a part
void JointParser::readPoint(const char* filename) {
  string wrl = "wrl/";
  wrl.append(filename);
  wrl.append("_prt.wrl");
  fstream in (wrl.c_str(),fstream::in);
  if(!in) {
    cout << "can't find WRL file " << filename << endl;
  }
  else {
    //cout << "read point" << endl;
    string s;
    while(in) {
      getline(in,s);
      /* in wrl file, points are specified in this format
	point  [
	0.00764 0.04219 -0.01323,
	0.00384 0.04219 -0.01764,
	...
	]}
      */
      if(!s.substr(0,5).compare("point")) { 
	getline(in,s);
	//get point
	while(s.substr(0,2).compare("]}")) {
	  double* p = new double[3];
	  stringstream pp;
	  pp << s;
	  pp >> p[0] >> p[1] >> p[2];
	  jointPoint.push_back(p);
	  getline(in,s);
	}
      }

      /* in wrl file, how points are connected are specified in this format
	 (-1 indicates the ):
	 coordIndex [
	 5, 1, 0, -1, 
	 6, 5, 0, -1,
	 1, 5, 4, -1,
	 1, 4, 3, -1,
	 2, 1, 3, -1,
	 ]
       */
      if(!s.substr(0,10).compare("coordIndex")) {
	getline(in,s);

	char comma;
	stringstream ss;
	int n;
	    
	ss << s;
	while(ss >> n >> comma) {
	  int* c = new int[3];
	  c[0] = n;
	  ss >> c[1] >> comma >> c[2] >> comma >> n >> comma;
	  //cout <<  c[0] << comma << c[1] << comma << c[2] << comma << endl;
	  jointCoord.push_back(c);
	}	
      }
    }
  }
}

// reading an "assembly"
void JointParser::readJoint(const char* filename) {
  cout << "readJoint" << endl;
  string wrl = "wrl/";
  wrl.append(filename);
  fstream in (wrl.c_str(),fstream::in);
  if(!in) {
    cerr << "error: cannot find WRL file" << endl;
  }
  else {
    //cout << "found WRL file" << endl;
    string s;
    stringstream ss;

    vector <double> temp;
    string jointname;

    while(in) {
      getline(in,s);


      //transformation for each part

      if(!s.substr(0,9).compare("Transform")) {
	//cout << endl << s << endl;

	getline(in,s);
	
	double rot[4];
	double tran[3];

	if(!s.substr(0,8).compare("rotation")) {
	  //cout << s << endl;
	  //read rotation
	  ss << s.substr(9); //remove "rotation"
	  for(int j=0;j<4;j++)
	    ss >> rot[j];
	  getline(in,s);
	}
	else {
	  rot[0]=1;rot[1]=0;rot[2]=0;rot[3]=0;
	}
	if(!s.substr(0,11).compare("translation")) {
	  //read translation
	  ss << s.substr(11); //remove "translation"
	  for(int j=0;j<3;j++)
	    ss >> tran[j];
	}
	  
	getline(in,s); //children[
	getline(in,s); //Inline {
	
	getline(in,s);
	if(!s.substr(0,5).compare("url \"")) {
	  jointname = s.substr(5,s.find_last_of("wrl")-12); 
	  //-4 to get the length of filename, -8 to remove _prt.wrl
	}


	//for mirroring parts
	if(!jointname.compare("mirrorr"))
	  mirrorPointR = new JointInfo(jointname,rot,tran);
	else
	  if(!jointname.compare("mirrorl"))
	    mirrorPointL = new JointInfo(jointname,rot,tran);
	  else 
	    jointInfo.push_back(JointInfo(jointname,rot,tran));
      }
      if(!in) break;
    }
  }
  in.close();  
  //get the info of joint points from the first file
  readPoint(jointInfo[0].jointName.c_str());
  cout << "joint wrl loaded, " <<  jointInfo.size() << " joints" << endl;
}

void JointParser::writeXml(const char* filename) {
  TiXmlDocument doc;
  TiXmlDeclaration * decl = new TiXmlDeclaration("1.0","","");
  doc.LinkEndChild(decl);

  TiXmlElement * element = new TiXmlElement ("joints");
  doc.LinkEndChild(element);

  
  //Parsing Mirror
  if(mirrorPointL) {
    TiXmlElement * m = new TiXmlElement("mirror");
    element->LinkEndChild(m);
    
    m->SetAttribute("name",mirrorPointL->jointName.c_str());
    
    if(sizeof(mirrorPointL->jointTran)) {
      stringstream ss;
      double* p = mirrorPointL->jointTran;
      ss << p[0] << " " << p[1] << " " << p[2];
      m->SetAttribute("translation",ss.str().c_str());
    }
    
    if(sizeof(mirrorPointL->jointRot)) {
      stringstream ss;
      double* p = mirrorPointL->jointRot;
      ss << p[0] << " " << p[1] << " " << p[2] << " " << p[3];
      m->SetAttribute("rotation",ss.str().c_str());
    }
  }
  if(mirrorPointR) {
    TiXmlElement * m = new TiXmlElement("mirror");
    element->LinkEndChild(m);
    
    m->SetAttribute("name",mirrorPointR->jointName.c_str());
    
    if(sizeof(mirrorPointR->jointTran)) {
      stringstream ss;
      double* p = mirrorPointR->jointTran;
      ss << p[0] << " " << p[1] << " " << p[2];
      m->SetAttribute("translation",ss.str().c_str());
    }
    
    if(sizeof(mirrorPointR->jointRot)) {
      stringstream ss;
      double* p = mirrorPointR->jointRot;
      ss << p[0] << " " << p[1] << " " << p[2] << " " << p[3];
      m->SetAttribute("rotation",ss.str().c_str());
    }
  }

  //Parsing Joint Points and Triangles
  if(jointPoint.size()) {
    TiXmlElement * points = new TiXmlElement("points");
    element->LinkEndChild(points);
    
    for(size_t i=0; i<jointPoint.size(); i++) {
      TiXmlElement * point = new TiXmlElement("point");
      points->LinkEndChild(point);

      double* p = jointPoint[i];
      stringstream ss;
      ss << p[0] << " " << p[1] << " " << p[2];
      point->SetAttribute("coord",ss.str().c_str());
    }
  }

  if(jointCoord.size()) {
    TiXmlElement * coords = new TiXmlElement("triangles");
    element->LinkEndChild(coords);
    
    for(size_t i=0; i<jointCoord.size(); i++) {
      TiXmlElement * coord = new TiXmlElement("triangle");
      coords->LinkEndChild(coord);

      int* p = jointCoord[i];
      stringstream ss;
      ss << p[0] << " " << p[1] << " " << p[2];
      coord->SetAttribute("index",ss.str().c_str());
    }
  }

  //Parsing Joints
  for(size_t i=0;i<jointInfo.size();i++) {
    TiXmlElement * msg;
    msg = new TiXmlElement("joint");
    element->LinkEndChild(msg);

    msg->SetAttribute("name",jointInfo[i].jointName.c_str());

    //msg->SetAttribute("rotation",ss.str().c_str());

    if(sizeof(jointInfo[i].jointTran)) {
      stringstream ss;
      double* p = jointInfo[i].jointTran;
      ss << p[0] << " " << p[1] << " " << p[2];
      msg->SetAttribute("translation",ss.str().c_str());
    }

    if(sizeof(jointInfo[i].jointRot)) {
      stringstream ss;
      double* p = jointInfo[i].jointRot;
      ss << p[0] << " " << p[1] << " " << p[2] << " " << p[3];
      msg->SetAttribute("rotation",ss.str().c_str());
    }

  }

  doc.SaveFile(filename);
}

bool JointParser::readXml(const char* filename) {
  TiXmlDocument doc(filename);
  if(!doc.LoadFile()) {
    cout << "error: JointParser cannot find " << filename << endl;
    return false;
  }
  TiXmlElement* root = doc.FirstChildElement("joints");
  if(!root) {
    cout << "error: " << filename << " has wrong JointParser xml format" << endl;
    doc.Clear();
    return false;
  }
  
  //parsing mirror
  for(TiXmlElement* mirror=root->FirstChildElement("mirror");mirror;mirror=mirror->NextSiblingElement()) {
    string name = "";
    if(mirror->Attribute("name"))
      name = mirror->Attribute("name");
    if(!name.substr(0,6).compare("mirror")) {
      double tran[3];
      double rot[4];
      
      if(mirror->Attribute("translation")) {
	stringstream ss;
	ss << mirror->Attribute("translation");
	ss >> tran[0] >> tran[1] >> tran[2];
      }  
      if(mirror->Attribute("rotation")) {
	stringstream ss;
	ss << mirror->Attribute("rotation");
	ss >> rot[0] >> rot[1] >> rot[2] >> rot[3];
      }  
      if(!name.compare("mirrorr"))
	mirrorPointR = new JointInfo(name,rot,tran);
      else
	mirrorPointL = new JointInfo(name,rot,tran);
    }
  }

  //parsing points
  if(root->FirstChildElement("points")) {
    for(TiXmlElement* point=root->FirstChildElement("points")->FirstChildElement("point");point;point=point->NextSiblingElement()) {
      stringstream ss;
      ss << point->Attribute("coord");
      double *p = new double[3];
      ss >> p[0] >> p[1] >> p[2];
      jointPoint.push_back(p);
    }
  }
  //parsing triangles
  if(root->FirstChildElement("triangles")) {
    for(TiXmlElement* tri=root->FirstChildElement("triangles")->FirstChildElement("triangle");tri;tri=tri->NextSiblingElement()) {
      stringstream ss;
      ss << tri->Attribute("index");
      int *p = new int[3];
      ss >> p[0] >> p[1] >> p[2];
      jointCoord.push_back(p);
    }
  }

  //parsing joint
  for(TiXmlElement* joint=root->FirstChildElement("joint");joint;joint=joint->NextSiblingElement()) {
    double tran[3];
    double rot[4];
    
    if(joint->Attribute("translation")) {
      stringstream ss;
      ss << joint->Attribute("translation");
      ss >> tran[0] >> tran[1] >> tran[2];
    }  
    if(joint->Attribute("rotation")) {
      stringstream ss;
      ss << joint->Attribute("rotation");
      ss >> rot[0] >> rot[1] >> rot[2] >> rot[3];
    }  
    jointInfo.push_back(JointInfo(joint->Attribute("name"),rot,tran));
  }

  cout << "joint xml loaded, " << jointInfo.size() << " joints" << endl;
  return true;
}

JointInfo* JointParser::getJoint(const char* name) {
  for(size_t i=0;i<jointInfo.size();i++) {
    if(!jointInfo[i].jointName.compare(name)) {
      //cout << "ok " << name << endl;
      return &jointInfo[i];
    }
  }
  return NULL;
}

void JointParser::print() {
  for(size_t i=0;i<jointInfo.size();i++) {
    cout << jointInfo[i].jointName << " " << jointInfo[i].jointRot[0] << " "<< endl;
  }
}
