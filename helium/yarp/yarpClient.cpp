/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/yarp/yarpClient.h>
#include <helium/util/ioUtil.h>
#include <helium/core/exception.h>
#include <helium/signal/wrapInfo.h>
#include <helium/net/commandSplitter.h>

namespace helium{



namespace exc{

class YarpException:public Exception{
public:
  YarpException(const std::string&);
  void rethrow();
  Exception* clone() const;
};


YarpException::YarpException(const std::string& s):
  Exception("YarpException"+addDetail(s))
{
  
}



void YarpException::rethrow(){
  throw *this;
}

Exception* YarpException::clone() const {
  return new YarpException(*this);
}
    


}//ns exc


  ConnInfo::ConnInfo():port(-1){}

  ConnInfo::ConnInfo(const std::string& h,int p):host(h),port(p){}



  YarpClient::Sock::Sock():
    reversed(false)
  {

  }

  void YarpClient::Sock::reverse(){
    send("r\r\n");
    start();
  }
  
  void YarpClient::Sock::onConnected(){
    if (message.listenerNum()){
      reverse();
    }
  }





  void YarpClient::Sock::callback(NetSocket& socket){
    reversed=true;
    std::string cmd;
    CommandSplitter split(cmd,socket,"\n","");
    while(message.listenerNum()){

      try{
	split.excWaitMessage();
	//discard the update header
	split.excWaitMessage();
	message(cmd);
      }catch(exc::Exception& e){
	if (message.listenerNum()) throw; //if there's no one listening, we disconnected, silently ignore the exc
      }
    }        
  }

  YarpClient::Sock::~Sock(){
    if (reversed){
      join();
    }
  }



  void YarpClient::Sock::recvUntil(char* buffer,int buffersize,const char* terminator){
    int r;
    int rec=0;
    int tlen=strlen(terminator);
    buffersize--;//let the space for the 0 termination
    do{
      if (!buffersize){
	throw exc::SyntaxError("Buffer end reached before terminator");
      }

     r=recv(buffer+rec,buffersize);

     rec+=r;
     buffer[rec]=0;
     buffersize-=r;

    }while(rec<tlen||strcmp(terminator,buffer+rec-tlen));      
  }


  static const int BUFFERSIZE=1024;
  


  void YarpClient::Sock::connectReader(ConnReference<const std::string&>& c){
    message.connect(c);
    if (message.listenerNum()==1){      
      reverse();
    }
  };


  void YarpClient::Sock::disconnectReader(ConnReference<const std::string&>& c){
    message.disconnect(c);
    if (!message.listenerNum()){      
      disconnect();
    }
  };


void YarpClient::Sock::connect(const ConnInfo& ci){
  char buffer[BUFFERSIZE];
  ClientSocket::connect(ci.host,ci.port,false);
  send("CONNECT helium\n");  
  recvUntil(buffer,BUFFERSIZE,"helium\r\n"); 
}


  bool YarpClient::Sock::hasListeners(){
    return message.listenerNum();
  }

  void YarpClient::connectReader(const std::string& path,ConnReference<const std::string&>&c){
    getSocket(path)->connectReader(c);
    wrapInfo(c,path);
  }


  void YarpClient::disconnectReader(ConnReference<const std::string& >&c){
    std::string path;
    unwrapInfo(c,path);
    getSocket(path)->disconnectReader(c);
  }

  


  void YarpClient::disconnect(){
    nameserver.disconnect();
    for (SockCache::iterator it=clients.begin();it!=clients.end();){
      it->second->disconnect();
      if (!it->second->hasListeners()){	
	delete it->second;
	SockCache::iterator itcopy=it;
	++it;
	clients.erase(itcopy);
      }else{
	++it;
      }
    }
  }//disconnect


    ConnInfo YarpClient::NameServer::getLocation(const std::string& path){
      ConnInfo ci;  
      send("d\nquery "+path+"\n");  
      char buffer[BUFFERSIZE];
      recvUntil(buffer,BUFFERSIZE,"*** end of message\r\n");
      std::stringstream ss(buffer);     
      std::string s;
      for (int i=0;i<4;i++){
	ss>>s;//
      }
      ss>>ci.host;
      ss>>s;
      try{
	helium::read(ci.port,ss);
      }catch(exc::InvalidDataValue&){
	throw exc::YarpException("cannot retrieve port. Is {"+path+ "} a valid path?");
      }      
      return ci;
    }


YarpClient::Sock* YarpClient::getSocket(const std::string& path){     
  SockCache::iterator it=clients.find(path);
  Sock* s;
  if (it==clients.end()){
    s=new Sock();
    s->connect(nameserver.getLocation(path));    
    clients.insert(std::make_pair(path,s));
  }else{
    s=it->second;
  }
  return s;
}



void YarpClient::connect(const ConnInfo& c){
  nameserver.connect(c);
  for (SockCache::iterator it=clients.begin();it!=clients.end();++it){
    it->second->connect(nameserver.getLocation(it->first));
  }
}




void  YarpClient::send(const std::string& path,const char* c){  
  getSocket(path)->send(c);    
}




int  YarpClient::recv(const std::string& path,char* c,size_t size){
  return getSocket(path)->recv(c,size);
}

  void YarpClient::recvUntil(const std::string& path,char* c,size_t size,const char* terminator){
      return getSocket(path)->recvUntil(c,size,terminator);
  };





}//ns hel
