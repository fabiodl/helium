
/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.05.05
 * \version 0.2
 * Release_flags s
 */

#include <sstream>
#include <helium/net/netSocket_private.h>
#include <helium/net/clientSocket.h>
#include <helium/util/ioUtil.h>
#include <helium/util/mout.h>
#include <helium/os/time.h>
#include <cstring> //for memset

#ifndef _WIN32
#include <unistd.h>
#endif

namespace helium{



  template<>
  struct EnumStrings<ClientSocket::Error>{
    static const int NUM=4;
    static const char* strings[NUM];       
  };
  

  template<>
  struct EnumStrings<ClientSocket::State>{
    static const int NUM=4;
    static const char* strings[NUM];       
  };



  
  const char* EnumStrings<ClientSocket::Error>::
  strings[EnumStrings<ClientSocket::Error>::NUM]={"WSAStartup",
						  "socket cretion",
						  "get host by name",
						  "connection"};


  const char* EnumStrings<ClientSocket::State>::
  strings[EnumStrings<ClientSocket::State>::NUM]={"disconnected",
						  "connected",
						  "connecting",
						  "disconnecting"};


  namespace exc{
    class ConnectException:public NetException{
    public:      
      ConnectException(const std::string& host,int port,ClientSocket::Error e):
	NetException(host,port,toString(e)+" failed")
      {
	
      }
      virtual void rethrow(){
	throw *this;
      }
      
      Exception* clone() const{
	return new NetException(*this);
      }
    };//Connect exception

    
    class NetTimeoutException:public NetException{
    public:      
      NetTimeoutException(const std::string& host,int port,Time tout):
	NetException(host,port,stringBegin()<<" timeout of "<<tout<<" expired during recv"<<stringEnd())
      {
	
      }
      virtual void rethrow(){
	throw *this;
      }
      
      Exception* clone() const{
	return new NetException(*this);
      }
    };//Connect exception

  }//ns exc



    
  void ClientSocket::setState(ClientSocket::State pstate){
    sendMutex.lock();
    state=pstate;
    sendMutex.unlock();
  }

  ClientSocket::State ClientSocket::getState(){
    State r; 
    sendMutex.lock();
    r=state;
    sendMutex.unlock();
    return r;
  }

  ClientSocket::ClientSocket():sendMutex("sendMutex"),
			       state(DISCONNECTED),
			       sockfd(*new NetSocket()),
			       lastPort(-1)
  {
  }
  using namespace en;
  void ClientSocket::connect(const std::string& host, int port,bool startConn){

    //std::cout<<"test here"<<en::eio<<state<<std::endl;

    sendMutex.lock();
    if (state!=DISCONNECTED){
      sendMutex.unlock();
      throw exc::InvalidNetOperation(lastHost,lastPort,
				     stringBegin()
				     <<"connecting to "<<host<<":"<<port<<" while status is "<<en::eio<<state<<stringEnd());
    }
    state=CONNECTING;
    sendMutex.unlock();

    lastPort=port;
    lastHost=host;
    
    mout(msg::OK)("Connecting to "+toString(host)+":"+toString(port));
  
     //if something happens, we put the state back to disconnect adn then rethrow
    try{

  //set up the socket file descriptor
  
  /* get a datagram socket */
#ifdef WIN32
   SOCKADDR_IN serv_addr;
   WSADATA wsaData;
   if(WSAStartup(MAKEWORD(2,0),&wsaData) != 0){
     throw exc::ConnectException(lastHost,lastPort,WSASTARTUP);
   }

   sockfd.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if(sockfd.socket == INVALID_SOCKET){
     throw exc::ConnectException(lastHost,lastPort,SOCKETCREATION);
   }
#else
  sockfd.socket = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd.socket<0) {
    throw exc::ConnectException(lastHost,lastPort,SOCKETCREATION);
  }
  struct sockaddr_in serv_addr;
#endif


 

 struct hostent *he;
 if((he=gethostbyname(host.c_str())) == NULL){
   throw exc::ConnectException(lastHost,lastPort,HOSTBYNAME);
  }




  /* assign our destination address */
  memset ( &serv_addr, 0, sizeof(serv_addr) );
  serv_addr.sin_family	 =	AF_INET;
  serv_addr.sin_addr.s_addr  =	inet_addr(host.c_str());
  serv_addr.sin_port		 =	htons(port);
  //  std::cout<<"connection request"<<std::endl;



  serv_addr.sin_addr = *((struct in_addr *)he->h_addr);
  if ( ::connect(sockfd.socket, (struct sockaddr*) &serv_addr, sizeof(serv_addr))<0)  {
    throw exc::ConnectException(lastHost,lastPort,CONNECTION);
  }
  
  
  
  if (startConn){   
    start();    
  }else{   
  }

  setState(CONNECTED); 

    }catch(exc::Exception& ){
        setState(DISCONNECTED); 
	throw;
    }

 

