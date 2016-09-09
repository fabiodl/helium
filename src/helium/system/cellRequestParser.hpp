/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_CELLREQUESTPARSERIMPL
#define HE_CELLREQUESTPARSERIMPL

namespace helium{

template <typename M>
void tell(typename M::Port &port,helium::MessageType type,const std::string& alias,const std::string &content){
  std::stringstream ss;
  if (alias!="") ss<<helium::ALIASCHAR<<alias<<" ";
  ss<<helium::replyChar[type]<<" ";
  ss<<content;
  ss<<helium::COMMANDTERMINATORCHAR<<std::endl;
  port<<ss.str();
}

  template <typename M> 
  void CellRequestParser<M>::CellConnection::operator()(Cell::CellSignalData d){
     std::stringstream ss;
     c->formatSignal(ss,d);
     tell<M>(p,helium::UPDATEMSG,alias,ss.str());
  }

  template <typename M> 
  void CellRequestParser<M>::CellConnection::handle(exc::Exception& e){    
    tell<M>(p,helium::ERRORMSG,alias,toString(e));
  }


template <typename M>
inline void CellRequestParser<M>::send(helium::MessageType type,const std::string& alias,const std::string &content){
  tell<M>(msg.defaultPort,type,alias,content);
}

template <typename M>
inline CellRequestParser<M>::~CellRequestParser(){
  for (typename CellConns::iterator it=cellConns.begin();it!=cellConns.end();it++){   
      delete it->second;   
  }
}

template <typename M>
inline void CellRequestParser<M>::parse(std::string &command){
  std::string alias;
  try{
    std::stringstream ss;
    ss<<command;
    //std::cout<<"received {"<<command<<"}"<<std::endl;
    if (ss.peek()==helium::ALIASCHAR){
      read(star,ss);
      read(alias,ss);
      ss.get();//the space
    }
    
    if (ss.peek()==helium::IDCHAR){
      //std::cout<<"I am reading an address"<<std::endl;
      read(star,ss);
      read(target,ss);	     
    }else{
      read(path,ss);
      //if (alias.length()==0) alias=path;
      std::vector<std::string> tokens;
      tokenize(tokens,path,helium::PATHSEPARATORCHAR);	   
      if (tokens.size()==0) throw exc::SyntaxError("Path cannot be empty");	     
      target=m.downPath(IDPath(tokens))->getDataP();
    }
    if (target.isNull()){
      throw exc::InvalidOperation("No cell attached");
    }

    try{
#ifdef TRACEDEBUG
      exc::ExceptionsAreSilent=true;
#endif
      read(oper,ss);
#ifdef TRACEDEBUG
      exc::ExceptionsAreSilent=false;
#endif


    }catch(exc::Exception& ){
      oper=helium::requestChar[helium::GETVALUEMSG];
    }

    int req=-1;
    for (int i=0;i<helium::MESSAGETYPES;i++){
      if (helium::requestChar[i]==oper){
	req=i;
	break;
      }
    }    

    switch (req){
    case helium::GETVALUEMSG:{
      std::stringstream os;
      target->streamGet(os);
      tell<M>(msg.defaultPort,helium::GETVALUEMSG,alias,os.str());
    }	
    break;
    case helium::GETVALUEPARAMMSG:{
	std::stringstream os;
	target->streamGet(os,ss);
	tell<M>(msg.defaultPort,helium::GETVALUEPARAMMSG,alias,os.str());
    }	
    break;
    case helium::SETVALUEMSG:
      target->streamSet(ss);
    break;	   
    case helium::SETVALUEPARAMMSG:{
      std::stringstream os;
      target->streamSet(os,ss);
      tell<M>(msg.defaultPort,helium::SETVALUEPARAMMSG,alias,os.str());	
    }	
      break;
    case helium::CONNECTMSG:{      
      typename CellConns::iterator cci;
      std::pair<Cell*,std::string> connID=std::make_pair<Cell*,std::string>(target.p,"");
      if ((cci=cellConns.find(connID))!=cellConns.end()){ //avoid multiple connections
	throw exc::InvalidOperation("already connected as "+toString(cci->second->alias));
      }//alreadyConnecrted	
      CellConnection *cc=new CellConnection(target.p,alias,msg.getPort(target.p));
      cellConns.insert(std::make_pair(connID,cc));
      std::stringstream os;
      //void* ref=(void*)cc;
      //os<<ref;
      StdReply r;
      os<<r;
      tell<M>(msg.defaultPort,helium::CONNECTMSG,alias,os.str());
    }
      break;
    case helium::CONNECTPARAMMSG:{
      std::string par;
      getline(ss,par);
      std::stringstream pss;
      pss<<par;
      typename CellConns::iterator cci;
      std::pair<Cell*,std::string> connID=std::make_pair<Cell*,std::string>(target.p,par);
      if ((cci=cellConns.find(connID))!=cellConns.end()){ //avoid multiple connections
	throw exc::InvalidOperation("already connected as "+toString(cci->second->alias));
      }//alreadyConnecrted	
      CellConnection *cc=new CellConnection(target.p,alias,msg.getPort(target.p),pss);
      cellConns.insert(std::make_pair(connID,cc));

      std::stringstream os;
      //void* ref=(void*)cc;
      //os<<ref;
      StdReply r;
      os<<r;
      tell<M>(msg.defaultPort,helium::CONNECTPARAMMSG,alias,os.str());
    }
      break;
    case helium::DISCONNECTMSG:{
      typename CellConns::iterator cci;
      std::pair<Cell*,std::string> connID=std::make_pair<Cell*,std::string>(target.p,"");      
      typename CellConns::iterator it=cellConns.find(connID);
      if (it!=cellConns.end()){
	delete it->second;
	cellConns.erase(it);//the deconstructor removes the connection	   
      }else{
	throw exc::InvalidOperation("unable to disconnect, not connected");
      }
      StdReply r;
      tell<M>(msg.defaultPort,helium::DISCONNECTMSG,alias,toString(r));
    }
      break;
    case helium::DISCONNECTPARAMMSG:{
      typename CellConns::iterator cci;
      std::string par;
      getline(ss,par);
      std::pair<Cell*,std::string> connID=std::make_pair<Cell*,std::string>(target.p,par);      
      typename CellConns::iterator it=cellConns.find(connID);
      if (it!=cellConns.end()){
	delete it->second; //the deconstructor removes the connection	   
	cellConns.erase(it);
      }else{
	throw exc::InvalidOperation("unable to disconnect, not connected");
      }      
      StdReply r;
      tell<M>(msg.defaultPort,helium::DISCONNECTPARAMMSG,alias,toString(r));
    }    
      break;
    default:

      throw exc::SyntaxError(oper+std::string(" is not a valid operator"));	     
    }//switch oper	 
     //COMMAND EXECUTION END
  }catch(exc::Exception& e){
    std::string s=toString(e);
    tell<M>(msg.defaultPort,helium::ERRORMSG,alias,s);
  }
}//parse
  
  
  
  
}//helium

#endif
