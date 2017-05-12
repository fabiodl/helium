#include <map>
#include <helium/os/usbHid.h>
#include <fstream>
#include <stdlib.h>
#include "commandParser.h"


void usbSend(const RawCommand& cmd){  
  helium::UsbMessage<helium::array::RESIZABLE> msg(cmd.packet.size());
  for (size_t i=0;i<cmd.packet.size();i++){
    msg[i]=cmd.packet[i];
  }
  std::cout<<"prepared message "<<msg<<std::endl;
  //helium::UsbHid hid(0x16c0,0x05dc,"fabiodl@gmail.com","dusb");
  //hid.sendAll(msg);    
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

  try{
    usbSend(cmd.parse(argc-1,argv+1));
  }catch(std::invalid_argument& e){
    std::cout<<e.what()<<std::endl;
  }
  

  

}
