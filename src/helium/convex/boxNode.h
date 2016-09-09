/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \author  Fransiska Basoeki
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef BOXNODE
#define BOXNODE

#include <iostream>
#include <vector>
#include <string>

#include <helium/vector/array.h>

class TiXmlElement;
namespace helium{
  namespace convex{
class BoxNode {
 public:
  enum BoxNodeType {BJOINT,BPART,BMOTORS,BCARTESIAN};
  std::vector<BoxNode*> boxChildren;
  BoxNodeType boxType;
 private:
  std::string boxName;  
  bool isCopy;
  void copy(const BoxNode& bn);
 public:
  BoxNode& operator=(const BoxNode& bn);
  BoxNode(const BoxNode& bn);
  helium::Array<double,4> boxRot;
  helium::Array<double,3> boxTran;//double* boxTran;
  std::vector<helium::Array<double,3> > boxPoint;//std::vector<double*> boxPoint;
  std::vector<helium::Array<int,3> > boxCoord;//std::vector<int*> boxCoord; //coordIndex
  BoxNode* boxParent;
  BoxNode();
  virtual ~BoxNode();
  BoxNode(std::string name);
  BoxNode(std::string name, double* rot, double* tran, BoxNode* parent);
  BoxNode(BoxNode* parent);
  void setPoint(std::vector<double*>& point, std::vector<int*>& coord);
  void setPoint(std::vector<helium::Array<double,3> >& point, std::vector<helium::Array<int,3> >& coord);
  void readBox(const char* filename, BoxNode* boxNode);
  void writeXml(const char* filename, BoxNode* boxNode);
  void writeBox(BoxNode* boxNode, TiXmlElement* element);
  bool readXml(const char* filename, BoxNode* boxNode);
  void readXmlBox(TiXmlElement* element, BoxNode* boxNode);
  void print(BoxNode* boxNode); 
  void printShort();
  std::string getName() {
    return boxName;
  }
  void setName(std::string name) {
    boxName = name;
  }
  size_t getPointSize() {
    return boxPoint.size();
  }
  BoxNode* getBox(const char* name,BoxNode* boxNode);
};

  }//convex
}//helium


#endif //BOXNODE

/*//structure

b0000
 b2000
  arm
 b3000
  leg
 neck

 */

/*//usage: 20101004

  string filename = "b0000sbox_asm.wrl";
  BoxNode* boxNode = new BoxNode(filename.substr(0,filename.size()-8));

  //from wrl to vector
  //boxNode->readBox(filename.c_str(),boxNode);

  //from vector to xml
  //boxNode->writeXml("boxnode.xml",boxNode);
  
  //from xml to vector
  //boxnodeEdited.xml contains the edited joint names
  boxNode->readXml("boxnodeEdited.xml",boxNode);

  //print nodes
  //boxNode->print(boxNode);

  //contains mirror joint
  JointParser jointParser;
  filename = "b0000sjoint_asm.wrl";
  //from wrl to vector
  jointParser.readJoint(filename.c_str());
  //from vector to xml
  jointParser.writeXml("joint.xml");

  
  ConvexModel con(&jointParser,boxNode);
  //from (hand written) xml to vector
  if(con.loadFromFile("model.xml")) {
    cout << "convex xml loaded, " << con.links.size() << " links" << endl;
    //con.print();
  }
 
*/
