/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \author  Fransiska Basoeki
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

/*

asm files: 
 search for Transform
 saves the Transform info as its child
 get url, recursive to the corresponding wrl (child)

prt files:
 search for points, coordIndex

*/

#include "boxNode.h"

#include "stdio.h"
#include "string.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <helium/destroy/autoDestroy.h>
#include <tinyxml/tinyxml.h>

using namespace std;
using namespace helium::convex;


BoxNode::BoxNode():isCopy(false) {
  boxParent = NULL;
  boxType = BPART;
}

void  BoxNode::copy(const BoxNode& bn){
  boxChildren=bn.boxChildren;
  boxRot=bn.boxRot;
  boxTran=bn.boxTran;
  boxPoint=bn.boxPoint;
  boxCoord=bn.boxCoord;
  boxParent = bn.boxParent;
  boxName = bn.boxName;
  boxType = bn.boxType;
  isCopy=true;
}

BoxNode::BoxNode(const BoxNode& bn){
  copy(bn);
}
BoxNode& BoxNode::operator=(const BoxNode& bn){
  copy(bn);
  return *this;
}

BoxNode::~BoxNode() {
  if (!isCopy){
    destroyS(boxChildren);
    //delete[] boxRot; //replaced with arrays
    //delete[] boxTran;
    //destroyA(boxPoint);
    //destroyA(boxCoord);
  }
}

BoxNode::BoxNode(BoxNode* parent):isCopy(false) {
  boxParent = parent;
  boxType = BPART;
}

BoxNode::BoxNode(string name):isCopy(false) {
  boxName = name;
  double *rot = new double[4];
  double *tran = new double[3];
  rot[0]=1;rot[1]=0;rot[2]=0;rot[3]=0;
  tran[0]=0;tran[1]=0;tran[2]=0;
  boxRot = rot;
  boxTran = tran;
  boxParent = NULL;
  boxType = BPART;
}

BoxNode::BoxNode(string name, double* rot, double* tran, BoxNode* parent):isCopy(false) {
  boxType = BPART;
  boxName = name;
  boxRot = rot;
  boxTran = tran;
  boxParent = parent;
}

void BoxNode::setPoint(vector<double*>& point, vector<int*>& coord) {
  //boxPoint = point;
  //boxCoord = coord;
  //replaced with array
  //cout << "setpoint" << endl;
  //cout << "boxPoint" << boxPoint.size() << " " << point.size() << endl;
  //cout << "coord" << boxCoord.size() << " " << coord.size() << endl;
  for(size_t i=0;i<point.size();i++)
    boxPoint[i] = point[i];
  for(size_t i=0;i<coord.size();i++)
    boxCoord[i] = coord[i];

  //cout << boxName << boxPoint.size() << endl;
}

void BoxNode::setPoint(std::vector<helium::Array<double,3> >& point, std::vector<helium::Array<int,3> >& coord) {
  boxPoint = point;
  boxCoord = coord;
}

void BoxNode::print(BoxNode* boxNode) {
  boxNode->printShort();
  if(boxNode->boxChildren.size()) {
    for(size_t i=0;i<boxNode->boxChildren.size();i++) 
      print(boxNode->boxChildren[i]);
  }
}

void BoxNode::printShort() {
  cout << boxName << " ";
  if(boxParent)
  cout << boxParent->boxName;// << " r:" << boxRot[0] <<  " " << boxRot[1] << " " << boxRot[2] << " " << boxRot[3] << " t:" << boxTran[0] << " "<< boxTran[1] << " "<< boxTran[2] << " p:" << boxPoint.size() << " c:" << boxCoord.size();
  
  /*for(size_t i=0; i<boxPoint.size(); i++) {
    double* p = boxPoint[i];
    cout << " : " << p[0] << " " << p[1] << " " << p[2];
    }*/
  
  cout << endl;
}

