/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_NETSOCKET_PRIVATE
#define HE_NETSOCKET_PRIVATE

#ifdef _WIN32
#pragma warning (disable: 4018)
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wsock32.lib")
#include <winsock2.h>
#include <Windows.h>

#else
#include <arpa/inet.h>
#include <netdb.h>
#endif

namespace helium{
class NetSocket{
public:
#ifdef _WIN32
  SOCKET socket;
#else
  int socket;
#endif  

  /*  bool send(const char* buffer,int nBytes);
  bool send(NetSocket& sock,const std::string& s);
  int recv(NetSocket& sock,char*,int n); //<=0 on error
  void close(NetSocket& sock);
  }*/
};//NetSocket



}

#endif
