#include <helium/math/crc.h>
#include <helium/math/bit.h>
#include <helium/hiddevice/hidUSBIO.h>
#include <helium/hiddevice/crcException.h>
#include <helium/vector/operations.h>
#include <helium/util/printHex.h>


using namespace helium;






void HidUSBIO::sendPacket(Cmd cmd,const unsigned char* data){
  outmsg[0]=cmd;
  for (size_t i=0;i<ddr.size();++i){
    outmsg[1+i]=data[i];
  }
  outmsg[outmsg.payloadSize()-1]=crc8maxim::getCrc(outmsg,outmsg.payloadSize()-1);
  sendAll(outmsg);
  //std::cout<<"sending packet "<<asHexString(outmsg,outmsg.payloadSize())<<std::endl;

  if (cmd!=CMDREAD){
    //std::cout<<"waiting ack"<<std::endl;
    ackWaiter.waitData();
    //std::cout<<"ack rec"<<std::endl;
    if (differs(ackWaiter.data,(unsigned char*)outmsg)){
      throw exc::InvalidOperation("Wrong ack received");
    }else{
      //std::cout<<"ok"<<std::endl;
    }
  }
}



HidUSBIO::HidUSBIO(int portNum):
  UsbHid(0x16c0,0x05dc,"fabiodl@gmail.com","USBIO"),
  running(true),
  ddr(portNum),updatemask(portNum),port(portNum),toRead(portNum),
  outmsg(portNum+2),inmsg(portNum+2),
  readWaiter(portNum),updateWaiter(portNum),ackWaiter(portNum+2)
{
  zero(ddr);
  zero(updatemask);
  zero(port);
  setAll(toRead,0xFF);
  onRead.connect(readWaiter);
  onUpdate.connect(updateWaiter);  
  onAck.connect(ackWaiter);
  start();
}


void HidUSBIO::run(){

  while(running){
    try{
      //std::cout<<"waiting receiving.."<<std::endl;
      recvAll(inmsg);      
      //printHex(inmsg,inmsg.payloadSize());
      //std::cout<< "is inmgs"<<std::endl;
      if (crc8maxim::getCrc(inmsg,inmsg.payloadSize())){
	throw exc::CRCException(inmsg,inmsg.payloadSize());
      }      
      switch(inmsg[0]){
      case DATAUPDATE:
	onUpdate(inmsg+1);
	break;
      case CMDREAD:
	onRead(inmsg+1);
	break;
      case INVALIDCMD:
      default:
	onAck(inmsg);	
      }
    }catch(exc::Exception& e){
      std::cout<<"exception thrown"<<std::endl;
      onRead.handle(e);
      onUpdate.handle(e);
    }
  }
  //  std::cout<<"thread exiting"<<std::endl;
}

HidUSBIO::~HidUSBIO(){  
  running=false;
 
  sendPacket(INVALIDCMD,updatemask.data);//send an invalid command just to return
  UsbHid::close();
  join();
}

void HidUSBIO::setDirection(avrPort::Name p,unsigned char mask){
  ddr[p]=mask;
  sendPacket(CMDDDR,ddr.data);
}

void HidUSBIO::setDirection(avrPort::Name p,int bit,bool value){
  setBit(ddr[p],bit,value);
  sendPacket(CMDDDR,ddr.data);
}


void HidUSBIO::setUpdateRead(avrPort::Name p,unsigned char mask){
  updatemask[p]=mask;
  sendPacket(CMDUPDATE,updatemask.data);
}

void HidUSBIO::setUpdateRead(avrPort::Name p,int bit,bool value){
  setBit(updatemask[p],bit,value);
  sendPacket(CMDUPDATE,updatemask.data);
}


void HidUSBIO::includeUpdateRead(const avrPort::ConditionMask& c){
  for (size_t i=0;i<updatemask.size();++i){
    updatemask[i]|=c.setCheck[i]|c.clearCheck[i];
  }
  sendPacket(CMDUPDATE,updatemask.data);
}


void HidUSBIO::setOutput(const unsigned char* ports){
  assign(port,ports);
  sendPacket(CMDPORT,port.data);
}

void HidUSBIO::setOutput(avrPort::Name p,unsigned char value){
  port[p]=value;
  sendPacket(CMDPORT,port.data);
}

void HidUSBIO::setOutput(avrPort::Name p,int bit,bool value){
  setBit(port[p],bit,value);
  sendPacket(CMDPORT,port.data);
}


void HidUSBIO::setPullup(avrPort::Name p,unsigned char value){
  port[p]=value;
  sendPacket(CMDPORT,port.data);
}

void HidUSBIO::setPullup(avrPort::Name p,int bit,bool value){
  setBit(port[p],bit,value);
  sendPacket(CMDPORT,port.data);
}




HidUSBIO::DataWaiter::DataWaiter(size_t psize):
  EhConnReference<const unsigned char*>(this),
  data(psize){

}



void HidUSBIO::DataWaiter::operator()(const unsigned char* c){
  assign(data,c);
  stateChanged.signal();
}


void HidUSBIO::DataWaiter::handle(exc::Exception& e){
  eh.handle(e);
  stateChanged.signal();
}

void HidUSBIO::DataWaiter::waitData(){
  stateChanged.wait();
  eh.check();  
}

unsigned char HidUSBIO::read(avrPort::Name p){
  sendPacket(CMDREAD,toRead);
  readWaiter.waitData();
  return readWaiter.data[p];
}



bool HidUSBIO::read(avrPort::Name p,int bit){
  sendPacket(CMDREAD,toRead);
  readWaiter.waitData();
  return (readWaiter.data[p]&(1<<bit))!=0;
}

void HidUSBIO::read(unsigned char* c){
  sendPacket(CMDREAD,toRead);
  readWaiter.waitData();
  assign(c,readWaiter.data);
}



void HidUSBIO::waitFor(const avrPort::ConditionMask& c){
  do{
    if (c(updateWaiter.data)) return;
    updateWaiter.waitData();
  }while(true); 
}

namespace helium{

  namespace avrPort{
    
    ConditionMask::ConditionMask(size_t s):setCheck(s),clearCheck(s){
      reset();
    };
    
    void ConditionMask::reset(){
      zero(setCheck);
      zero(clearCheck);
    }
  
    bool ConditionMask::operator()(const unsigned char* c) const{
      for (size_t i=0;i<setCheck.size();++i){
	if ((setCheck[i]&c[i])!=setCheck[i]) return false;
	if ((clearCheck[i]&c[i])!=0) return false;
      }
      return true;
    }
    
  }//ns ConditionMask
}//ns helium
