/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_YARPCLIENT
#define HE_YARPCLIENT


#include <helium/system/basicCell.h>
#include <helium/net/clientSocket.h>

namespace helium{

  

  struct ConnInfo{
    std::string host;
    int port;

    ConnInfo();
    ConnInfo(const std::string& ,int);

  };
  
  
  class YarpClient{

    
    class Sock:public ClientSocket{      
      
      TsSignal<const std::string&> message;
      void onConnected();
      bool reversed;
      void reverse();
    public:
      void connect(const ConnInfo& );
      void recvUntil(char* buffer,int buffersize,const char* terminator);
      void callback(NetSocket& socket);
      void connectReader(ConnReference<const std::string&>&);
      void disconnectReader(ConnReference<const std::string&>&);
      bool hasListeners();
      Sock();
      ~Sock();
    };


    

    
    class NameServer:public Sock{

    public:
      ConnInfo getLocation(const std::string& path);

    };


    NameServer nameserver;


    typedef std::map<std::string,Sock*> SockCache;



    SockCache clients;

    Sock* getSocket(const std::string& path);


    

  public:  



    void send(const std::string& path,const char*);
    int recv(const std::string& path,char*,size_t size);
    void recvUntil(const std::string& path,char*,size_t size,const char* terminator);



    void connect(const ConnInfo&);   
    void disconnect();

    void connectReader(const std::string& path,ConnReference<const std::string&>&c);
    void disconnectReader(ConnReference<const std::string&>&c);

    void disconnect(const std::string& path);
    //YarpClient();
    //~YarpClient();

  };








}//ns hel








#endif
