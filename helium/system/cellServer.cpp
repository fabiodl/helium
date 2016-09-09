/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <map>
#include <helium/system/cellServer.h>
#include <helium/system/cellRequestParser.h>
#include <helium/system/socketMessageSpooler.h>
#include <helium/core/multiportPipe.h>
#include <helium/net/commandSplitter.h>
#include <helium/util/mout.h>


using namespace std;
using namespace helium;









CellServer::CellServer(int port,Memory& pm,bool autostart):
  ServerSocket(port,autostart),m(pm),sidMutex("sidMutex"),sid(0)
#ifdef LOGCELLSERVER
  ,slog("cellCommunication","")
#endif
{
 
}

void CellServer::callback(NetSocket& socket){
  sidMutex.lock();
  int mid=sid++;
  mout(msg::PINFO,1)(stringBegin()<<"New server session, id "<<mid<<stringEnd());
  sidMutex.unlock();
  SocketMessageSpooler msg(socket
#ifdef LOGCELLSERVER
			   ,slog
#endif
			   );
  CellRequestParser<SocketMessageSpooler> parser(msg,m);
  parser.send(helium::WELCOMEMSG,"","Welcome client "+toString(mid));
  std::string command;
  char toIgnore[3]={10,13,0};
  CommandSplitter splitter(command,socket,";",toIgnore);
  while(true){    
    if (splitter.retWaitMessage()){
      break;      
    }
#ifdef LOGCELLSERVER
    slog<<getSystemMillis()<<"r "<<command<<std::endl;
#endif
    parser.parse(command);
    //std::cout<<"parsed command"<< command<<std::endl;
  }//while running
  mout(msg::NINFO,1)(stringBegin()<<"Server session closed, id "<<mid<<stringEnd());
}//callback

CellServer:: ~CellServer(){
  close();
  join();
}


