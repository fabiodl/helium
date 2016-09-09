#ifndef HE_BUFFEREDSERIAL
#define HE_BUFFEREDSERIAL

#include <helium/thread/thread.h>
#include <helium/thread/mutex.h>
#include <helium/thread/semaphore.h>
#include <helium/os/serial.h>


#ifdef ONLYME

namespace helium{
  class BufferedSerial:public Thread{    
    Serial s;
    Mutex access;
    Semaphore newData;
    static const int buffSize=1024;
    unsigned char buffer[buffSize];
    int buffStart;
    int pendingOperations;

    void run();

  public:
    BufferedSerial(const char *portname, int BaudRate,  int dataBits=8,Serial::StopBit stopBits=Serial::ONE_SB,Serial::Parity parityBit=Serial::NO_PARITY);  
    ~BufferedSerial();

    void flush();
    void send(const unsigned char* buff,int size);
    void recvAll(unsigned char* data,int size,Time timeout);
    int bufferContent(unsigned char* data);
    std::string getName(){
      return s.getName();
    }
  };

}
#endif

#endif
