#include <map>
#include <helium/os/usbHid.h>
#include <fstream>
#include <stdlib.h>
#include "commandParser.h"
#include "keyMapping.h"
#include "config.h"


enum LocalCmd{
  LCMD_LISTKEYS,LCMD_LISTPADKEYS,LCMD_LISTIOKEYS
};



void usbSend(const RawCommand& cmd){  
  helium::UsbMessage<helium::array::RESIZABLE> msg(cmd.packet.size());
  msg.resize(sizeof(Config)+2);
  for (size_t i=0;i<cmd.packet.size();i++){
    msg[i]=cmd.packet[i];
  }
  std::cout<<"prepared message "<<msg<<std::endl;
  helium::UsbHid hid(0x16c0,0x05dc,"fabiodl@gmail.com","dusb");
  hid.sendAll(msg);    
}


  
int main(int argc,char** argv){

  NoParamCommand usbReset(CMD_USBRESET),applycfgIo(CMD_APPLYCFG_IO),loadCfg(CMD_LOADCFG),storeCfg(CMD_STORECFG);  
  WriteCfgBytesCommand writecfgBytes;
  WriteCfgBitsCommand writecfgBits;

  SetCfgKeysCommand setcfgKeys;
  SetCfgAutofireCommand setcfgAutofire;
  SetCfgIoCommand setcfgIo;

  SetIoPortCommand setioPort;
  SetIoKeyvalCommand setioKeyval;
  
  DictionaryCommand applycfg;
  DictionaryCommand writecfg;
  DictionaryCommand setcfg;
  DictionaryCommand setio;

  
  DictionaryCommand cmd;

  LocalCommand listKeys(LCMD_LISTKEYS),listPadkeys(LCMD_LISTPADKEYS),listIokeys(LCMD_LISTIOKEYS);

  DictionaryCommand list;
  
  applycfg.commands["io"]=&applycfgIo;

  writecfg.commands["bytes"]=&writecfgBytes;
  writecfg.commands["bits"]=&writecfgBits;

  setcfg.commands["keys"]=&setcfgKeys;
  setcfg.commands["autofire"]=&setcfgAutofire;
  setcfg.commands["io"]=&setcfgIo;

  setio.commands["port"]=&setioPort;
  setio.commands["keys"]=&setioKeyval;
  
  cmd.commands["reset"]=&usbReset;
  cmd.commands["loadcfg"]=&loadCfg;
  cmd.commands["storecfg"]=&storeCfg;  
  cmd.commands["applycfg"]=&applycfg;
  cmd.commands["writecfg"]=&writecfg;
  cmd.commands["setcfg"]=&setcfg;
  cmd.commands["setio"]=&setio;

  list.commands["keys"]=&listKeys;
  list.commands["pad"]=&listPadkeys;
  list.commands["io"]=&listIokeys;

  cmd.commands["list"]=&list;
  try{
    const RawCommand rcmd(cmd.parse(argc-1,argv+1));
    if (rcmd.packet[0]==LocalCommand::ID){
      switch(rcmd.packet[1]){
      case LCMD_LISTKEYS:
        ::listKeys();
        break;
      case LCMD_LISTPADKEYS:
        ::listPadKeys();
        break;
      case LCMD_LISTIOKEYS:
        ::listExtraKeys();
        break;

      }
    }else{
      usbSend(rcmd);
    }
  }catch(std::invalid_argument& e){
    std::cout<<e.what()<<std::endl;
  }catch(helium::exc::Exception& e){
    std::cout<<e.what()<<std::endl;
  }
  

  

}
