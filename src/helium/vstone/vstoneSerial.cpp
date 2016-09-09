/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#include <helium/vstone/vstoneSerial.h>
#include <helium/vector/operations.h>
#include <helium/robot/hardwareException.h>
#include <helium/vstone/vsByte.h>
#include <helium/os/time.h>

static const unsigned char WRF=0x40;
static const unsigned char RDF=0x20;
static const unsigned char RAM=0x80;
static const unsigned char BURST=0xC0;



namespace helium{

  static const Time ADDITIONALTIME=30;

  namespace exc{
    class VstoneSerialChecksumError:public exc::HardwareException{
    public:
      VstoneSerialChecksumError(int id,int addr,int size):
	HardwareException("Checksum error while writing "+toString(size)+" bytes from "+toString(addr)+" in motor "+toString(id)){
       
      }
      
      void rethrow(){
	throw *this;
      }
      
    Exception* clone(){
      return new VstoneSerialChecksumError(*this);
    }
      
    };
  }


 


  VstoneSerial::VstoneSerial(Serial& ps):s(ps),lastId(-1){
    zeron(zeros,32);
  }


  void VstoneSerial::unlock(int id){
    unsigned char c=0x55;
    send(id,0x14,&c,1);
  }

  void VstoneSerial::waitReady(int id){
    if (id==lastId) milliSleep(2);
    lastId=id;
  }


  
  void VstoneSerial::send(int id,int addr,const unsigned char* data,int size){
    waitReady(id);    
    unsigned char header[3];
    header[0]=0x80|id;
    header[1]=WRF|size;
    header[2]=addr;
    s.send(header,3);
    s.send(data,size);    
  }


  void VstoneSerial::recv(int id,int addr,unsigned char* data,int size){
    waitReady(id);    
    unsigned char header[3];
    header[0]=0x80|id;
    header[1]=RDF|size;
    header[2]=addr;
    s.flush();
    s.send(header,3);
    s.send(zeros,size);
    s.recvAll(data,size,ADDITIONALTIME);
    /*std::cout<<id<<":"<<addr<<"received "<<size<<"bytes:"<<std::endl;
    for (int i=0;i<size;i++){
      std::cout<<" "<<(int)(data[i])<<std::endl;
      }*/
  }
  
  
  void VstoneSerial::sendRcv(int id,int addr,unsigned char* recData, const unsigned char* sendData,int size){
    waitReady(id);    
    unsigned char header[3];
    header[0]=0x80|id;
    header[1]=WRF|RDF|size;
    header[2]=addr;
    s.flush();
    s.send(header,3);
    s.send(sendData,size);
    s.recvAll(recData,size,ADDITIONALTIME);    
  }
  
  void VstoneSerial::burstSendRcv(int id,unsigned char* recData,const unsigned char* sendData,int size){
    waitReady(id);    
    unsigned char header=0xC0 | id;
    s.flush();
    s.send(&header,1);
    s.send(sendData,size);
    s.recvAll(recData,size,ADDITIONALTIME);
  }

  void VstoneSerial::romWrite(int id,int addr,const unsigned char* data,int size){
    waitReady(id);    
    unsigned char header[4];
    header[0]=0x80|id;
    header[1]=size;
    to14(&header[2],addr);
    int csum=header[2]+header[3];
    for (int i=0;i<size;i++){
      if (data[i]&0x80){
	throw exc::InvalidOperation("sending a data with more than 7 bits");
      }
      csum+=data[i];
    }
    unsigned char mcsum=(-csum)&0x7f;
    s.flush();
    s.send(header,4);
    s.send(data,size);

    s.send(&mcsum,1);
    s.recvAll(&mcsum,1,ADDITIONALTIME);
    if (mcsum!=id){ // returns 0|id on success and 0x40|id on error
      std::cout<<"csum="<<std::hex<<(int)mcsum<<"id="<<std::dec<<id<<std::endl;
      throw exc::VstoneSerialChecksumError(id,addr,size);
    }
  }


   void VstoneSerial::write1(int id,int addr,int val){
      unsigned char data=val&0x7F;
      send(id,addr,&data,1);
    }
    
     int VstoneSerial::read1(int id,int addr){
      unsigned char data;
      recv(id,addr,&data,1);
      return data;
    }


   void VstoneSerial::write2(int id,int addr,int val){
      unsigned char data[2];
      to14(data,val);
      send(id,addr,data,2);    
    }
     int VstoneSerial::read2(int id,int addr){
      int r;
      unsigned char data[2];
      recv(id,addr,data,2);
      from14(r,data);      
      return r;
    }
  void VstoneSerial::write4(int id,int addr,int val){
    unsigned char data[4];
    to28(data,val);
      send(id,addr,data,4);    
  }
  int VstoneSerial::read4(int id,int addr){
      int r;
      unsigned char data[4];
      recv(id,addr,data,4);
      from28(r,data);      
      return r;
    }



  const std::string VstoneSerial::getName(){
    return s.getName();
  }
  
}
