/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_CELLSERVER
#define HE_CELLSERVER


#include <helium/net/serverSocket.h>
#include <helium/system/cell.h>
#include <helium/debug/debugOptions.h>


#ifdef LOGCELLSERVER
#include <helium/util/basicLog.h>
#endif

namespace helium{
  
  
  class CellServer:public ServerSocket{    
    Memory &m;
    Mutex sidMutex;
    int sid;
#ifdef LOGCELLSERVER
    BasicLog slog;
#endif
  protected:
    void callback(NetSocket& socket);
  public:
    CellServer(int port,Memory &m,bool autostart=true);
    ~CellServer();
  };


}



#endif
