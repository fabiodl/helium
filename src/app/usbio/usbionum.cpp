#include <helium/os/time.h>
#include <helium/hiddevice/hidUSBIO.h>

using namespace helium;


int main(int argc,char** argv){
  HidUSBIO io(4);
  io.setDirection(avrPort::B,0x3F);//6 bit
  io.setDirection(avrPort::C,0xFF);//1 bit
  std::stringstream ss;
  ss<<argv[1];
  int val;
  readAutoHex(val,ss);
  io.setOutput(avrPort::B,val&0x3F);
  io.setOutput(avrPort::C,val&0x40?0x04:0x00);
  milliSleep(10);
}
