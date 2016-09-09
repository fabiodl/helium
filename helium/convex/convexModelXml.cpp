/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \author  Fransiska Basoeki
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <helium/util/ioUtil.h>
#include <helium/core/exception.h>
#include <helium/util/stringUtil.h>
#include "convexModel.h"
#include <tinyxml/tinyxml.h>


using namespace std;
using namespace helium::convex;
/*ConvexModel::ConvexModel() {
}

ConvexModel::~ConvexModel() {
}*/

bool ConvexModel::loadFromFile(const char* modelFilename) {
  if(loaded && jointParser.getSize() > 0)
    return false;
  TiXmlDocument doc(modelFilename);
  doc.LoadFile();
  if (doc.Error()) throw exc::XMLError(doc.ErrorDesc());    

  TiXmlElement* root = doc.FirstChildElement("robot");
  if(!root) {
    throw exc::BadFileFormat("xml robot geometry model",modelFilename,"no robot root");
  }

  string jointFile = root->FirstChildElement("joint")->Attribute("filename");
  string boxFile = root->FirstChildElement("box")->Attribute("filename");

#ifndef WIN32
  string folder;
  helium::getFolder(string(modelFilename),folder);

  stringstream ss;
  ss << folder << "/" << jointFile;
  jointFile = ss.str();
  ss.str("");
  ss << folder << "/" << boxFile;
  boxFile = ss.str();
#endif


  //initialize box and joint
  boxNode.readXml(boxFile.c_str(),&boxNode);
  //boxNode.print(boxNode);
  jointParser.readXml(jointFile.c_str());

  //get mirrorpoint from jointparser
  mirrorPointL = jointParser.getMirrorL();
  mirrorPointR = jointParser.getMirrorR();

  initializeLink(root);
  initializeCollMargin(root,modelFilename);

  loaded = true;
  return true;
}


void ConvexModel::initializeLink(TiXmlElement *root) {
  //loop for every link in the xml
  for(TiXmlElement* link=root->FirstChildElement("link"); link; link=link->NextSiblingElement("link")) {
    ConvexLink* convexLink =ad.s= new ConvexLink();

    //insert parent, id, motorid, multiply, offset
    stringstream ss;
    ss << link->Attribute("id") << " " << link->Attribute("parent") << " " << link->Attribute("motorId") << " " << link->Attribute("multiply") << " " << link->Attribute("offset");
    ss >> convexLink->id >> convexLink->parent >> convexLink->motorId >> convexLink->multiply >> convexLink->offset;

    //if it's a mirror link, add mirror parts
    if (link->Attribute("mirroring")) {
      string m = link->Attribute("mirroring");
      JointInfo *infom =ad.s= new JointInfo();
      //copy the mirrored part info
      *infom = *jointParser.getJoint(link->Attribute("mirroring"));
      //change name "partname" -> "partnamemi"
      infom->setName(m.append("mi"));

      convexLink->info = infom;
      convexLink->toMirror = true;
    }
    else {
      convexLink->info = jointParser.getJoint(link->Attribute("name"));
      convexLink->toMirror = false;
    }

    //if the convexlink's info is still null
    if (convexLink->info == NULL) {
      //if a mirror part, the mirroring joint has wrong name
      if(link->Attribute("mirroring"))
	cout << "error: found no joint to mirror, check " <<  link->Attribute("mirroring") << endl;
      else
	convexLink->info =ad.s= new JointInfo(link->Attribute("name"));
      //**jbody, or falsely named joint will still get a fake JointInfo
    }

    //connecting boxnode
    if(link->Attribute("mirroring")) {
      //get boxpart from the mirroring
      int lid = getPartId(link->Attribute("mirroring"),true).first;
      //get convexlink id to copy part from it
      if(lid>=0) {
	for(size_t i=0;i<links[lid]->part.size();i++) {
	  BoxNode* b = ad.s= new BoxNode();
	  //copy boxnode properties
	  *b = *links[lid]->part[i];
	  //add mi for mirrored part
	  b->setName(b->getName().append("mi"));
	  convexLink->part.push_back(b);
	}
      }
    }
    else {
      //get box pointer
      for(TiXmlElement* part=link->FirstChildElement("part");part;part=part->NextSiblingElement()) {
	BoxNode* b; //= new BoxNode();
	b =  boxNode.getBox(part->Attribute("name"),&boxNode);
	if(b!=NULL)
	  convexLink->part.push_back(b);
      }
    }
    //cout << convexLink->info->getName() << " " << convexLink->part.size() << endl;
    links.push_back(convexLink);
  }//finished adding links and parts 

}

