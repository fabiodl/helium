/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.02.18
 * \version 0.1
 * Release_flags s
 */


#ifndef HE_DYNAMIXELSERIAL
#define HE_DYNAMIXELSERIAL


#include <helium/vector/array.h>


namespace helium{
  
  namespace dynamixel{
    enum {HEAD1,HEAD2,ID,LENGTH,INSTRUCTION,PARAMETERSTART};

    enum InstructionType{PING=0x01,READ_DATA=0x02,WRITE_DATA=0x03,REG_WRITE=0x04,ACTION=0x05,RESET=0x06,SYNC_WRITE=0x83};
    
    enum ReadComponents{READADDRESS=INSTRUCTION+1,READLENGTH=INSTRUCTION+2};
    enum StatusComponents{STATUSERRORFLAGS=LENGTH+1};

    enum ErrorFlags{INSTRUCTIONERROR=1<<6,OVERLOADERROR=1<<5,CHECKSUMERROR=1<<4,RANGEERROR=1<<3,OVERHEATINGERROR=1<<2,ANGLELIMITERROR=1<<1,INPUTVOLTAGEERROR=1<<0};
    
    enum StatusReturnLevel{NEVERRETURNSTATUS,ONLYONREADSTATUS,ALWAYSRETURNSTATUS};
    enum {BROADCASTID=0xFE};



  };
  

  


  template<typename Srl>
  class DynamixelSerial{
    Srl& s;
    unsigned char outbuff[2024],inbuff[1024];
    Array<dynamixel::StatusReturnLevel> statusReturnLevel;
    static const int ADDITIONALTIME=100;
    
   

    void communicate(int id,dynamixel::InstructionType inst,int params);

    

  public:
    DynamixelSerial(Srl& ps,int maxId);

    void setStatusReturnLevel(int id,dynamixel::StatusReturnLevel);

    void ping(int id);
    void action(int id);
    void reset(int id);    
    void recv(int id,int addr, unsigned char* data, size_t n);
    void send(int id,int addr,const unsigned char* data, size_t n);

    template<int bytes>    
    int read(int id,int addr);
    
    template<int bytes>    
    void write(int id,int addr,int val);
    

    void regWrite(int id,int addr,const unsigned char* data, size_t n);

    //first call getSyncBuffer, fill in the data and then call syncwrite
    //format: id1 data_1...data_n id2 data_1 ...data_n

    unsigned char* getSyncWriteBuffer();
    void syncWrite(int addr,int nmotors,int singleMotordataLength);

  
    
  };



}//ns helium

#include <helium/dynamixel/dynamixelSerial.hpp>


#endif
