#define ONLYME
#include <helium/os/bufferedSerial.h>
#include <cstring>
#include <iostream>

namespace helium{

  BufferedSerial:: BufferedSerial(const char *portname, int BaudRate,  int dataBits,Serial::StopBit stopBits,Serial::Parity parityBit):
    s(portname,BaudRate,0,dataBits,stopBits,parityBit),
    access("buffSerial"),
    buffStart(0),
    pendingOperations(0)
  {
    start();

  }
  void BufferedSerial::flush(){
  }
  

  void BufferedSerial::recvAll(unsigned char* data,int size,Time timeout){
    access.lock();
    if (buffStart!=0){
      std::cerr<<"buffer contains "<<buffStart<<"garbage bytes"<<std::endl;
    }
    if (pendingOperations>0){
      std::cerr<<"multiple access on the serial"<<std::endl;
    }
    pendingOperations++;
    while (buffStart<size){
      access.unlock();
      newData.wait();
      access.lock();
    }
    memcpy(data,buffer,size);
    buffStart-=size;
    pendingOperations--;
    access.unlock();
  }


  void BufferedSerial::send(const unsigned char* buff,int size){
     access.lock();
      if (pendingOperations>0){
	std::cerr<<"multiple access on the serial"<<std::endl;
      }
      pendingOperations++;
      access.unlock();
      std::cout<<"sending"<<size<<"bytes"<<std::endl;
      s.send(buff,size);

      access.lock();
      pendingOperations--;
      access.unlock();
  }

  void  BufferedSerial::run(){    
    while(true){
      {
	access.lock();
	int st=buffStart;
	access.unlock();
	st+=s.recv(buffer+st,buffSize-st);
	access.lock();
	int buffStart=st;
	access.unlock();
	std::cout<<"now "<<buffStart<<"bytes"<<std::endl;
	newData.signal();
      }      
    }
  }


  BufferedSerial::~BufferedSerial(){
    join();
  }



}
