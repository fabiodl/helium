
#include <helium/hiddevice/hidUSBIO.h>
#include <helium/os/time.h>
#include <helium/math/bit.h>
#include <fstream>

using namespace helium;
using namespace helium::avrPort;



static const avrPort::Name DATA=avrPort::A;
static const avrPort::Name ADDRL=avrPort::B;
static const avrPort::Name ADDRH=avrPort::C;

static const avrPort::Name CONTROL=avrPort::D;


enum Control{
  RD,WR,EXM1,MREQ,CARD,DSRAM,CON,M1
};

static const int CONTROLHIGH=(1<<RD)|(1<<WR)|(1<<EXM1)|(1<<MREQ);

static const int  ROMRD=DSRAM;
static const int ROMWR=CON;
static const int ROMCE=M1;
static const int ROMHIGH=(1<<ROMRD)|(1<<ROMWR)|(1<<ROMCE);


class Flash:public HidUSBIO{
  std::string x;
  
public:  

  void pause(){
    //std::cin>>x;
  }
  Flash():HidUSBIO(4){
    setDirection(DATA,ALLIN); 
    setDirection(ADDRH,ALLOUT); 
    setDirection(ADDRL,ALLOUT); 
    setDirection(CONTROL,ROMHIGH);
    setOutput(CONTROL,ROMHIGH);
    setOutput(ADDRL,0);
    setOutput(ADDRH,0);
    std::cout<<"addr=0"<<std::endl;
    pause();
  }


  void write(unsigned char h,unsigned char l,unsigned char d){
    unsigned char p[4];
    setDirection(DATA,ALLOUT);
    p[DATA]=d;
    p[ADDRL]=l;
    p[ADDRH]=h;
    p[CONTROL]=ROMHIGH;
    setOutput(p);
    std::cout<<"addr= "<<std::hex<<(((int)h)<<8|l)<<" data="<<(int)d<<std::endl;
    pause();
    std::cout<<"/CE /WR"<<std::endl;
    setOutput(CONTROL,ROMHIGH&~((1<<ROMCE)|(1<<ROMWR)));
    pause();
    std::cout<<"CE WR"<<std::endl;
    setOutput(CONTROL,ROMHIGH);
    setDirection(DATA,ALLIN);
  }


  void initWrite(){
    write(0x05,0x55,0xAA);
    write(0x02,0xAA,0x55);
    write(0x05,0x55,0xA0);
  }


  
  int read(unsigned char h,unsigned char l){
    pause();
    //std::cout<<"/CE /RD"<<std::endl;
    setOutput(ADDRH,h);
    setOutput(ADDRL,l);

    
    setOutput(CONTROL,ROMHIGH&~((1<<ROMCE)|(1<<ROMRD)));
    pause();
    int r=HidUSBIO::read(DATA);
    setOutput(CONTROL,ROMHIGH);
    return r;
  }
};


int main(){
  Flash f;
  /*f.initWrite();
  f.write(0x00,0x00,0xFA);
  f.write(0x00,0x01,0xB1);*/

  for (int i=0;i<10;i++){
    unsigned char c=f.read(i,i);
    std::cout<<"rom["<<i<<"]="<<std::hex<<(int)c<<std::endl;
  }
}
