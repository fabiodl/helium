/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_NETSOCKET
#define HE_NETSOCKET

#include <string>

namespace helium{

  class NetSocket;
  

  bool sockSend(NetSocket& sock,const char*,int n);  //retruns false on error

  inline bool sockSend(NetSocket& sock,const std::string& s){
    return sockSend(sock,s.c_str(),(int)s.length());
  }
      
  int sockRecv(NetSocket& sock,char*,int n); //<=0 on error
  void sockClose(NetSocket& sock);

  

};


#endif