void BoxNode::readBox(const char* filename, BoxNode* boxNode) {
  string wrl = "wrl/";
  wrl.append(filename);
  fstream in (wrl.c_str(),fstream::in);

  string s;
  string boxname;

  if(!in) {
    cout << "can't find " << filename << endl;
  }
  else {
    //cout << "reading " << filename << endl;
    
    stringstream ff;
    ff << filename;
    ff >> s;
    
    //is it a part file? if it is, only read point and vector
    if(!s.substr(s.find_last_of("wrl")-6,3).compare("prt")) {
  
      vector<double*> point;
      vector<int*> coord;

      //cout << " prt: " << boxNode->getName() << endl;
      while(in) {
	getline(in,s);
	if(!s.substr(0,5).compare("point")) {
	  getline(in,s);

	  //get point
	  while(s.substr(0,2).compare("]}")) {
	    double* p = new double[3];
	    stringstream pp;
	    pp << s;
	    pp >> p[0] >> p[1] >> p[2];
	    point.push_back(p);
	    getline(in,s);
	  }
	}
	  
	if(!s.substr(0,10).compare("coordIndex")) {
	  char comma;
	  int n;
	  getline(in,s);
	  while(s.substr(0,1).compare("]")) {
	    stringstream ss;
	    ss << s;
	    while(ss >> n >> comma) {
	      // is always triangle
	      int* c = new int[3];
	      c[0] = n;
	      ss >> c[1] >> comma >> c[2] >> comma >> n >> comma;
	      coord.push_back(c);
	    }
	    getline(in,s);
	  }
	  /*
	  for (size_t i=0; i<coord.size();i++) {
	    int* p = coord[i];
	    cout << p[0] << " " << p[1] << " " << p[2] << " " << endl;
	  }
	  */
	  //cout << s << endl;
	}
      }
      //set the vectors to the class
      boxNode->setPoint(point,coord);
      //cout << point.size() << boxNode->getName() << boxNode->getPointSize() << endl;
    }
    
    else {

      while(in) {

	/* //simply reads file names recursively
	   if(!url.compare(s.substr(0,5))) {
	   boxname = s.substr(5,s.find_last_of("wrl")-4);
	   boxNode->readBox(boxname.c_str(),boxNode);
	   }
	*/
         
	getline(in,s);
	
	//cout << "in";	
	if(!s.substr(0,10).compare("DEF Member")) {
	  //cout << "def";

	  //initialize boxnode items
	  double *rot = new double[4];
	  double *tran = new double[3];
	  rot[0]=1;rot[1]=0;rot[2]=0;rot[3]=0;
	  tran[0]=0;tran[1]=0;tran[2]=0;
	  
	  while(s.substr(0,5).compare("url \"") && in) {
	    //cout << "url";
	    getline(in,s);	
	    //if there is transform
	    if(!s.substr(0,9).compare("Transform")) {
	      getline(in,s);
	      if(!s.substr(0,8).compare("rotation")) {
		//cout << s << endl;
		//read rotation
		stringstream ss;
		ss << s.substr(9); //remove "rotation"
		for(int j=0;j<4;j++)
		  ss >> rot[j];
		getline(in,s);
	      }
	      if(!s.substr(0,11).compare("translation")) {
		//cout << s << endl;
		//read translation
		stringstream ss;
		ss << s.substr(11); //remove "translation"
		for(int j=0;j<3;j++)
		  ss >> tran[j];
	      }
	    }
	  }
	  
	  if(in) {
	    boxname = s.substr(5,s.find_last_of("wrl")-4);
	    BoxNode* child = new BoxNode(boxname.substr(0,boxname.size()-8),rot,tran,boxNode);
	    //child->printShort();
	    boxNode->boxChildren.push_back(child);
	    readBox(boxname.c_str(),child);
	  }
	}
      }
    }
    in.close();
  }
}

