/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/system/keyboardCellServer.h>
#include <stdio.h>
namespace helium{

KeyboardCellServer::Port& KeyboardCellServer::getPort(Cell* c){
      return std::cout;
    }


void KeyboardCellServer::run(){
    CellRequestParser<KeyboardCellServer> parser(*this,m);

     
     std::cin.ignore();
    while(std::cin){
      std::string command;
      std::cin.clear();
      std::getline(std::cin,command);             
      //std::cout<<"got line"<<(std::cin?"*":"/")<<std::endl;
      if (!std::cin) break;
      if (command.length()>0){
	  parser.parse(command);
      }
    }
    onStreamTermination();
  }  

KeyboardCellServer:: KeyboardCellServer(Memory &pm):defaultPort(std::cout),m(pm){
    }

KeyboardCellServer::~KeyboardCellServer(){
  join();
}

void KeyboardCellServer::stopReading(){
  std::cout<<"closing kbd"<<std::endl;
    //((std::iostream*)std::cin)->~std::iostream();
    //fclose(stdin);
    //close(1);
}
}
