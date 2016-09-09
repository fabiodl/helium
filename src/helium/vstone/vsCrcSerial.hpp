/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/os/time.h>
#include <helium/util/printHex.h>
namespace helium{


  template<typename T> struct hasFlush{    
    template<typename X,void (X::*m)()> struct dummy;    
    template<typename X,typename Y>
    static tt::yes check(dummy<X,&X::flush>*,Y);
			 template<typename X>
			 static tt::no check(...);  
			 static const bool value= sizeof(check<T>(NULL,0))==sizeof(tt::yes);			 			
			 };
    
    template<typename T,bool b=hasFlush<T>::value>
    struct callFlush{
      static void call(T& t){}
    };

    template<typename T>
    struct callFlush<T,true>{
      static void call(T& t){t.flush();}
    };



  template<typename Srl>
  VsCrcSerial<Srl>::VsCrcSerial(Srl& ps):s(ps){}
  

  template<typename Srl>
  void VsCrcSerial<Srl>::send(int id,int addr,const unsigned char* data,int size){
    unsigned char msg[3+32+1];//header+maxsize+crc
    msg[0]=PACKETSTART|id;
    msg[1]=WRF|size;
    msg[2]=addr;
    memcpy(msg+3,data,size);
    msg[3+size]=0x7f&crc8maxim::getCrc(msg,3+size);
    callFlush<Srl>::call(s);
    s.send(msg,3+size+1);
   
    unsigned char remCrc;
    //void* add=&remCrc;
    //std::cout<<"address "<<add<<"+"<<std::endl;
    //Time t=getSystemMillis();
    s.recvAll(&remCrc,1,ADDITIONALTIME);
    //std::cout<<"waited reply"<<getElapsedMillis(t)<<std::endl;
    if (remCrc!=msg[3+size]){
      throw exc::VsSerialCRCError<false,true>(id,addr,size,msg[3+size],remCrc);
    }
  }

  template<typename Srl>
  void VsCrcSerial<Srl>::recv(int id,int addr,unsigned char* data,int size){  
    unsigned char header[3];
    header[0]=PACKETSTART|id;
    header[1]=RDF|size;
    header[2]=addr;
    callFlush<Srl>::call(s);
    s.send(header,3);
    //printHex(header,3);
    //std::cout<<" was sent"<<std::endl;
    unsigned char recdata[32+1];
    s.recvAll(recdata,size+1,ADDITIONALTIME);
    unsigned char crc=crc8maxim::getCrc(header,3);
    crc=crc8maxim::getCrc(recdata,size,crc);//no need of 0x7f, read returns the full crc
    //printHex(recdata,size);
    //std::cout<<" was received i.e"<<from14(recdata)<<" "<<from14(recdata+2)<<std::endl;    
    //unsigned char xxx[2];
    //to14(xxx,1000);
    //    printHex(xxx,2);
    //std::cout<<"would make sense"<<std::endl;

    if (recdata[size]!=crc){
      throw exc::VsSerialCRCError<true,false>(id,addr,size,crc,recdata[size]);
    }
    memcpy(data,recdata,size);
  }

  template<typename Srl>
  unsigned char VsCrcSerial<Srl>::initSend(int id,int addr,const  unsigned char* data,int size){
    unsigned char msg[3+32+1];//header+maxsize+crc
    msg[0]=PACKETSTART|id;
    msg[1]=WRF|size;
    msg[2]=addr;
    memcpy(msg+3,data,size);
    msg[3+size]=0x7f&crc8maxim::getCrc(msg,3+size);
    s.send(msg,3+size+1);   
    return msg[3+size];
  }

  template<typename Srl>
  void VsCrcSerial<Srl>::finalizeSend(unsigned char* receivedCrc,const unsigned char* expectedCrc,int n){   
    s.recvAll(receivedCrc,n,ADDITIONALTIME);   //may send an exception here
    {
      exc::MultipleExceptionHolder meh;
      for (int i=0;i<n;i++){
	if (receivedCrc[i]!=expectedCrc[i]){
	  meh.handle(exc::VsSerialCRCError<false,true>("multiple send"));	
	}
      }
    }
  }


  template<typename Srl>
  unsigned char VsCrcSerial<Srl>::initRecv(int id,int addr,int size){
    unsigned char header[3];
    header[0]=PACKETSTART|id;
    header[1]=RDF|size;
    header[2]=addr;
    //s.flush();
    s.send(header,3);
    return crc8maxim::getCrc(header,3);
  }

