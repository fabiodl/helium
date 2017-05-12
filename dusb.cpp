#include "dusb.h"
#include <iostream>

using namespace helium;

Dusb::Dusb():
  Dubsb(0x16c0,0x05dc,"fabiodl@gmail.com","dusb"){
}

/*
void Dusb::setKeyConfig(uint8_t* keys){
  helium::UsbMessage<NUMKEYS>  outmsg;
  outmsg=keys;
  sendAll(outmsg);
  std::cout<<"keys set"<<std::endl;
  }*/

void Dusb::sendCommand(uint8_t cmdId,void* param,int paramLength){
  helium::UsbMessage<array::RESIZABLE> outmsg(paramLength+1);
  outmsg[i]=cmdId;
  for (int i=0;i<paramLength;i++){
    outmsg[i+1]=param[i];
  }
  sendAll(outmsg);
}
