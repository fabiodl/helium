/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#include <helium/net/netSocket.h>
#include <helium/net/netSocket_private.h>

#include <iostream>//debug

#ifndef _WIN32
#include <errno.h>
#include <unistd.h>
#endif


namespace helium{
  
 
  class Session{
  public:
    NetSocket socket;
    std::string host;
    int port;
    Session(NetSocket& psocket,const std::string& phost,int pport):
      socket(psocket),host(phost),port(pport){};
  };

  NetSocket* getNewSocket(){
    return new NetSocket;
  }


  bool sockSend(NetSocket& sock,const char* buffer,int nBytes){
    while (nBytes>0){
#ifdef _WIN32
      int s=send(sock.socket,buffer,nBytes,0);
      if (s==SOCKET_ERROR) return false;     
      
#elif defined(__APPLE__)
      int s=send(sock.socket,buffer,nBytes,SO_NOSIGPIPE);
      if (s==-1) return false;      
#else
      int s=send(sock.socket,buffer,nBytes,MSG_NOSIGNAL); 
      if (s==-1) return false;      
#endif
      nBytes=nBytes-s;
    }
    return true;
  }//sockSend


  int sockRecv(NetSocket& sock,char* buffer,int nBytes){
    int r;
    do{
      r=
#ifdef _WIN32
	recv(sock.socket,buffer,nBytes,0);
#else
      read(sock.socket, buffer,nBytes);     
#endif
    }while( (r==
#ifdef _WIN32
	     SOCKET_ERROR
#else
	     -1
#endif
	     )&&
#ifdef _WIN32
	    (WSAGetLastError()==WSAEINTR)
#else
	    (errno==EINTR)
#endif
	    );
    return r;    
  }

  void sockClose(NetSocket& sock){
#ifdef _WIN32
    closesocket(sock.socket);
#else
    shutdown(sock.socket,2);
    close(sock.socket);
#endif
    
  }

  /*


  bool  NetSocket::send(const char* buffer,int nBytes){
    sockSend(*this,buffer,nBytes);
  }

  bool send(NetSocket& sock,const std::string& s){
    sockSend(*this,s);
  }

  int recv
  */

};//helium
