#include "reconnectingCellClient.h"
#include <helium/os/time.h>

namespace helium{
ReconnectingCellClient::Reconnector::Reconnector(ClientSocket& _client):
  client(_client),
  running(false)
{	
  start();
}


void ReconnectingCellClient::Reconnector::askReconnection(){
  s.signal();
}

void ReconnectingCellClient::Reconnector::run(){
  running=true;
  int retries=1;
  while(running){
    s.wait();
    while(running){
      std::cout<<"Trying to reconnect in "<<retries<<"seconds"<<std::endl;
      milliSleep(1000*retries);
      try{	      
	client.reconnect();
	std::cout<<"reconnection successfull"<<std::endl;
	retries=1;
	break;
      }catch(...){
	std::cerr<<"reconnection failed"<<std::endl;
	retries++;
      }
    }//while running innter	
  }//while running outer
}//while run


ReconnectingCellClient::ReconnectingCellClient(Memory& _mem):      
  rci(NULL),mem(_mem),reconnector(*this){
}

ReconnectingCellClient::~ReconnectingCellClient(){
  if (rci) delete rci;
}

void ReconnectingCellClient::onConnected(){
  rci=new RemoteCellImporter(mem,*this);
  rci->connectProxies();
}
void ReconnectingCellClient::onDisconnected(){
  if (rci){
    delete rci;
    rci=NULL;
  }
  reconnector.askReconnection();      
}


void ReconnectingCellClient::parse(const std::string & command){
  try{
    CellClient::parse(command);
  }catch(exc::Exception& e){
    std::cerr<<"Exception in parsing "<<command<<":"<<e<<std::endl;
  }
}

}//ns helium
