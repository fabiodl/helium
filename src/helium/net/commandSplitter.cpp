/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/net/commandSplitter.h>
#include <helium/core/exception.h>

namespace helium{

  
  CommandSplitter::CommandSplitter(std::string& pcommand,NetSocket& psocket,const char* terminators,const char* toIgnore):
    socket(psocket),
    command(pcommand),bufferEnd(0),bufferStart(0){
    for (int i=0;i<256;i++){
      charmap[i]=i;
      isTerminator[i]=false;
      isToIgnore[i]=false;
    }
    for (int i=0;terminators[i]!=0;i++){
      isTerminator[(unsigned char)terminators[i]]=true;
    }
    for (int i=0;toIgnore[i]!=0;i++){
      isToIgnore[(unsigned char)toIgnore[i]]=true;
    }
  }

  void CommandSplitter::setMap(unsigned char dst,unsigned char src){
    charmap[src]=dst;
  }
  
  bool CommandSplitter::processBuffer(){
    for (;bufferStart<bufferEnd;bufferStart++){
      unsigned char b=(unsigned char)buff[bufferStart];       
      if (isTerminator[b]&&command.length()>0){
	bufferStart++;
	return true;	    
      }
      if (isToIgnore[b]) continue;
      //std::cout<<"command is"<<command<<std::endl;
      command+=charmap[(unsigned char)buff[bufferStart]];//toupper(buff[i]);	
    }//for  bufferStart
    return false;
  }

  void CommandSplitter::excWaitMessage(){     
      command.clear();
      while(true){
	if (processBuffer()) return;
	bufferStart=0;	
	bufferEnd=sockRecv(socket,buff,BUFFERSIZE);    	
	if (bufferEnd<=0){				 
	  sockClose(socket);
	  throw exc::WaitingOnDisconnectedSocket("(incomplete message"+command+")");

	}  
      }//while true 
    }//waitMessage
  

  bool CommandSplitter::retWaitMessage(){     
      command.clear();
      while(true){
	if (processBuffer()) return false;
	bufferStart=0;
	bufferEnd=sockRecv(socket,buff,BUFFERSIZE);    	
	if (bufferEnd<=0){				 
	  sockClose(socket);
	  return true;
	}  
      }//while true 
  }//waitMessage
  

}
