/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/system/socketMessageSpooler.h>


namespace helium{

SocketMessageSpooler::SocketMessageSpooler(NetSocket& psocket
#ifdef LOGCELLSERVER
					   ,BasicLog& pslog
#endif
):
  socket(psocket),running(false)
#ifdef LOGCELLSERVER
  ,slog(pslog)
#endif

{

  attachPort(defaultPort);
  start();
}

void SocketMessageSpooler::run(){
  running=true;
  while(true){
    DataReference d;
    pop(d);
    if (!running) break;
    sockSend(socket,**d);

#ifdef LOGCELLSERVER
    slog<<getSystemMillis()<<"s "<<**d<<std::endl;
#endif
  }

#ifdef LOGCELLSERVER
  //slog<<this<<"dead"<<std::endl;
#endif

}

SocketMessageSpooler::~SocketMessageSpooler(){
  running=false;
  defaultPort<<"END";//fake message for assuring termination
  join();  
  for (std::map<int,Port*>::iterator it=activePorts.begin();it!=activePorts.end();it++){
    delete it->second;
  }
}


  SocketMessageSpooler::Port& SocketMessageSpooler::getPort(Cell* target){
    int p=target->getProperties()==NULL?-1:target->getProperties()->getPreferredPort();	   
    std::map<int,SocketMessageSpooler::Port*>::iterator it;
    if ((it=activePorts.find(p))==activePorts.end()){
      SocketMessageSpooler::Port* commport=new SocketMessageSpooler::Port();
      attachPort(*commport);
      //std::cout<<"created port "<<p<<std::endl;
      std::pair<std::map<int,SocketMessageSpooler::Port*>::iterator,bool> ii=activePorts.insert(std::make_pair(p,commport));
      it=ii.first;
    }
    return *(it->second);
  }


}
