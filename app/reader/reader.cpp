#include <helium/hiddevice/hidUSBIO.h>
#include <helium/os/time.h>
#include <helium/math/bit.h>
#include <fstream>

using namespace helium;


static const avrPort::Name DATA=avrPort::A;
static const avrPort::Name ADDRL=avrPort::B;
static const avrPort::Name ADDRH=avrPort::C;

static const avrPort::Name CONTROL=avrPort::D;


static const unsigned char _BV(int x){
  return 1<<x;
}




class M3Reader:public HidUSBIO{

  static const unsigned int BANKSIZE=0x4000;
  static const unsigned int BANKSELECTOR=0xFFFF;
  static const unsigned int MAPPEDBANKSTART=0x8000;

  /*  static const unsigned int MAPDESTINATION=;
  static const unsigned int MAPPEDSTART=;
  static const unsigned int BANKSIZE=;
  */




  enum Control{
    RD,WR,EXM1,MREQ,CARD,DSRAM,CON,M1
  };

  static const int CONTROLHIGH=(1<<RD)|(1<<WR)|(1<<EXM1)|(1<<MREQ);



public:

  unsigned char readLocation(unsigned int addr){
    Array<unsigned char,4> newOut;
    newOut[ADDRL]=addr&0xFF;
    newOut[ADDRH]=(addr>>8)&0xFF;
    newOut[CONTROL]=CONTROLHIGH;    
    setBit(newOut[CONTROL],EXM1,(addr>=0x8000&&addr<0xC0000)?0:1);
    setBit(newOut[CONTROL],RD,(addr<0xC0000)?0:1);
    //setBit(newOut[CONTROL],MREQ,0);
    newOut[DATA]=0xFF;

    setOutput(newOut);

    setOutput(CONTROL,MREQ,0);//request the data

    //milliSleep(1);

    unsigned char data=read(DATA),newData;

    int count=0;
    do{
	newData=read(DATA);	
      if (newData==data){
	count++;
      }else{
	count=0;
	data=newData;
      }
    }while (count<3);
    return data;
  }


  void write(unsigned int addr,unsigned char data){    
    setOutput(CONTROL,CONTROLHIGH);
    setDirection(DATA,avrPort::ALLOUT);
    setOutput(CONTROL,MREQ,0);
    Array<unsigned char,4> newOut;
    newOut[ADDRL]=addr&0xFF;
    newOut[ADDRH]=(addr>>8)&0xFF;
    newOut[CONTROL]=CONTROLHIGH;    

    newOut[DATA]=data;
    setOutput(newOut);    
    setOutput(CONTROL,WR,0);

    setOutput(CONTROL,CONTROLHIGH);
    setDirection(DATA,avrPort::ALLIN);    
  }



  M3Reader():HidUSBIO(4){
    setDirection(DATA,avrPort::ALLIN);
    setDirection(ADDRL,avrPort::ALLOUT);
    setDirection(ADDRH,avrPort::ALLOUT);
    setOutput(CONTROL,CONTROLHIGH);//inverted logic, disable all
    setDirection(CONTROL,CONTROLHIGH);
    setPullup(DATA,0xFF);
  }

  void directDump(unsigned char* dst,unsigned int size){
    for (unsigned int i=0;i<size;){
      std::cout<<"\r i="<<i<<std::flush;
      try{
	dst[i]=readLocation(i);
	i++;
      }catch(exc::Exception&){
      }
      
    }
  }


  void bankedDump(unsigned char* dst,unsigned int size){
    write(0xFFFC,0);
    write(0xFFFD,0);
    write(0xFFFE,0);
    write(0xFFFF,0);

    Time t=getSystemMillis();
    for (unsigned char bank=0;bank<(unsigned char)(size/BANKSIZE);bank++){
      write(BANKSELECTOR,bank);
      for (unsigned int addr=0;addr<BANKSIZE;addr++){
	int realAddr=bank*BANKSIZE+addr;
	std::cout<<"\r"<<std::hex<<(int)bank<<" 0x"<<realAddr<<" " <<((double)getElapsedMillis(t))/realAddr<< std::flush;
	dst[realAddr]=readLocation(MAPPEDBANKSTART+addr);
      }    
    }

  }



};





int main(){
 M3Reader m3r;

 //static const int size=0x1000; 
static const int size=0x80000;

 Array<unsigned char> content(size);

 m3r.bankedDump(content,size);

 //m3r.directDump(content,size);
 
 std::ofstream outfile ("dump.txt",std::ofstream::binary);
 outfile.write((char*)content.data,content.size());
 outfile.close();
 return 0;


}
