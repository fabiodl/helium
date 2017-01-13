#include "megankro.h"
#include <iostream>

using namespace helium;

MegaNkro::MegaNkro():
  UsbHid(0x16c0,0x05dc,"fabiodl@gmail.com","MegaNkro"){
}


void MegaNkro::set(uint8_t* keys){
  helium::UsbMessage<NUMKEYS>  outmsg;
  outmsg=keys;
  sendAll(outmsg);
  std::cout<<"keys set"<<std::endl;
}
