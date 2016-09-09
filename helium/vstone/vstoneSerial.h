/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */



#ifndef HE_VSTONESERIAL
#define HE_VSTONESERIAL

#include <helium/os/serial.h>

namespace helium{
  
  class VstoneSerial{
    Serial& s;
    unsigned char zeros[32];
    int lastId;
    void waitReady(int id);
  public:
    VstoneSerial(Serial& s);
    const std::string getName();
    void send(int id,int addr,const unsigned char* data,int size); //max size is 16
    void recv(int id,int addr,unsigned char* data,int size);
    void sendRcv(int id,int addr,unsigned char* recData, const unsigned char* sendData,int size);
    void burstSendRcv(int id,unsigned char* recData,const unsigned char* sendData,int size);

    void unlock(int id);
    void romWrite(int id,int addr,const unsigned char* data,int size);
    
    void write1(int id,int addr,int val);    
    int read1(int id,int addr);

    void write2(int id,int addr,int val);
    int read2(int id,int addr);

    void write4(int id,int addr,int val);
    int read4(int id,int addr);


  };

}


#endif
