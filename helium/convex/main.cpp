// to parse wrl to xml to vector

#include "boxNode.h"
#include "jointParser.h"
#include "convexModel.h"
#include <string>

using namespace std;

int main(int argc, char** argv) {

  
  //string filename = "b0000sbox_asm.wrl";
  //BoxNode* boxNode = new BoxNode(filename.substr(0,filename.size()-8));

  //from wrl to vector
  //boxNode->readBox(filename.c_str(),boxNode);

  //from vector to xml
  //boxNode->writeXml("v4gParts.xml",boxNode);//convextestBox.xml"
  //from xml to vector
  //boxnodeEdited.xml contains the edited joint names
  //boxNode->readXml("boxnodeEdited.xml",boxNode);
  //boxNode->readXml("v4gParts.xml",boxNode);

  //print nodes
  //boxNode->print(boxNode);

  /*
  //contains mirror joint
  JointParser jointParser;
  string filename = "b0000sjoint_asm.wrl";//convextestJoint_asm.wrl";
  //from wrl to vector
  jointParser.readJoint(filename.c_str());
  //from vector to xml
  jointParser.writeXml("v4gJoint.xml");
  */

  
  //ConvexModel con(&jointParser,boxNode);
  convex::ConvexModel con;
  //from (hand written) xml to vector
  if(con.loadFromFile("v4gModel_debug.xml")) {
    cout << "convex xml loaded, " << con.links.size() << " links" << endl;
    con.printMargin();
  }
  
  
  return 0;
}
