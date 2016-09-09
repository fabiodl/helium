/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \author  Fransiska Basoeki
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef JOINTPARSER
#define JOINTPARSER

#include <iostream>
#include <vector>
#include <string>


namespace helium{
namespace convex{
class JointInfo {
  std::string jointName;
 public:
  double jointRot[4];
  double jointTran[3];
  JointInfo();
  JointInfo(const std::string& name);
  JointInfo(const std::string& name,const double* rot,const double* tran);
  std::string getName();
  void setName(const std::string& name);
  friend class JointParser;
};

class JointParser {
 public:
  JointParser();
  virtual ~JointParser();
  void readPoint(const char* filename);
  void readJoint(const char* filename);
  void writeXml(const char* filename);
  bool readXml(const char* filename);
  JointInfo* getJoint(const char* name);
  size_t getSize() {
    return jointInfo.size();
  }
  void print();
 private:
  std::vector<double*> jointPoint;
  std::vector<int*> jointCoord; //coordIndex
  std::vector <JointInfo> jointInfo;
  JointInfo *mirrorPointR, *mirrorPointL;
 public:
  JointInfo* getMirrorL() {
    return mirrorPointL;
  }
  JointInfo* getMirrorR() {
    return mirrorPointR;
  }
};
};
/*
class JointParser {
 public:
  JointParser();
  virtual ~JointParser();
  void readJoint(const char* filename);
 private:
  std::vector <std::string> jointName;
  std::vector <std::vector<double> > jointRot;
  std::vector <std::vector<double> > jointTran;
};
*/
}
#endif //JOINTPARSER