  template<typename Srl>
  void VsCrcSerial<Srl>::finalizeRecv(unsigned char* receivedData,
					       const unsigned char* headerCrc,
					       int spacing,int n){
    s.recvAll(receivedData,spacing*n,ADDITIONALTIME);
    for (int i=0;i<n;i++){
      unsigned char crc=crc8maxim::getCrc(receivedData,spacing-1,headerCrc[i]);
      if (receivedData[spacing*i]!=crc){
	throw exc::VsSerialCRCError<true,false>("multiple recv");
      }
    }    
  }

  template<typename Srl>
  void VsCrcSerial<Srl>::sendRecv(int id,int addr,unsigned char* recData, const unsigned char* sendData,int size){  
    unsigned char header[3];
    header[0]=PACKETSTART|id;
    header[1]=WRF|RDF|size;
    header[2]=addr;
    callFlush<Srl>::call(s);
    s.send(header,3);
    s.send(sendData,size);
    s.recvAll(recData,size,ADDITIONALTIME);    
    unsigned char crc=crc8maxim::getCrc(header,3);
    crc=crc8maxim::getCrc(recData,size,crc);
    unsigned char remCrc;
    s.recvAll(&remCrc,size,ADDITIONALTIME);    
    if (remCrc!=crc){
      throw exc::VsSerialCRCError<true,true>(id,addr,size,crc,remCrc);
    }
  }

  template<typename Srl>
  void VsCrcSerial<Srl>::vsBurstSendRecv(int id,unsigned char* recData,const unsigned char* sendData,int size){
    unsigned char header=0xC0 | id;
    callFlush<Srl>::call(s);
    s.send(&header,1);
    s.send(sendData,size);
    s.recvAll(recData,size,ADDITIONALTIME);
  }

  template<typename Srl>
  unsigned char VsCrcSerial<Srl>::getHeadCrc(int id,const unsigned char* implicit,int size){
    unsigned char header=0xC0 | id;
    return crc8maxim::getCrc(implicit,size,crc8maxim::getCrc(header));
  }


  /*template<typename Srl>
  void VsCrcSerial<Srl>::fastBurstSend(int id,int addr,unsigned char* data,int size,unsigned char crc){
      }else{
      unsigned char remCrc;
      s.recvAll(&remCrc,1,ADDITIONALTIME);
      if (crc!=remCrc){
	throw VsSerialCRCError<false,true>(id,addr,sendSize,crc,remCrc);	  
      }

  }*/


  template<typename Srl>
  void VsCrcSerial<Srl>::fastBurstSend(int id,unsigned char* sendData,int sendSize,unsigned char sendCrc){
    unsigned char buffer[32+2];
    buffer[0]=0xC0 | id;
    memcpy(buffer+1,sendData,sendSize);
    buffer[sendSize+1]=0x7F&crc8maxim::getCrc(sendData,sendSize,sendCrc);     
    callFlush<Srl>::call(s);
    s.send(buffer,sendSize+2);
    //std::cout<<"wait.."<<std::endl;
    s.recvAll(buffer,1,ADDITIONALTIME);
    if (buffer[0]!=buffer[sendSize+1]){
      throw exc::VsSerialCRCError<true,false>(id,-1,sendSize,buffer[sendSize+1],buffer[0]);	  
    }    
  }

  
  template<typename Srl>
  void VsCrcSerial<Srl>::fastBurstRecv(int id,unsigned char* recvData,int recvSize,unsigned char recvCrc){
    unsigned char buffer[32+2];
    buffer[0]=0xC0 | id;
    callFlush<Srl>::call(s);
    s.send(buffer,1);
    s.recvAll(buffer,recvSize+1,ADDITIONALTIME);
    unsigned char crc=crc8maxim::getCrc(buffer,recvSize,recvCrc);
    if (crc!=buffer[recvSize]){     
      throw exc::VsSerialCRCError<true,false>(id,-1,recvSize,crc,buffer[recvSize]);	  
    }
    memcpy(recvData,buffer,recvSize);      
  }

