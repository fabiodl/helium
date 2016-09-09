#include <helium/os/usbHid.h>
#include <helium/math/crc.h>
#include <fstream>
#include <helium/os/time.h>
#include <helium/vector/operations.h>
#include <helium/util/printHex.h>
using namespace helium;

enum cmd{
  READBANK=5,  
  WRITEBANK,
  ERASEROM
};

enum format{
  REQUEST,
  BANKADDR,
  BANKVALUE,
  CRC,
  CMDLENGTH
};

namespace helium{
namespace exc{
  class WriteException{
  public:
  };
}
}
class M3Reader:public UsbHid{
public:
  static const unsigned int BANKSIZE=0x4000;
  static const unsigned int CHUNKSIZE=8;
private:  
  UsbMessage<CMDLENGTH> cmdMsg;
  UsbMessage<BANKSIZE> dataInMsg;
  UsbMessage<CHUNKSIZE> ackMsg,dataOutMsg;
  std::ofstream f;
public:
  M3Reader():
    UsbHid(0x16c0,0x05dc,"fabiodl@gmail.com","MRKWR"),
    f("confirm.dump",std::ios::binary|std::ios::out)
  {
    
  }

  void read(unsigned char bank){
    cmdMsg[REQUEST]=READBANK;
    cmdMsg[BANKADDR]=0xFF;
    cmdMsg[BANKVALUE]=bank;
    cmdMsg[CRC]=crc8maxim::getCrc(cmdMsg,3);
    sendAll(cmdMsg);
    recvAll(dataInMsg);
    f.write((char*)(unsigned char*)dataInMsg,BANKSIZE);
  }
  
  
  void erase(){
    cmdMsg[REQUEST]=ERASEROM;
    cmdMsg[BANKADDR]=0;
    cmdMsg[BANKVALUE]=0;
    cmdMsg[CRC]=crc8maxim::getCrc(cmdMsg,3);
    sendAll(cmdMsg);
    recvAll(ackMsg);
    std::cout<<"received erase ack ";
    printHex(ackMsg,CHUNKSIZE);
    std::cout<<std::endl;
  }

  void write(unsigned char bank,const unsigned char* outMsg){
    cmdMsg[REQUEST]=WRITEBANK;
    cmdMsg[BANKADDR]=0xFF;
    cmdMsg[BANKVALUE]=bank;
    cmdMsg[CRC]=crc8maxim::getCrc(cmdMsg,3);
    sendAll(cmdMsg);
    recvAll(ackMsg);

    std::cout<<"received ack ";
    printHex(ackMsg,CHUNKSIZE);
    std::cout<<std::endl;
    if (ackMsg[0]!=cmdMsg[0]){    
      throw exc::WriteException();

    }
    for (size_t i=1;i<CHUNKSIZE;i++){
      if (ackMsg[i]!=0xFF) throw exc::WriteException();
    }

    for (size_t i=0;i<BANKSIZE/CHUNKSIZE;i++){
      //std::cout<<"."<<std::flush;
      std::cout<<i*100/(BANKSIZE/CHUNKSIZE)<<"%\r"<<std::flush;
      assignn<CHUNKSIZE>(dataOutMsg,outMsg);
      outMsg+=CHUNKSIZE;
      sendAll(dataOutMsg);
      //std::cout<<"wait ack "<<std::flush;
      recvAll(ackMsg);
      //std::cout<<"ok"<<std::endl;
      
      for (size_t j=0;j<CHUNKSIZE;j++){
	if (dataOutMsg[j]!=ackMsg[j]){
	 
	  std::cout<<"sending  ";
	  printHex(dataOutMsg,CHUNKSIZE);
	  std::cout<<std::endl<<"received ";
	  printHex(ackMsg,CHUNKSIZE);
	  std::cout<<std::endl;

	  throw exc::WriteException();
	}
      }

    }
    std::cout<<std::endl<<std::flush;
  }

};


int main(int argc,char** argv){
 

  M3Reader m3;
  const char* src="alexReal.sms";
  if (argc>1){
    src=argv[1];
  }

  std::ifstream in(src,std::ios::binary|std::ios::in);

  std::cout<<"erasing.."<<std::endl;
  m3.erase();
  std::cout<<"ok"<<std::endl;
  

  
  Array<unsigned char,M3Reader::BANKSIZE> bankData;

  std::cout<<"writing "<<src<<std::endl;
  int bankNum=0;
  while(!in.eof()){
    in.read((char*)bankData.data,M3Reader::BANKSIZE);
    if (in){
      std::cout<<"writing bank"<<bankNum<<std::endl;
      m3.write(bankNum,bankData.data);
      bankNum++;
    }    
  }


  std::cout<<"ok"<<std::endl<<"reading"<<std::endl;
  
  for (int i=0;i<bankNum;i++){
    std::cout<<"bank "<<i<<std::endl;
    m3.read(i);
  }
  std::cout<<"read completed"<<std::endl;

  /*Time t=getSystemMillis();
  for (int i=0;i<32;i++){
    std::cout<<"Bank "<<i<<std::endl;
    m3.read(i);
  }
  std::cout<<getElapsedMillis(t)<<std::endl;*/
  
  
}
