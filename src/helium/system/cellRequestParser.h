/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_CELLREQUESTPARSER
#define HE_CELLREQUESTPARSER
#include <helium/core/exception.h>
#include <helium/system/cell.h>
#include <helium/system/cellProtocol.h>

namespace helium{
  
   /*  class M should have
       - a typedef/class Port with operator <<
       - a member Port defaultPort
       - a function Port& getPort(Cell* c)
  
       an example is SocketMessageSpooler
   */
  

  
  template <typename M> 
  class CellRequestParser{
  private:
    class CellConnection:public Cell::CellCallback,public Cell::CellConnection{ //used for the cells to which we connect
      friend class CellRequestParser;
      Cell* c;
      std::string alias;
      typename M::Port &p;     
    public:
      
      CellConnection(Cell* pc,const std::string& palias,typename M::Port &pp):Cell::CellConnection(this),
	c(pc),alias(palias),p(pp){
	c->connect(*static_cast<Cell::CellConnection*>(this));
      }    
      
      CellConnection(Cell* pc,const std::string& palias,typename M::Port &pp,std::istream& param):Cell::CellConnection(this),
	c(pc),alias(palias),p(pp){
	c->connect(*static_cast<Cell::CellConnection*>(this),param);
	//std::cout<<"connected "<<this<<" dinfo="<<this->disc<<std::endl;
      }    
      
      virtual ~CellConnection(){
	try{
	  //std::cout<<"disconnecting "<<alias<<" from cell "<<"with disc ponter "<<&this->disc<<" value "<<this->disc<<std::endl;
	  c->disconnect(*static_cast<Cell::CellConnection*>(this));
	}catch(exc::UnsupportedCellOperation e){
	  std::cout<<"Exception while disconnecting: "<<e.what()<<std::endl;
	}
      }    
      void operator()(Cell::CellSignalData d);
      /*Cell::CellCallback* clone() const{
	return NULL; //Why should you clone this?
	} */
      
      void handle(exc::Exception &e);

    };//CellConnection
     
  M &msg;
  Memory &m;
  typedef std::map<std::pair<Cell*,std::string> ,CellConnection*> CellConns;
  CellConns cellConns;
  char star;
  CellReference target;
  std::string path;
  char oper;
    
 public:
  inline void send(helium::MessageType type,const std::string& alias,const std::string &content);
  inline void parse(std::string &command);
  CellRequestParser(M& pmsg,Memory& pm):msg(pmsg),m(pm){
  }
  
  virtual ~CellRequestParser();

  };//CellRequestParser
  

}//helium

#include <helium/system/cellRequestParser.hpp>

#endif
