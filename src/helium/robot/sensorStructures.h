#ifndef HE_SENSORSTRUCTURES
#define HE_SENSORSTRUCTURES

#include <helium/vector/operations.h>
#include <helium/robot/sensorInterface.h>
#include <helium/system/cellDescription.h>

namespace helium{
  
  namespace sensor{

    template<typename T>
    class SensorCalib{
    public:
      T minv;
      T maxv;      
    };
    
    template<typename T> class Calib:public Array<SensorCalib<T> >{
   
     
      
    };//Calib

    inline ReadType getReadType(int x){
      return (ReadType)(x);
    }


    template<typename T,typename N=double>
    class LinearNormalizer{
      Calib<T>& calib;
      
    public:      
      N normVal(T& val,int i){
	return ((N)(val-calib[i].minv))/(calib[i].maxv-calib[i].minv);
      }

      N clampedNormVal(T& val,int i){
	return limit(0,1,normVal(val,i));
      }
 
      LinearNormalizer(Calib<T>& pcalib):calib(pcalib){
      }
    }; 


  }//sensor

  template<typename T>
  class DefDescr<sensor::SensorCalib<T> >:public Description<sensor::SensorCalib<T> >{
  public:
    DefDescr(sensor::SensorCalib<T>& c):Description<sensor::SensorCalib<T> >(c){
    }
    
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(scalarMapping(this->obj.minv,"min"));
      members.push_back(scalarMapping(this->obj.maxv,"max"));
    }
    
  };
       

  template<typename T>
  class DefDescr<sensor::Calib<T> >:public Description<sensor::Calib<T> >{
  public:
    DefDescr(sensor::Calib<T>& c):Description<sensor::Calib<T> >(c){
    }
    
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(vectMapping(vd(this->obj),"sensor","","num"));
    }
    
  };


}//helium



#endif