void ConvexModel::initializeCollMargin(TiXmlElement *root,const char* modelFilename) {
  //parsing collmargin
  if(root->FirstChildElement("collmargin")) {
    stringstream ssr;
    ssr << root->FirstChildElement("collmargin")->FirstChildElement("default")->Attribute("margin");
    ssr >> defaultMargin;
    for(TiXmlElement* coll=root->FirstChildElement("collmargin")->FirstChildElement("coll");coll;coll=coll->NextSiblingElement()) {
      //updates margin
      double cmargin=defaultMargin;
      if(coll->Attribute("margin")) {
	stringstream ss;
	ss << coll->Attribute("margin");
	ss >> cmargin;
      }

      //name of two objects
      string obj1 = coll->Attribute("obj1");
      string obj2 = coll->Attribute("obj2");

      //gets the link vector id of objects
      pair<int,int> li1 = getPartId(obj1.c_str(),true);
      pair<int,int> li2 = getPartId(obj2.c_str(),true);

      //only do if both names are valid objects
      
      if(li1.first>-1 && li2.first>-1) {

	//start and end of for loop (loop through all parts in the link of the object)
	int s1=0;
	int e1=links[li1.first]->part.size();
	int s2=0;
	int e2=links[li2.first]->part.size();

	//if object is a part, only loop once
	if(li1.second>-1) {
	  s1=li1.second;
	  e1=s1+1;
	}
	if(li2.second>-1) {
	  s2=li2.second;
	  e2=s2+1;
	}

	//check if the object's joint is a mirror
	int m1 = getId(links[li1.first]->info->getName().append("mi").c_str());
	int m2 = getId(links[li2.first]->info->getName().append("mi").c_str());

	//if not found, push the original
	if(m1<0)
	  m1 = links[li1.first]->id;
	if(m2<0)
	  m2 = links[li2.first]->id;

	for(int i=s1;i<e1;i++) {
	  for(int j=s2;j<e2;j++) {
	    margin[pair<pair<int,int>,pair<int,int> >(pair<int,int>(links[li1.first]->id,i),pair<int,int>(links[li2.first]->id,j))]=cmargin;
	    //also add margin for mirror
	    if(!links[li1.first]->toMirror &&!links[li2.first]->toMirror) {
	      margin[pair<pair<int,int>,pair<int,int> >(pair<int,int>(m1,i),pair<int,int>(m2,j))]=cmargin;
	    }
	  }
	}
	
      }
      else
	cout << "error: " << obj1 << " is " << li1.first << ", " << obj2 << " is " << li2.first << ", check the name of -1 in " << modelFilename<< endl;
         
    }
    
  }
}


void ConvexModel::printLink() {
  for(size_t i=0;i<links.size();i++) {
    cout << links[i]->info->getName() << " " << links[i]->toMirror << " " << links[i]->part.size() << " ";
    if(links[i]->part.size())
      cout << links[i]->part[0]->getName();
    cout << endl;
  }
}

void ConvexModel::printMargin() {
  //print collision
  cout << "collmargin: " << margin.size() << " parts margin defined, " << defaultMargin*1000 << "mm default margin" << endl;
  for (map<std::pair< std::pair<int,int>,std::pair<int,int> >,double >::const_iterator iter = margin.begin(); iter != margin.end(); ++iter )
    cout << iter->first.first.first  << " " 
	 << iter->first.first.second  << " + " 
	 << iter->first.second.first  << " " 
	 << iter->first.second.second  << " = " 
	 << iter->second << endl;
}

//return id
int ConvexModel::getId(const char* name) {
  for(size_t i=0;i<links.size();i++) {
    if(!links[i]->info->getName().compare(name)) {
      return links[i]->id;
    }
  }
  //cout << "error: getId() link " << name << " not found" << endl;
  return -1;
}

pair<int,int> ConvexModel::getPartId(const char* name, bool returnVectorId) const {
  //get the correct links's id from the attribute id
  int a=-1;
  int b=-1;
  for(size_t j=0;j<links.size();j++) {
    if(!links[j]->info->getName().compare(name))
      returnVectorId?a=j:a=links[j]->id;
    for(size_t i=0;i<links[j]->part.size();i++) {
      if(!links[j]->part[i]->getName().compare(name)) {
	returnVectorId?a=j:a=links[j]->id;
	b=i;
      }
    }
  }
  //cout << "error: getPartId() part " << name << " not found" << endl;
  return make_pair(a,b);
}
