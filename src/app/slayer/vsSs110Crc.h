/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef XPLAYER_VSSS110CRC
#define XPLAYER_VSSS110CRC


#include <helium/vstone/vsCrcTouchSensor.h>
#include <helium/vstone/vsCrcSerial.h>
#include "sensorPollerInterface.h"

namespace helium{

  class VsSs110Crc{

  public:
    struct Config{
      static const int TIMEOUT=30;
      static const int BAUDRATE=115200;
      std::string serialDevice;
      vsTs::RawIdMap rid;
      vsTs::RawIdMap ledid;
      
      Config(Memory& mem,const IDPath& confPath){	
	dimport(*this,mem,confPath);
      }
    }; 

  private:
      Config cfg;
    
    Serial srl;
    VsCrcSerial<Serial> vs;
    std::auto_ptr<vsTs::LocklessLedBurstSetter> ledBurst;
    VsCrcTouchSensor<VsCrcSerial<Serial> > vts;
    SensorPollerInterface<int> polling;

  public:
    VsSs110Crc(Memory& mem,const HwComponentName& name,const IDPath& confPath,const IDPath& servicePath):
    
      cfg(mem,confPath),
      srl(cfg.serialDevice.c_str(),Config::BAUDRATE,Config::TIMEOUT),
      vs(srl),
      ledBurst(cfg.ledid.size()==0?NULL:
	       new vsTs::LocklessLedBurstSetter(polling.ss,cfg.ledid.size())		 
	       ),
      vts(vs,cfg.rid,cfg.ledid.size()==0?NULL:&cfg.ledid,ledBurst.get()),
      polling(vts){
      dexport(mem,polling,servicePath);
    }

    bool isTemporary(){
      return false;
    }

  };


    
    
  template<> 
  class DefDescr<VsSs110Crc::Config>:public Description<VsSs110Crc::Config>{
  public:
    DefDescr(VsSs110Crc::Config& p):Description<VsSs110Crc::Config>(p){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(scalarMapping(obj.serialDevice,IDPath("bus.device",".")));
      members.push_back(scalarMapping(d(obj.rid),""));
      members.push_back(scalarMapping(cd<LedDescr>(obj.ledid),IDPath("leds",".") ));
    }
  };



}

#endif
