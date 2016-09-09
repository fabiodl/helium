#ifndef HE_ACCELEROMETERDENORM
#define HE_ACCELEROMETERDENORM

#include <helium/robot/genericFilter.h>
#include <helium/robot/gyroStructures.h>
namespace helium{

class AccelerometerDenormComp:public sensor::GenericFilterComputation<int,double>{
  const GyroCalib& g;


  void compute(int* r,const double *m,const HwIdList& what){  
    static const double magnitude=300;        
    /*std::cout<<"converting";
    print(m)<<std::endl<<" with ";
    std::cout<<g.xratio<<" "<<g.xoffset<<std::endl;
    std::cout<<g.yratio<<" "<<g.yoffset<<std::endl;
    std::cout<<" "<<g.zoffset<<std::endl;
    std::cout<<" to ";*/
    r[0]=-round(magnitude*m[8]*g.xratio-g.xoffset);
    r[1]=-round(magnitude*m[9]*g.yratio-g.yoffset);
    r[2]=-round(magnitude*m[10]*1-g.zoffset);
    r[3]=r[4]=0;  
    //print(r,5)<<std::endl;
   }

   void getRequired(HwIdList& req,const HwIdList& what){
     if (what.ids.size()){
       req.ids.resize(16);
       setConsecutive(req.ids);
     }else{
       req.ids.clear();
     }
   }

  void getGenerated(HwIdList& gen,const HwIdList& id){
    gen.ids.resize(5);
    setConsecutive(gen.ids);
  }

public:
  AccelerometerDenormComp(const GyroCalib& pg):g(pg){
  }

};

}

#endif