  onConnected();

}


  void ClientSocket::send(const char *str,int n){
    sendMutex.lock();
    if (state!=CONNECTED){
      sendMutex.unlock();
      throw exc::SendingOnDisconnectedSocket(lastHost,lastPort);
    }
#ifdef WIN32
    ::send(sockfd.socket, (const char*)str, n,0);
#else
    if (::write(sockfd.socket, str, n)!=n){
      sendMutex.unlock();
      throw exc::NetException(lastHost,lastPort,stringBegin()<<"unable to send "<<n<<" bytes"<<stringEnd());
    }
#endif
    sendMutex.unlock();
  }

  void ClientSocket::send(const char *str) 
  {
    send(str,(int)strlen(str));
  }

  void ClientSocket::send(const std::string &s){
    send(s.c_str());
  }

  int ClientSocket::recv(char* s,int n){
    return sockRecv(sockfd,s,n);
  }

  void ClientSocket::recvAll(char* data,int size,Time additionalTime){
    int n=recv(data,size);
    Time now=getSystemMillis();
    while (n<size&&(getSystemMillis()<now+additionalTime)){
      n=n+recv(&data[n],size-n);
    }
    if (n!=size){
      throw exc::NetTimeoutException(lastHost,lastPort,additionalTime);
    }
  }


  void ClientSocket::closeConnection(){
    sendMutex.lock();
    if (state==ClientSocket::DISCONNECTING||state==ClientSocket::DISCONNECTED){
      sendMutex.unlock();
      return;
    }
    state=ClientSocket::DISCONNECTING;
    sendMutex.unlock();
   
    try{  
      sockClose(sockfd);
    }catch(...){	
    }

  }


  void ClientSocket::finalizeRun(){
    closeConnection();
    setState(DISCONNECTED);
    onDisconnected();
  }
  
  void ClientSocket::run(){
    try{        
      callback(sockfd);
    }catch(exc::Exception &){
      finalizeRun();
      throw;
    }
    finalizeRun();
  }



  void ClientSocket::disconnect(){
    closeConnection();
    join();
    if (state!=DISCONNECTED){
      setState(DISCONNECTED);
      onDisconnected();
    }
  }


  
  ClientSocket::~ClientSocket(){
    disconnect();
    delete &sockfd;
  }



  void ClientSocket::reconnect(){
    disconnect();
    connect(lastHost.c_str(),lastPort);
  }

  void ClientSocket::onConnected(){
    mout(msg::PINFO,1)("Connected to "+lastHost+":"+toString(lastPort));
    onConnect();
  }

  void ClientSocket::onDisconnected(){
    mout(msg::NINFO)("Disconnected from "+lastHost+":"+toString(lastPort));

    onDisconnect();
  }
  
  
  void ClientSocket::send(const unsigned char* s,int n){
    send((const char*)s,n);
  }

  int ClientSocket::recv(unsigned char* s,int n){
    return recv((char*)s,n);
  }

  void ClientSocket::recvAll(unsigned char* s,int n,Time additionalTime){
   recvAll((char*)s,n,additionalTime);
  }


  
}//helium