void BoxNode::writeXml(const char* filename, BoxNode* boxNode) {
  TiXmlDocument doc;
  TiXmlDeclaration * decl = new TiXmlDeclaration("1.0","","");
  doc.LinkEndChild(decl);

  TiXmlElement * element = new TiXmlElement ("boxnodes");
  doc.LinkEndChild(element);

  writeBox(boxNode, element);

  /*
  for(size_t i=0; i<boxNode->boxChildren.size(); i++) {
    msg = new TiXmlElement("boxnode");
    element->LinkEndChild(msg);

    //stringstream ss;
    //double* p = boxNode->boxPoint[i];
    //ss << p[0] << " " << p[1] << " " << p[2];

    msg->SetAttribute("name",boxNode->boxChildren[i]->boxName.c_str());
  }
  */

  doc.SaveFile(filename);
}

void BoxNode::writeBox(BoxNode* boxNode, TiXmlElement* element) {
  TiXmlElement * msg;
  msg = new TiXmlElement("boxnode");
  element->LinkEndChild(msg);

  msg->SetAttribute("name",boxNode->boxName.c_str());

  /*
  if(boxNode->boxParent) {
    msg->SetAttribute("parent",boxNode->boxParent->getName().c_str());
    //cout << boxNode->getName() << " " << boxNode->boxParent->getName() << endl;
  }
  */

  if(sizeof(boxNode->boxRot)) {
    stringstream ss;
    double* p = boxNode->boxRot.data;
    ss << p[0] << " " << p[1] << " " << p[2] << " " << p[3];

    msg->SetAttribute("rotation",ss.str().c_str());
  }

  if(sizeof(boxNode->boxTran)) {
    stringstream ss;
    double* p = boxNode->boxTran.data;
    ss << p[0] << " " << p[1] << " " << p[2];

    msg->SetAttribute("translation",ss.str().c_str());
  }

  if(boxNode->boxPoint.size()) {
    TiXmlElement * points = new TiXmlElement("points");
    msg->LinkEndChild(points);
    
    for(size_t i=0; i<boxNode->boxPoint.size(); i++) {
      TiXmlElement * point = new TiXmlElement("point");
      points->LinkEndChild(point);

      stringstream ss;
      //double* p = boxNode->boxPoint[i];
      //ss << p[0] << " " << p[1] << " " << p[2]; //replaced with array 20111017
      ss << boxNode->boxPoint[i][0] << " " << boxNode->boxPoint[i][1] << " " << boxNode->boxPoint[i][2];
      point->SetAttribute("coord",ss.str().c_str());
    }
  }

  if(boxNode->boxCoord.size()) {
    TiXmlElement * coords = new TiXmlElement("triangles");
    msg->LinkEndChild(coords);
    
    for(size_t i=0; i<boxNode->boxCoord.size(); i++) {
      TiXmlElement * coord = new TiXmlElement("triangle");
      coords->LinkEndChild(coord);

      stringstream ss;
      //int* p = boxNode->boxCoord[i];
      //ss << p[0] << " " << p[1] << " " << p[2];
      ss << boxNode->boxCoord[i][0] << " " << boxNode->boxCoord[i][1] << " " << boxNode->boxCoord[i][2];
      coord->SetAttribute("index",ss.str().c_str());
    }
  }
  
  //cout << boxNode->boxName << " " << boxNode->boxChildren.size() << endl;

  for(size_t i=0; i<boxNode->boxChildren.size(); i++) {
    //cout << boxNode->boxChildren[i]->boxName << endl;
    writeBox(boxNode->boxChildren[i],msg);
  }
}

bool BoxNode::readXml(const char* filename, BoxNode* boxNode) {
  TiXmlDocument doc(filename);
  if(!doc.LoadFile()) {
    cout << "error: BoxNode cannot find " << filename << endl;
    return false;
  }
  TiXmlElement* root = doc.FirstChildElement("boxnodes");
  if(!root) {
    cout << "error: " << filename <<" has wrong BoxNode xml format" << endl;
    doc.Clear();
    return false;
  }
  cout << "boxnode xml loaded " ;
  readXmlBox(root->FirstChildElement("boxnode"),boxNode);
  cout << endl;
  return true;
}

