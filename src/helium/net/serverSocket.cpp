/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/net/netSocket_private.h>
#include <helium/net/serverSocket.h>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"wsock32.lib")
//#pragma comment(lib,"ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#endif

#include <helium/core/exception.h>
#include <helium/util/mout.h>

using namespace std;



namespace helium{

  


  template<>
  struct EnumStrings<ServerSocket::Error>{
    static const int NUM=5;
    static const char* strings[NUM];     
  };


  const char* EnumStrings<ServerSocket::Error>::strings[]={"WSAStartup"
							   "socket creation",
							   "socket binding",
							   "socket listening",
							   "socket accept"			   

  };

  namespace exc{

    class ServerStartupException:public NetException{

    public:


      ServerStartupException(int port,ServerSocket::Error e):NetException("TCP server, port "+toString(port)+" error in "+toString(e)){
      }
      
      virtual void rethrow(){
	throw *this;
      }

      Exception* clone() const{
	return new ServerStartupException(*this);
      }


    };



  };


/*std::string ServerSocket::getErrorDescription(Error e){
  stringstream ss;
  ss<<"TCP server, port "<<port<<": error in "<<error[e]<<".";
  return ss.str();
  }*/


/*void ServerSocket::onError(Error e){
  std::string s=getErrorDescription(e);
  std::cerr<<s<<std::endl;
  throw exc::NetException(s);
  }*/


  
  class Session{
    public:
    NetSocket socket;
    std::string host;
    int port;
    Session(const NetSocket& psocket,const std::string& phost,int pport):
      socket(psocket),host(phost),port(pport){};
  };
  
  class ServerSessionCallback:public ClonableCallback<void>{
    ServerSocket* ss;
    NetSocket sock;
    Session session;

    
  public:
    
    ServerSessionCallback(ServerSocket* pss,
			  const NetSocket& pns,const std::string& pip,int pport):
      ss(pss),
      sock(pns),
      session(sock,pip,pport){
      //cout<<"new session, socket "<<session.socket.socket<<std::endl;	
    }
					 
    void operator()(){
      ss->executeSession(session);
    }
    Callback<void>* clone() const{
      return new ServerSessionCallback(*this);
    }

  };


ServerSocket::ServerSocket(int pport,bool autostart):state(READY),port(pport),
				      mainSocket(*new NetSocket()),
				      sessionMutex("sessionMutex"){

  if (autostart) start();
}


ServerSocket::~ServerSocket(){
  close();
  delete &mainSocket;
}
void ServerSocket::run(){
 
  struct sockaddr_in local;
#ifdef _WIN32	
  WSADATA wsaData;
  int wsaret=WSAStartup(0x101,&wsaData);
  if(wsaret!=0)
    {
      throw exc::ServerStartupException(port,WSASTARTUP);
    }
#endif
  local.sin_family=AF_INET;
#ifdef _WIN32
  local.sin_addr.s_addr=INADDR_ANY;
#else
  local.sin_addr.s_addr=htonl(INADDR_ANY);
#endif 
  local.sin_port=htons((u_short)port);
  mainSocket.socket=socket(AF_INET,SOCK_STREAM,0);
#ifdef _WIN32
  int val=1;
  setsockopt(mainSocket.socket,SOL_SOCKET,SO_REUSEADDR,(char*)&val,sizeof(val));
#else
  int val=1;
  setsockopt(mainSocket.socket,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));
#endif
#ifdef _WIN32
  if(mainSocket.socket==INVALID_SOCKET)
#else
    if(mainSocket.socket<0)
#endif
      {
	throw exc::ServerStartupException(port,SOCKETCREATION);
      }
  if(bind(mainSocket.socket,(struct sockaddr*)&local,sizeof(local))!=0)
    {
      throw exc::ServerStartupException(port,SOCKETBINDING);
    }



  if(listen(mainSocket.socket,10)!=0)
    {
      throw exc::ServerStartupException(port,SOCKETLISTENING);
    }
	
  struct sockaddr_in from;
#ifdef _WIN32
  int fromlen=sizeof(from);
#else
  socklen_t fromlen=sizeof(from);
#endif
  sessionMutex.lock();
  if (state==READY) state=RUNNING;
  sessionMutex.unlock();
  onStartedListening();
  while(state==RUNNING){
    //std::cout<<"accept"<<std::endl;
    NetSocket cs;

    do{
      cs.socket=accept(mainSocket.socket,(struct sockaddr*)&from,&fromlen);            
    }while(cs.socket==-1&&errno==
#ifdef _WIN32
	   WSAEINTR
#else
	   EINTR
#endif
);
      
    
    if (state==RUNNING){

      if (cs.socket==
#ifdef _WIN32
	  INVALID_SOCKET
#else
	  -1
#endif
	  ){

	throw exc::ServerStartupException(port,SOCKETACCEPT);
      }else{
	Thread::run(ServerSessionCallback(this,cs,inet_ntoa(from.sin_addr),ntohs(from.sin_port)));//the callback is copied, and destroyed at thread termination
      }

    }//if running
  }//while running
	
}//run

void ServerSocket::executeSession(Session& s){
  sessionMutex.lock();
  std::list<Session*>::iterator id=sessions.insert(sessions.begin(),&s);
  sessionMutex.unlock();
  //std::cout<<getThreadId()<<"has socket "<<s.socket.socket<<std::endl;
  callback(s.socket);
  sockClose(s.socket);
  //std::cout<<getThreadId()<<"ends with socket "<<s.socket.socket<<std::endl;
  sessionMutex.lock();
  sessions.erase(id);
  sessionMutex.unlock();
  sessionTerminated.signal();
}


  const Session* ServerSocket::getSession(const NetSocket& ns){
    for (std::list<Session*>::iterator i=sessions.begin();i!=sessions.end();i++){
      if ((*i)->socket.socket==ns.socket) return *i;
     }
    return NULL;
  }


  int  ServerSocket::getClientPort(const NetSocket& s){
    return getSession(s)->port;
  }
  std::string ServerSocket::getClientHost(const NetSocket& s){
    return getSession(s)->host;
  }

  

void ServerSocket::close(){
  sessionMutex.lock();
  state=CLOSED;
  sessionMutex.unlock();
  for (std::list<Session*>::iterator i=sessions.begin();i!=sessions.end();i++){
    sockClose((*i)->socket);
  }
  //std::cout<<"close"<<std::endl;
  sockClose(mainSocket);
  sessionMutex.lock();
  do{
    if (sessions.size()>0){
      sessionMutex.unlock();
      sessionTerminated.wait();
      sessionMutex.lock();
    }
  }while(sessions.size()>0);
  sessionMutex.unlock();
  //std::cout<<"calling join from close"<<std::endl;
  //join(); //no nead, already in thread
  //std::cout<<"caled join from close"<<std::endl;
}


void ServerSocket::onStartedListening(){
  mout(msg::PINFO)(stringBegin()<<"Server listening on port "<<port<<stringEnd());
}

  int ServerSocket::getPort(){
    struct sockaddr_in sin;
#ifdef _WIN32
	int
#else
    socklen_t  
#endif
		len = sizeof(sin);
    if (getsockname(mainSocket.socket, (struct sockaddr *)&sin, &len) == 
#ifdef _WIN32
	SOCKET_ERROR
#else
-1
#endif
	){
      throw exc::NetException("Unable to retrieve socket's port");
    }
    return ntohs(sin.sin_port);

  }


}//ns hel
