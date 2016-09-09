/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SOCKETMESSAGESPOOLER
#define HE_SOCKETMESSAGESPOOLER

#include <helium/core/multiportPipe.h>
#include <helium/net/netSocket.h>
#include <helium/thread/thread.h>
#include <helium/system/cell.h>
#include <helium/debug/debugOptions.h>
#include <map>

namespace helium{

  class SocketMessageSpooler:public Thread,public MultiportPipe<std::string>{ //used for internal managaement   
  public:
    typedef MultiportPipe<std::string>::BackPort Port;
  private:
    NetSocket& socket;
    bool running;
    std::map<int,Port*> activePorts;
#ifdef LOGCELLSERVER
    BasicLog& slog;
#endif
    
  public:
    
    Port defaultPort;
    void run();
    SocketMessageSpooler(NetSocket& socket
#ifdef LOGCELLSERVER
			 ,BasicLog& pslog
#endif		       
			 );
    virtual ~SocketMessageSpooler();
    Port& getPort(Cell* c);
    
  };
  
  
  
  
  
}//ns helium



#endif
