/*Fabio Dalla Libera   sourceforge.net/projects/libhelium/  */

#include <helium/net/serverSocket.h>
#include <helium/net/commandSplitter.h>


using namespace helium;
using namespace std;

class MyServer:public ServerSocket{   


  void callback(NetSocket& socket){                                   //ovverride the callback, corresponding to one client connected
    const char terminators[3]={10,13,0};    
    std::string command;
    CommandSplitter splitter(command,socket,terminators,terminators); 
    try{                                                              //loop that waits a command and sends it back
      while(true){
	splitter.excWaitMessage();	
	std::stringstream reply;
	reply<<"You sent {"<<command<<"}"<<std::endl;
	sockSend(socket,reply.str());
      }
    }catch(exc::WaitingOnDisconnectedSocket& e){
      cout<<"Connection closed"<<endl;
    }
  }
public:
  MyServer():ServerSocket(1234,false){ //we want the server to listen on tcp port 1234
                                       //and avoiding create a new thread for the listener
  }

};

int main(){
  MyServer server;
  server.execute(); //from Thread 
}
