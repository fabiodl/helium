/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_COMMANDSPLITTER
#define HE_COMMANDSPLITTER

#include <helium/net/netSocket.h>
namespace helium{


 
  class CommandSplitter{
    static const int BUFFERSIZE=1024;
    char buff[BUFFERSIZE];    
    bool isTerminator[256];
    bool isToIgnore[256];
    char charmap[256];
    NetSocket& socket;
    std::string& command;
    int bufferEnd;
    int bufferStart;
   

    bool processBuffer();


  public:
    void setMap(unsigned char dst,unsigned char src);

    CommandSplitter(std::string& pcommand,NetSocket& psocket,const char* terminators,const char* toIgnore);

  
    void excWaitMessage(); //waits for a message, which is stored the string command passed to the constructor
                        //on disconnection, exc::WaitingOnDisconnectedSocket is thrown  
  
    bool retWaitMessage(); //returns true on disconnection
  
  };
  
}

#endif
