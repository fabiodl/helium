#ifndef _HE_RECONNECTINGCELLCLIENT_
#define _HE_RECONNECTINGCELLCLIENT_


#include <helium/system/cellClient.h>
#include <helium/system/remoteCellImporter.h>

namespace helium{

  class ReconnectingCellClient:public CellClient{
    RemoteCellImporter* rci;
    Memory& mem;

    bool running;

    //needed because calling connect inside onDisconnected would nest the process
    class Reconnector:public Thread{
      Semaphore s;
      ClientSocket& client;
      bool running;
    public:
      Reconnector(ClientSocket& _client);
      void askReconnection();
      void run();
    };//class Reconnector
    Reconnector reconnector;
    void parse(const std::string & command);
  public:
    ReconnectingCellClient(Memory& _mem);
    ~ReconnectingCellClient();        
    void onConnected();
    void onDisconnected();
  };

  
  

}//helium

#endif
