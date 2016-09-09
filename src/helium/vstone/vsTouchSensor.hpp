#include <helium/vstone/vsByte.h>
namespace helium{

template<typename Param, void (*updaterf)(Param,int,HwState)>
void VStouchSensor::isWorking(Param target,const SensorToken::ReadData &r){
    try{
      //std::cout<<"maxid="<<r.maxId<<",mind="<<r.minId<<std::endl;
      s.recv(r.bid,M_VIN0+r.minId,buffer,2*(r.maxId-r.minId+1));	
      for (int si=r.minId;si<=r.maxId;si++){
	int g=r.gid[si];
	//std::cout<<"checking sensor "<<g<<std::endl;
	if (g>=0){
	  int v;	    
	  from14(v,&buffer[(si-r.minId)*2]);	  
	  (*updaterf)(target,g,v==0?FAULTYVALUEHWSTATE:WORKINGHWSTATE);
	}//g>=0
      }//for si	
    }catch(exc::Exception& ){
      //std::cout<<"caugth!"<<std::endl;
      for (int si=r.minId;si<=r.maxId;si++){
	int g=r.gid[si];
	if (g>=0){
	  (*updaterf)(target,g,UNREACHABLEHWSTATE);
	}
      }//for si
    }//catch
  }

}//helium
