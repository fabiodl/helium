/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.05.05
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_CLIENTSOCKET
#define HE_CLIENTSOCKET

#include <helium/signal/signal.h>
#include <helium/thread/mutex.h>
#include <helium/thread/thread.h>
#include <helium/net/netSocket.h>
#include <helium/core/time.h>
#include <string>

namespace helium{

  class NetSocket;



  class ClientSocket:public Thread{


    void finalizeRun();
  protected:
    Mutex sendMutex;


  public:
    enum State{DISCONNECTED,CONNECTED,CONNECTING,DISCONNECTING};

  protected:
    State state; 

    NetSocket& sockfd;
    std::string lastHost;
    int lastPort;

    void setState(State pstate);
    void closeConnection(); 


  public:

    enum Error{WSASTARTUP,SOCKETCREATION,HOSTBYNAME,CONNECTION};

    ClientSocket();

    virtual ~ClientSocket();
    
    void connect(const std::string& host,int port,bool autoStart=true);

    //if autostart is true it starts a new thread, which simply calls the callback once

    virtual void callback(NetSocket& socket){}
    void send(const char* s,int n);
    void send(const unsigned char* s,int n);

    void send(const char *str);
    void send(const std::string& s);   

    int recv(char* s,int n);
    int recv(unsigned char* s,int n);

    void recvAll(char* s,int n,Time additionalTime);    
    void recvAll(unsigned char* s,int n,Time additionalTime);
   
    void disconnect();
    void reconnect();
    
    State getState();
    //ConnSignal<false> onConnect;
    //ConnSignal<true> onDisconnect;

    TsSignal<void> onConnect;
    TsSignal<void> onDisconnect;



    virtual void onConnected();//prints and calls onConnect
    virtual void onDisconnected();//prints and calls onDisconnect
    
   
    void run();
    

    
  };
  
  
}//helium

#endif
