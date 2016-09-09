/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/hiddevice/hidBusyBitTouchSensor.h>
#include <helium/robot/cachedSensor.h>

namespace helium{
class BusyBitTouchSensor{

  HidBusyBitTouchSensor bbsensor;

  typedef   CachedSensor<int,sensorProcessing::CHANGES_CELL,sensorProcessing::CHANGES_CELL> ExpandedSensor;
  ExpandedSensor cachedSensor;

  class DataAdapter:public Callback<const unsigned char*>,public Callback<exc::Exception*> {

  public:
    DataAdapter(ExpandedSensor& pcs,int n):cs(pcs),expandData(n),full(n){            full.fillConsecutive();
    }
    
  private:

    ExpandedSensor& cs;
    Array<int> expandData;
    HwIdList full;

    void operator()(const unsigned char* c){
      for (size_t i=0;i<cs.size();i++){
	expandData[i]=c[i/8]&(1<<(i%8) )?1:0;	
      }
      //printHex(c,1);
      //std::cout<<"Expanding "; 
      //print(expandData);
      //std::cout<<std::endl;
      cs.update(expandData.data,full);
    }

    void operator()(exc::Exception* e){
      cs.handle(full,*e,expandData.data);
    }



  } dataAdapter;
  
public:
  BusyBitTouchSensor(helium::Memory& mem,const HwComponentName& name,const helium::IDPath& confPath,const helium::IDPath& servicePath):
    bbsensor(0x1D),
    cachedSensor(bbsensor.size()),
    dataAdapter(cachedSensor,bbsensor.size())
  {    

    dexport(mem,cachedSensor,servicePath);
    bbsensor.onSensorUpdate.connect(&dataAdapter);
    bbsensor.onError.connect(&dataAdapter);
  }
  
  bool isTemporary(){
    return false;
  }
  
  
};
}
