#include <helium/os/usbHid.h>

using namespace helium;


int main(int argc,char** argv){

  if (argc<2){
    std::cout<<"Usage "<<argv[0]<<" <device name> sequence of hex"<<std::endl;
    return 0;
  }
 

  UsbMessage<array::RESIZABLE> msg(argc-2);

  for (int i=2;i<argc;i++){
    msg[i-2]=static_cast<uint8_t>(strtol(argv[i],NULL,16));
  }

  UsbHid usbhid(0x16c0,0x05dc,"dallalibera.tk",argv[1]);
  usbhid.sendAll(msg);
  std::cout<<"sent "<<msg.payloadSize()<<" bytes"<<std::endl;
}
