/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_SERVERSOCKET
#define HE_SERVERSOCKET

#include <helium/net/netSocket.h>
#include <helium/thread/thread.h>
#include <helium/thread/semaphore.h>
#include <helium/thread/mutex.h>

#include <string>
#include <list>


namespace helium{

 
  class Session;

  class ServerSocket:public Thread{
  private:
    
    enum State{READY,RUNNING,CLOSED} state;
    
    int port;
    NetSocket& mainSocket;   
    

    const Session* getSession(const NetSocket& );

 protected:
    friend class ServerSessionCallback;
    void executeSession(Session& socket);

    std::list<Session*> sessions;
    Mutex sessionMutex;
    Semaphore sessionTerminated;  
    void run();
    virtual void callback(NetSocket& socket)=0;
  public:
    int getPort();
    int  getClientPort(const NetSocket& );
    std::string getClientHost(const NetSocket& );
    
    enum Error{WSASTARTUP,SOCKETCREATION,SOCKETBINDING,SOCKETLISTENING,SOCKETACCEPT};
    std::string getErrorDescription(Error e);
    //virtual void onError(Error e);
    virtual void onStartedListening();
    
    ServerSocket(int port,bool autostart=true);    
    void close();

    virtual ~ServerSocket(); 
  }; //ServerSocket
  
}//helium

#endif
