#include <helium/system/cellClient.h>
#include <helium/system/valueProxy.h>
#include <helium/system/cellDescription.h>
#include <helium/system/remoteCellImporter.h>
#include <helium/signal/staticConnReference.h>
#include <helium/robot/sensorInterface.h>

using namespace helium;


class Printer{

public:  
  void printValue(int d){
    std::cout<<d<<std::endl;
  }

  void printAll(const SensorValList<double>& v){
    std::cout<<v<<std::endl;
  }

};

int main(int argc,char** argv){ 
  CellClient cl;
  cl.connect("localhost",CELLPORT);
  Memory mem;  
  

  ValueProxy<int> sensVal(0);
  dexport(mem,sensVal,IDPath("robot.sensors.sensor.1.value","."));

  Void v;
  ValueProxy<SensorGroupCellSignature<double> > allSensors(v);
  dexport(mem,allSensors,"normSens");

  
  Printer printer;

  StaticConnReference<int,Printer,int,&Printer::printValue> 
    printConn(&printer);

  
  StaticConnReference<const SensorValList<double>&,Printer,const SensorValList<double>&,&Printer::printAll> 
    printAllConn(&printer);


  RemoteCellImporter rci(mem,cl);   
  rci.connectProxies();

  //sensVal.connect(printConn);
  allSensors.connect(printAllConn);

  int x;
  std::cin>>x;

  
}
