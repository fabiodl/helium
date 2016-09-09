#ifndef HE_SENSORCOMP
#define HE_SENSORCOMP

namespace helium{


class CombineFilterComp:public sensor::GenericFilterComputation<double,double>{
  MouseSimulatedTouchSensor& msts;
  
  double saturate(double x){
    return 1-exp(-2*x);
  }

  void compute(double* r,const double *s,const HwIdList& what){    
    for (size_t i=0;i<what.ids.size();i++){
      int idx=what.ids[i];
      r[idx]=saturate(s[idx]);
    }
    msts.addTo(r,what.ids);
  }

  void  getRequired(HwIdList& r,const HwIdList& s){
    r=s;
  }
  void getGenerated(HwIdList& gen,const HwIdList& id){
    gen=id;
  }

public:
  CombineFilterComp(MouseSimulatedTouchSensor& pmsts):msts(pmsts){
  }

};








class SensorDenormComp:public sensor::GenericFilterComputation<int,double>{
  sensor::Calib<int>& limit;
  
  void compute(int* r,const double *s,const HwIdList& what){    
    for (size_t i=0;i<what.ids.size();++i){
      int idx=what.ids[i];
      r[idx]=limit[idx].minv+s[idx]*(limit[idx].maxv-limit[idx].minv);
    }
  }

  void getRequired(HwIdList& req,const HwIdList& what){
    req=what;
  }
  
  void getGenerated(HwIdList& gen,const HwIdList& id){
    gen=id;
  }

public:
  SensorDenormComp(sensor::Calib<int>& plimit):limit(plimit){    
  }
  
};


}//ns he


#endif