void BoxNode::readXmlBox(TiXmlElement* element, BoxNode* boxNode) {

  boxNode->boxName = element->Attribute("name");

  /*
  if(element->Attribute("parent")) {
    boxNode->boxParent = getBox(element->Attribute("parent"),boxNode);
    cout << boxNode->getName() << " " << boxNode->boxParent->getName() << endl;
  }
  */
  //cout << "readxmlbox";
  if(element->Attribute("rotation")) {
    stringstream ss;
    ss << element->Attribute("rotation");
    helium::Array<double,4> rot;
    ss >> rot[0] >> rot[1] >> rot[2] >> rot[3];
    boxNode->boxRot = rot;
  }
  if(element->Attribute("translation")) {
    stringstream ss;
    ss << element->Attribute("translation");
    helium::Array<double,3> tran;     //double *tran = new double[3];
    ss >> tran[0] >> tran[1] >> tran[2];
    boxNode->boxTran = tran;
    //cout << boxNode->boxTran[0] << endl;
  }
  if(element->FirstChildElement("points")) {
    for(TiXmlElement* point=element->FirstChildElement("points")->FirstChildElement("point");point;point=point->NextSiblingElement()) {
      stringstream ss;
      ss << point->Attribute("coord");
      //double *p = new double[3]; replaced with array 20111017
      helium::Array<double,3> p;
      ss >> p[0] >> p[1] >> p[2];
      boxNode->boxPoint.push_back(p);
      if (ss.fail()){
	cerr<<"Bad coord!"<<point->Attribute("coord")<<endl;
	exit(1);
      }
    }
  }
  if(element->FirstChildElement("triangles")) {
    for(TiXmlElement* tri=element->FirstChildElement("triangles")->FirstChildElement("triangle");tri;tri=tri->NextSiblingElement()) {
      stringstream ss;
      ss << tri->Attribute("index");
      //int *p = new int[3]; replaced with array 20111017
      helium::Array<int,3> p;
      ss >> p[0] >> p[1] >> p[2];
      boxNode->boxCoord.push_back(p);
    }
  }
  //else {
  for(TiXmlElement* box=element->FirstChildElement("boxnode");box;box=box->NextSiblingElement()) {
    BoxNode* child = new BoxNode(boxNode);
    boxNode->boxChildren.push_back(child);
    readXmlBox(box,child);
  }
  //}

  int si = boxNode->boxChildren.size();
  if(si)
    cout << si << " ";
}

BoxNode* BoxNode::getBox(const char* name,BoxNode* boxNode) {  
  if(!boxNode->boxName.compare(name)) {
    //cout << "ok " << name << endl;
    return boxNode;
  }
  for(size_t i=0; i<boxNode->boxChildren.size(); i++) {
    BoxNode* b = getBox(name, boxNode->boxChildren[i]);
      if(b != NULL)
	return b;
  }
  return NULL;
}

  //cout << boxNode->boxName << endl;

  /*
  BoxNode* parent = new BoxNode;
  vector <int> index;
  index.push_back(0);
  cout << index.back()++ << endl;
  
  while(boxNode->boxName.compare(name)) {
    if(boxNode->boxChildren.size()) {
      if(!index.size())
	index.push_back(0);
      else {
	index.back()++;
	boxNode->boxChildren[index.back()];
      }
    }
    else
      if(boxNode->boxParent) 
	boxNode = boxNode->boxParent;
      else {
	cout << "error" << endl;
	break;
      }
  }

  cout << name << " " << boxNode->boxName << endl;

  if(!boxNode->boxName.compare(name))
    return boxNode;
  else
    return NULL;
  */

/*
BoxNode* BoxNode::getBoxChild(const char* name,BoxNode* boxNode) {
  for(size_t i=0; i<boxNode->boxChildren.size(); i++) {
    if(!boxNode->boxChildren[i]->boxName.compare(name)) {
      boxNode = boxNode->boxChildren[i];
      break;
    }
  }
  return boxNode;
}
*/
