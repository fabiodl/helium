#ifndef HE_SYNCCLIENTSOCKET
#define HE_SYNCCLIENTSOCKET

#include <helium/net/clientSocket.h>

namespace helium{

  class SyncClientSocket:public ClientSocket{
    static const inst BUFFERSIZE=1024;
    Semaphore dataReady,bufferReady;

    char buffer[BUFFERSIZE];
    int rhead,whead;


    void callback(NetSocket& socket){
      int rec;
      while (true){
	
	rec=sockRecv(socket,buffer,BUFFERSIZE-whead)

	sendMutex.lock();
	whead+=rec;
	while (whead>=BUFFERSIZE){
	  sendMutex.unlock();
	  bufferReady.wait();
	  sendMutex.lock();
	}
	sendMutex.unlock();
      }
    }

  public:
    SyncClientSocket():rhead(0),whead(0){
    };

    
  };
#endif