  template<typename Srl>
  void VsCrcSerial<Srl>::fastBurstSendRecv(int id,
					   unsigned char* recvData,int recvSize,unsigned char recvCrc,
					   const unsigned char* sendData,int sendSize,unsigned char sendCrc){
    unsigned char buffer[32+2];
    buffer[0]=0xC0 | id;
    memcpy(buffer+1,sendData,sendSize);
    buffer[sendSize+1]=0x7F&crc8maxim::getCrc(sendData,sendSize,sendCrc);        
    callFlush<Srl>::call(s);
    s.send(buffer,sendSize+2);
    s.recvAll(buffer,recvSize+1,ADDITIONALTIME);
    unsigned char crc=crc8maxim::getCrc(buffer,recvSize,recvCrc);    
    if (crc!=buffer[recvSize]){     
      throw exc::VsSerialCRCError<true,true>(id,-1,recvSize,crc,buffer[recvSize]);	  
    }
    memcpy(recvData,buffer,recvSize);      

    
  }

  

  template<typename Srl>
  void VsCrcSerial<Srl>::romWrite(int id,int addr,const unsigned char* data,int size){
    unsigned char header[4];
    header[0]=PACKETSTART|id; 
    header[1]=size; //no read and no write = ROM
    to14(&header[2],addr);
    int csum=header[2]+header[3];
    for (int i=0;i<size;i++){
      if (data[i]&0x80){
	throw exc::InvalidOperation("sending a data with more than 7 bits");
      }
      csum+=data[i];
    }
    unsigned char mcsum=(-csum)&0x7f;
    callFlush<Srl>::call(s);
    s.send(header,4);
    s.send(data,size);

    s.send(&mcsum,1);
    s.recvAll(&mcsum,1,ADDITIONALTIME);
    if (mcsum!=id){ // returns 0|id on success and 0x40|id on error
      std::cout<<"csum="<<std::hex<<mcsum<<"id="<<std::dec<<id<<std::endl;
      throw exc::VsSerialChecksumError(id,addr,size);
    }
  }
  
  template<typename Srl>
  void VsCrcSerial<Srl>::unlock(int id){
    unsigned char c=0x55;   
    send(id,0x14,&c,1);
  }

  template<typename Srl>
  const std::string VsCrcSerial<Srl>::getName(){
    return s.getName();
  }

  template<int bytes> struct crcVsSerialCaller;

  template<>
  struct crcVsSerialCaller<1>{

    template<typename Srl>
    inline static void write(VsCrcSerial<Srl>& cs,int id,int addr,int val){

      unsigned char data=val&0x7F;
      cs.send(id,addr,&data,1);      
    }
    
    template<typename Srl>
    inline static int read(VsCrcSerial<Srl>& cs,int id,int addr){
      unsigned char data;
      cs.recv(id,addr,&data,1);
      return data;
    }
  };

  template<>
  struct crcVsSerialCaller<2>{
    template<typename Srl>    
    inline static void write(VsCrcSerial<Srl>& cs,int id,int addr,int val){
      unsigned char data[2];
      to14(data,val);
      cs.send(id,addr,data,2);    
    }
    template<typename Srl>
    inline static int read(VsCrcSerial<Srl>& cs,int id,int addr){
      unsigned char data[2];
      cs.recv(id,addr,data,2);
      return from14(data);      

    }
  };

  template<>
  struct crcVsSerialCaller<4>{
     template<typename Srl>
    inline static void write(VsCrcSerial<Srl>& cs,int id,int addr,int val){
      unsigned char data[4];
      to28(data,val);
      cs.send(id,addr,data,4);    
    }
    template<typename Srl>
    inline static int read(VsCrcSerial<Srl>& cs,int id,int addr){
      unsigned char data[4];
      cs.recv(id,addr,data,4);
      return from28(data);          
    }
  };
  
  

  template<typename Srl>
  template<int bytes,int retries> void VsCrcSerial<Srl>::write(int id,int addr,int val){    
    try{
      crcVsSerialCaller<bytes>::write(*this,id,addr,val);
    }catch(exc::Exception&){    
      for (int i=0;i<retries;i++){
	callFlush<Srl>::call(s);
	try{
	  crcVsSerialCaller<bytes>::write(*this,id,addr,val);
	  return;
	}catch(exc::Exception&){	  
	}	
      }//for retries
      throw; //if we cannot return in one of the retries;
    }//catch      
  }//write


   
  template<typename Srl>
  template<int bytes,int retries> int VsCrcSerial<Srl>::read(int id,int addr){
    try{
      return crcVsSerialCaller<bytes>::read(*this,id,addr);
    }catch(exc::Exception&){    
      for (int i=0;i<retries;i++){
	callFlush<Srl>::call(s);
	try{
	  return crcVsSerialCaller<bytes>::read(*this,id,addr);
	}catch(exc::Exception&){	  
	}	
      }//for retries
      throw; //if we cannot return in one of the retries;
    }//catch      
  }
  
  


}
