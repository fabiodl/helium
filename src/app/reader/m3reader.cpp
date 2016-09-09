#include "m3reader.h"

#include <helium/math/crc.h>

#include <helium/os/time.h>
#include <helium/vector/operations.h>
#include <helium/util/printHex.h>


namespace helium{
namespace exc{
  class WriteException{
  public:
  };
}
}


using namespace helium;
using namespace m3reader;

enum cmd{
  READBANK=5,  
  WRITEBANK,
  ERASEROM
};

enum AckCode {ACK_WRONGCRC=0xFF,ACK_UNKNOWNCOMMAND=0xFE,ACK_WRITEBANK=WRITEBANK,ACK_WRITE_SPACEFREE=0x40+WRITEBANK,ACK_WRITECOMPLETED=0x80+WRITEBANK};


M3Reader::M3Reader():
  UsbHid(0x16c0,0x05dc,"fabiodl@gmail.com","MRKWR")
{
  
}



void M3Reader::read(std::ofstream& f,unsigned char bank,M3Reader::Mapper mapper){
  cmdMsg[REQUEST]=READBANK;

  switch(mapper){
  case NOMAPPER:
    cmdMsg[BANKADDR]=0x00;
    cmdMsg[BANKVALUE]=bank*0x40;
    break;
  case SEGA_ROM:
    cmdMsg[BANKADDR]=0xFF;
    cmdMsg[BANKVALUE]=bank;
    break;
  case SEGA_RAM:
    cmdMsg[BANKADDR]=0xFC;
    cmdMsg[BANKVALUE]=0x08+(bank?0x04:0);
    //0b00001000+(bank?0b100:0);
    break;
  }
  cmdMsg[CRC]=crc8maxim::getCrc(cmdMsg,3);
  sendAll(cmdMsg);
  recvAll(dataInMsg);
  f.write((char*)(unsigned char*)dataInMsg,BANKSIZE);
}

void M3Reader::erase(){
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


void M3Reader::eraseSector(unsigned char high){
  cmdMsg[REQUEST]=ERASEROM;
  cmdMsg[BANKADDR]=high;
  cmdMsg[BANKVALUE]=1;
  cmdMsg[CRC]=crc8maxim::getCrc(cmdMsg,3);
  sendAll(cmdMsg);
  recvAll(ackMsg);
  std::cout<<"received erase sector ";
  printHex(ackMsg,CHUNKSIZE);
  std::cout<<std::endl;
}


void M3Reader::write(unsigned char bank,const unsigned char* outMsg,M3Reader::Mapper mapper){
  cmdMsg[REQUEST]=WRITEBANK;
  switch(mapper){
  case NOMAPPER:
    cmdMsg[BANKADDR]=0x00;
    cmdMsg[BANKVALUE]=bank*0x40;
    break;
  case SEGA_ROM:
    cmdMsg[BANKADDR]=0xFF;
    cmdMsg[BANKVALUE]=bank;
    break;
  case SEGA_RAM:
    cmdMsg[BANKADDR]=0xFC;
    cmdMsg[BANKVALUE]=0x88+(bank?0x04:0);

      //0b10001000+(bank?0b100:0);
    break;
  }

  cmdMsg[CRC]=crc8maxim::getCrc(cmdMsg,3);
  sendAll(cmdMsg);
  std::cout<<"sending cmd";
  printHex(cmdMsg,CMDLENGTH);
  std::cout<<std::endl;
  recvAll(ackMsg);
  
  std::cout<<"received ack ";
  printHex(ackMsg,CHUNKSIZE);
  std::cout<<std::endl;
  if (ackMsg[0]!=ACK_WRITEBANK){    
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
    //std::cout<<"sending data ";
    //printHex(dataOutMsg,CHUNKSIZE);
    //std::cout<<std::endl;
    //std::cout<<"wait ack "<<i<<std::flush;
    recvAll(ackMsg);
    //std::cout<<"ok ";
    //printHex(ackMsg,CHUNKSIZE);
    //std::cout<<std::endl;
    
    if ( ackMsg[0]!=ACK_WRITE_SPACEFREE){
      std::cerr<<"Wrong ack received while waiting the started writing ack"<<std::endl;//this in an error on the PREVIOUS data
      printHex(ackMsg,CHUNKSIZE);
      std::cout<<std::endl;
      throw exc::WriteException(); 
    }	

    if (ackMsg[1]){
      std::cerr<<"Reported write error"<<std::endl;//this in an error on the PREVIOUS data
      throw exc::WriteException();
    }	
      
  }//for i
  //std::cout<<"wait bank final ack "<<std::flush;
    recvAll(ackMsg);
    if ( ackMsg[0]!=ACK_WRITECOMPLETED){
      std::cerr<<"Wrong ack received while waiting the complition of the last data"<<std::endl;//this in an error on the PREVIOUS data
       printHex(ackMsg,CHUNKSIZE);
  std::cout<<std::endl;
  throw exc::WriteException(); 
    }	

    if (ackMsg[1]){
      std::cerr<<"Reported write error"<<std::endl;//this in an error on the PREVIOUS data
      throw exc::WriteException();
    }	
    //  milliSleep(1000);
}//write


