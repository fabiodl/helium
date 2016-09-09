/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */


#ifndef HE_GYROSTRUCTURES
#define HE_GYROSTRUCTURES

#include <helium/system/cellDescription.h>
#include <helium/math/algebra.h>

namespace helium{

  class GyroCalib{
  public:
    double xoffset,yoffset,zoffset;
    double xratio,yratio;
    GyroCalib():xoffset(NAN),yoffset(NAN),zoffset(NAN),xratio(NAN),yratio(NAN){}
    void getRototranslation(double *m,const double * data);
    void getRightToRightRototranslation(double *m,const double * data);
    void setCalib(const double* c);
  };

  template<>
  class DefDescr<GyroCalib>:public Description<GyroCalib>{
  public:
    DefDescr(GyroCalib&);
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };


  template<int flags>
  class FDefDescr<GyroCalib,flags>:public Description<GyroCalib>{
  public:
    FDefDescr(GyroCalib& g): Description<GyroCalib>(g){
  }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(fscalarMapping<flags>(obj.xoffset,"xoffset"));
      members.push_back(fscalarMapping<flags>(obj.yoffset,"yoffset"));
      members.push_back(fscalarMapping<flags>(obj.zoffset,"zoffset"));
      members.push_back(fscalarMapping<flags>(obj.xratio,"xratio"));
      members.push_back(fscalarMapping<flags>(obj.yratio,"yratio"));
    }
  };


}




#endif
