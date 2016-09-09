#include <helium/mocap/mocapClient.h>
#include <helium/os/time.h>

using namespace helium;




class YokoyamaClient:public Callback<const WholeMarkerSet&>{
  Time previousTime;

  void operator()(const WholeMarkerSet& m){
    Time t=getSystemMillis();
    if (t-previousTime<100) return;
    previousTime=t;
    std::cout<<"called"<<std::endl;
    for (size_t i=0;i<m.bodies.size();i++){
      std::cout<<"body "<<i<<"has "<<m.bodies[i].size()<<" markers"<<std::endl;
      for (size_t j=0;j<m.bodies[i].size();j++){
	std::cout<<" marker"<< j<<" .x ="<<m.bodies[i][j][0]<<std::endl;
	std::cout<<" marker"<< j<<" .y ="<<m.bodies[i][j][1]<<std::endl;
	std::cout<<" marker"<< j<<" .z ="<<m.bodies[i][j][2]<<std::endl;
      }//for j     
    }//for i

       
  };
public:
  YokoyamaClient():previousTime(0){
  }

};



int main(){
  MocapClient mocap;
  mocap.connect("127.0.0.1",MOCAPCELLPORT);
  YokoyamaClient yc;
  mocap.whole.connect(&yc);
 

  int x;
  std::cout<<"connected"<<std::endl;
    std::cin>>x;
}
