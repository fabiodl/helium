#ifndef APP_M3READER
#define APP_M3READER
#include <helium/os/usbHid.h>
#include <fstream>

namespace m3reader{

enum format{
  REQUEST,
  BANKADDR,
  BANKVALUE,
  CRC,
  CMDLENGTH
};

}



class M3Reader:public helium::UsbHid{
public:
  static const unsigned int BANKSIZE=0x4000;
  static const unsigned int CHUNKSIZE=8;
private:  
  helium::UsbMessage<m3reader::CMDLENGTH> cmdMsg;
  helium::UsbMessage<BANKSIZE> dataInMsg;
  helium::UsbMessage<CHUNKSIZE> ackMsg,dataOutMsg;
public:
  enum Mapper{NOMAPPER,SEGA_ROM,SEGA_RAM};

  M3Reader();

  void read(std::ofstream& f,unsigned char bank,Mapper mapper=SEGA_ROM);
  void erase();
  void eraseSector(unsigned char high);
  void write(unsigned char bank,const unsigned char* outMsg,Mapper maper=SEGA_ROM);

  
};


#endif
