/*Fabio Dalla Libera   sourceforge.net/projects/libhelium/  */

#include <helium/net/clientSocket.h>
#include <helium/net/commandSplitter.h>

using namespace helium;
using namespace std;


class MyClient:public ClientSocket{

  void callback(NetSocket& sock){
    std::string msg;
    const char terminator[3]={10,13,0};
    CommandSplitter splitter(msg,sock,terminator,terminator);
    try{
      while(true){
	splitter.excWaitMessage();
	cout<<"Server told '"<<msg<<"'"<<endl;
      }
    }catch(exc::WaitingOnDisconnectedSocket& e){
      cout<<"Server dead"<<endl;
    }
  }   
};


int main(){
  MyClient client;
  client.connect("localhost",1234);
  std::string s;
  while(cin){
    cin>>s;
    client.send(s+"\n"); //we send what we read from keyboard
  }

}
