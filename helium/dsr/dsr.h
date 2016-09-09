#ifndef HE_DSR_
#define HE_DSR_

#include <helium/vector/array.h>

namespace helium{
class DsrActivity{
 public:
  virtual double getActivity()=0;
};


class DistanceActivity:public DsrActivity{
 public:
  Array<double> target;
  double* val; 
  
  DistanceActivity(const double* ptarget,double* pval,int pn):
    target(pn),val(pval){
    target=ptarget;    
  }
  
  double getActivity(){    
    return -euclideanDistance(target,val);
  }
  
};

class DsrMotorCommand{
 public:
  virtual void setRandom()=0;
  virtual void addPerturbation()=0;
  virtual void getMotorCommand(double*)=0;
};

class DsrLinearMC:public DsrMotorCommand{
 public:
  Array<double> u;
  Array<double> du;
  Array<double> minv;
  Array<double> maxv;
  double pe,me;

 DsrLinearMC(int n,double pminv,double pmaxv,double noise):u(n),du(n),minv(n),maxv(n){
   zero(u);
   setAll(minv,pminv);
   setAll(maxv,pmaxv);
    setNoise(noise);
  }

  void setNoise(double e){
    pe=e;
    me=-e;
  }
    
  void setRandom(){
    //std::cout<<"setting random"<<std::endl;
    randu(u,minv,maxv,u.size());
    //std::cout<<"un="<<u<<std::endl;
  }

  void addPerturbation(){
    du.setRandu(me,pe);
    u.sum(du);
	u.limit(minv,maxv);
  }

  void getMotorCommand(double* eu){
    u.get(eu);
  }
  const double* getU(){
    return u();
  }

};


class DsrController{
  DsrActivity* a;
  DsrMotorCommand* c;
  double prevA;
 public:
  DsrController(DsrActivity* pa,DsrMotorCommand* pc):a(pa),c(pc){
    prevA=a->getActivity();
  }

  void update(){
    double ac=a->getActivity();
    //std::cout<<"activity "<<ac<<std::endl;
    if (ac>=prevA)
      c->addPerturbation();
    else
      c->setRandom();
    prevA=ac;
  }
    
};

}
#endif
