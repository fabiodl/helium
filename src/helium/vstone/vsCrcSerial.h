/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_VSCRCSERIAL
#define HE_VSCRCSERIAL

#include <helium/core/time.h>
#include <helium/math/crc.h>
#include <helium/vstone/vsByte.h>
#include <helium/vstone/vsCrcException.h>

namespace helium{


  template<typename Srl>
  class VsCrcSerial{
    Srl& s;
    static const unsigned char WRF=0x40;
    static const unsigned char RDF=0x20;
    static const unsigned char PACKETSTART=0x80;
    static const unsigned char BURST=0xC0;
    static const Time ADDITIONALTIME=30;

    static const int READRETRIES=2;
  public:
    VsCrcSerial(Srl& s);
    const std::string getName();
    void send(int id,int addr,const unsigned char* data,int size); //max size is 16
    void recv(int id,int addr,unsigned char* data,int size);    
    unsigned char initSend(int id,int addr,const unsigned char* data,int size);//returns the checksum. a set of initSend must be concluded with a finalizeSends
    void finalizeSend(unsigned char* receivedCrc,const unsigned char* expectedCrc,int n);

    unsigned char initRecv(int id,int addr,int size); //returns the header crc. a set of initRecv must be concluded with a initRecv
    void finalizeRecv(unsigned char* receivedData,
		      const unsigned char* headerCrc,
		      int spacing,int n); //receivedData has size (spacing*n)



    void sendRecv(int id,int addr,unsigned char* recData, const unsigned char* sendData,int size);
    void vsBurstSendRecv(int id,unsigned char* recData,const unsigned char* sendData,int size);

    unsigned char getHeadCrc(int id,const unsigned char* implicit,int size);

    void fastBurstSend(int id,unsigned char* data,int size,unsigned char crc);
    void fastBurstRecv(int id,unsigned char* data,int recv,unsigned char crc);



    void fastBurstSendRecv(int id,
			   unsigned char* recvData,int recvSize,unsigned char recvCrc,
			   const unsigned char* sendData,int sendSize,unsigned char sendCrc		   
			   );

    void unlock(int id);
    void romWrite(int id,int addr,const unsigned char* data,int size);
    
   

    template<int bytes,int retries> void write(int id,int addr,int val);
    template<int bytes,int retries> int read(int id,int addr);


  };

}

#include "vsCrcSerial.hpp"

#endif
