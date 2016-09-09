/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/hiddevice/hidBusyBitTouchSensor.h>
#include <helium/math/crc.h>
#include <helium/util/printHex.h>
#include <helium/os/hidRawException.h>

using namespace helium;

enum DataFormat{DEVICE,LENGTH,ADDR,CRC};
enum LoopDevice{LOOPDEVADDR=0xFE};
enum {READOP=0x01,WRITEOP=0x00};
enum SpecialCommands{CMDCANCEL,CMDREAD};


namespace helium{
  namespace exc{
    
    class BbCrcException:public Exception{      

    public:
      BbCrcException():Exception("Busy bit loop read Crc error"){
      }      
      
      virtual void rethrow(){
	throw *this;
      }
      Exception* clone() const{
	return new BbCrcException(*this);
      }
    

  };

}

void HidBusyBitTouchSensor::requestRead(int offset){
  outbuffer[offset+DEVICE]=dev<<1|READOP;
  outbuffer[offset+LENGTH]=length;
  outbuffer[offset+ADDR]=addr;
  outbuffer[offset+CRC]=crc8maxim::getCrc(outbuffer+offset,3);

  try{
  sendAll(outbuffer);
  }catch(exc::Exception& ){
    throw exc::HidRawException(dev,addr,length,exc::HidRawException::REQUESTREAD);
  }
}


  HidBusyBitTouchSensor:: HidBusyBitTouchSensor(int pdev,int paddr,int pn,bool autoStart):
  UsbHid(0x16c0,0x05dc,"fabiodl@gmail.com","USBi2bb"),
  n(pn),
  dev(pdev),addr(paddr),length((n-1)/8+1),
  inbuffer(length+1),outbuffer(5),
  implicitCrc(getImplicitCrc())
{
  
  outbuffer[DEVICE]=LOOPDEVADDR|WRITEOP;
  requestRead(1);    
  if (autoStart){
    start();
  }
}


unsigned char HidBusyBitTouchSensor::getImplicitCrc(){
  unsigned char implicit[3];
  implicit[0]=dev<<1|READOP;
  implicit[1]=length;
  implicit[2]=addr;   
  return crc8maxim::getCrc(implicit,3);
}


void HidBusyBitTouchSensor::run(){
  running=true;
  while(running){
    try{
      recvAll(inbuffer);
      //std::cout<<"got ";
      //printHex(inbuffer.data,length+2);
      //std::cout<<std::endl;
      if (crc8maxim::getCrc(inbuffer,length+1,implicitCrc)!=0){
	throw exc::BbCrcException();
      }else{
	//std::cout<<"UPdate"<<std::endl;
	onSensorUpdate(inbuffer);
      }
    }catch(exc::Exception& e){      
      onError(&e);
    }
  }
}


HidBusyBitTouchSensor::~HidBusyBitTouchSensor(){
  running=false;
  close();
}

  
  size_t HidBusyBitTouchSensor::size(){
    return n;
  }
  
}//ns helium
