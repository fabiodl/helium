/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include "yarpMotor.h"
#include <helium/util/ioUtil.h>
#include <helium/vector/operations.h>
namespace helium{


static const int BUFFERSIZE=1024;


int YarpMotor::getAxes(const std::string& limb){
  int n;
  get(n,limb,"axes");
  return n;
}


  void  YarpMotor::getAxesNum(std::vector<int>& axes){
    axes=axesNum;
  }


  int YarpMotor::getAxesNum(){
    return sum(axesNum);
  }



  YarpMotor::YarpMotor(YarpClient& pc,const YarpPaths& yl):
    YarpRpc(pc),
    client(pc),config(yl),
    axesNum(config.paths.size())
  {        
    for (size_t i=0;i<config.paths.size();++i){
      axesNum[i]=getAxes(config.paths[i]);
      for (int j=0;j<axesNum[i];j++){
	jointmap.push_back(std::make_pair(i,j));
      }
    }

  }

void YarpMotor::rotate(int j,double d){
  set(std::make_pair(jointmap[j].second,d*180/M_PI),config.paths[jointmap[j].first],"pos");
}



  
  void YarpMotor::sendAllAxesScaled(double* v,const std::string& what){
    int off=0;
    for (size_t l=0;l<axesNum.size();++l){
      int n=axesNum[l];
      std::stringstream ss;
      ss<<"("<<v[off]*180/M_PI;
      for (int j=1;j<n;j++){
	ss<<" "<<(v[j+off]*180/M_PI);
      }
      ss<<")";        
      //std::cout<<"USING "<<config.paths[l]<<std::endl;
      set(ss.str(),config.paths[l],what);
      off+=n;
    }
        
  }
  

  


void YarpMotor::rotate(double* a){
  sendAllAxesScaled(a,"poss");
}
  
  void YarpMotor::setVelocity(int j,double v){
    set(std::make_pair(jointmap[j].second,v*180/M_PI),config.paths[jointmap[j].first],"vel");
  }


void YarpMotor::setVelocity(double* a){
  //sendAllAxesScaled(a,"velg");
  for (size_t j=0;j<jointmap.size();j++){
    setVelocity(j,a[j]);
  }
}


double YarpMotor::getEncoder(int j){
  //sendAllAxesScaled(a,"velg");
  double r;
  get(r,config.paths[jointmap[j].first],

      stringBegin()<<"enc "<<jointmap[j].second<<stringEnd());
  return r*M_PI/180;
}



}//helium
