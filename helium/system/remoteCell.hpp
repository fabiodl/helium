/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_REMOTECELLIMPL
#define HE_REMOTECELLIMPL

#include <helium/system/cellSignature.h>

namespace helium{

 

  /*template<typename GT,typename GP,typename ST,typename SP,typename UT,typename UP>
    const typename RemoteCell<GT,GP,typename Signature::ST,SP,UT,UP>::Props RemoteCell<GT,GP,ST,SP,UT,UP>::PROPS;*/
template<typename Signature>
  RemoteCell<Signature>::RemoteCell(RemoteCell<UnspecifiedRemoteCellSignature>* base):
    //connMutex("connMutex"),
    client(base->client),remotename(base->remotename){     
    //std::cout<<"new remoteCell"<<getName()<<"@"<<this<<" from "<<base<<std::endl;
  }
template<typename Signature>
  RemoteCell<Signature>::RemoteCell(CellClient* pc,const std::string& premotename):
    //connMutex("connMutex"),
    client(pc),remotename(premotename){     
    //std::cout<<"new remoteCell"<<getName()<<"@"<<this<<" from "<<base<<std::endl;
  }

   template<typename Signature>
  RemoteCell<Signature>::~RemoteCell(){
    onDelete.act(this);// inform the valueProxies that we are going to be destroyed

    //std::cout<<"deleting remoteCell @"<<((Cell*)this)<<std::endl;

    //printTrace();


    bool canDisconnect=true;
    for (typename std::map<std::string,UpdateReader*>::iterator it=conns.begin();it!=conns.end();it++){      
      if (canDisconnect){
	try{	    
	  if (it->second->par==""){
	    client->cellDisconnect(remotename);
	  }else{
	    client->cellDisconnect(remotename,it->second->par);
	  }	    
	}catch(exc::NetException& ){
	  if (client->getState()==ClientSocket::DISCONNECTED){
	    canDisconnect=false;
	  }
	}
      }//if canDisconnect
      client->remitUpdateReader(it->second);
    }//for 
  }//~RemoteCelll
template<typename Signature>
  void RemoteCell<Signature>::getData(DataP d){
    CellClient::GetRequest<Signature> r(remotename,client,trust_cast<typename Signature::GT*>(d));
    client->get(&r);
  }
template<typename Signature>  
  void RemoteCell<Signature>::getData(DataP out,Cell::CellSignalData in){
    CellClient::GetRequest<Signature> r(remotename,client,trust_cast<typename Signature::GT*>(out));
    client->get<typename Signature::GT,typename Signature::GP>(&r,trust_cast<const typename Signature::GP*>(in));
  }
template<typename Signature>  
  void RemoteCell<Signature>::setData(Cell::CellSignalData d){
    //Std::cout<<"in remCell"<<*((const ST*)d)<<std::endl;
    client->set<typename Signature::ST>(remotename,trust_cast<const typename Signature::ST*>(d));
  }
template<typename Signature>  
  void RemoteCell<Signature>::setData(DataP out,Cell::CellSignalData in){
    CellClient::SetParamRequest<Signature> r(remotename,client,trust_cast<typename Signature::SP*>(out));  
    client->set<typename Signature::ST>(&r,trust_cast<const typename Signature::ST*>(in));
  }

template<typename Signature>  
  void RemoteCell<Signature>::connect(CellConnection &c){
    SharedUse use(connAccess); //exceptions may be thrown by cellConnect
    //std::string nostring;
    typename std::map<std::string,UpdateReader*>::iterator it=conns.find("");
    UpdateReader* ur;
    if (it==conns.end()){
      connAccess.upToExclusive();
      conns[""]=ur=new UpdateReader(*this,"");
      connAccess.downToShared();
    }else{
      ur=it->second;
    }  

    if (ur->cs.size()==0){
      client->cellConnect(remotename,ur);
    }
    connAccess.upToExclusive();
    c.disc=new DInfo(ur,ur->cs.insert(ur->cs.end(),std::make_pair(c.cb,&c)));  
    connAccess.downToShared();
    //std::cout<<"assigned @"<<((Cell*)this)<<" , "<<c.disc<<std::endl;

  }
template<typename Signature>  
  void RemoteCell<Signature>::connect(CellConnection &c,Cell::CellSignalData p){
    SharedUse use(connAccess);


    std::string par;
    convert(par,*(static_cast<const typename Signature::UP*>(p)));
    UpdateReader* ur;
    typename std::map<std::string,UpdateReader*>::iterator it=conns.find(par);
    //std::cout<<"new connection with parameter"<<par<<"typename "<<getTypeName<typename Signature::UP>()<<std::endl;
    if (it==conns.end()){
      connAccess.upToExclusive();
      conns[par]=ur=new UpdateReader(*this,par);
      connAccess.downToShared();
    }else{
      ur=it->second;
    }  

    //connect before connecting, to get early updates
    connAccess.upToExclusive();
    c.disc=new DInfo(ur,ur->cs.insert(ur->cs.end(),std::make_pair(c.cb,&c)));  
    connAccess.downToShared();

    if (ur->cs.size()==1){
      client->cellConnect(remotename,ur,par);
      //std::cout<<"asking the client"<<par<<std::endl;
    }
    
    //std::cout<<"assigned @"<<((Cell*)this)<<" , "<<c.disc<<std::endl;
  }
template<typename Signature>  
  void RemoteCell<Signature>::disconnect(CellConnection &c){
    SharedUse use(connAccess);
    //std::cout<<"disconnecting @"<<((Cell*)this)<<" using "<<c.disc<<std::endl;
    DInfo* dinfo=(trust_cast<DInfo*>(c.disc));
    
    connAccess.upToExclusive();
    dinfo->ur->cs.erase(dinfo->it);
    connAccess.downToShared();    

    if (dinfo->ur->cs.size()==0){
      try{
	if (dinfo->ur->par==""){
	  client->cellDisconnect(remotename);
	}else{
	  client->cellDisconnect(remotename,dinfo->ur->par);
	}
      }catch(exc::WaitingOnDisconnectedSocket e){
      }
      connAccess.upToExclusive();
      conns.erase(dinfo->ur->par);
      connAccess.downToShared();
      client->remitUpdateReader(dinfo->ur);
    }  
    c.clearDisc();
  }



template<typename Signature>
  void RemoteCell<Signature>::streamGet(std::ostream& o){
    //TODO optimize using string
    typename Signature::GT result;
    getData((DataP)&result);
    //std::cout<<"getting the result for "<<remotename<<std::endl;
    write(o,result);
  }
 template<typename Signature>
  void RemoteCell<Signature>::streamGet(std::ostream& o,std::istream& i){
    //TODO optimize using string
    typename Signature::GT result;
    typename Signature::GP param;
    read(param,i);
    getData((DataP)&result,(DataP)&param);
    write(o,result);
  }

template<typename Signature>
  void RemoteCell<Signature>::streamSet(std::istream& i){
    std::string s;
    readWhole(s,i);
    client->set<std::string>(remotename,&s);
  }
template<typename Signature>
  void RemoteCell<Signature>::streamSet(std::ostream& o,std::istream& i){
    //TODO optimize using string
    std::string s,rec;
    readWhole(s,i);
    CellClient::TypedRequest<std::string> r(remotename,client,&rec);  
    client->set<std::string>(&r,&s);
    o<<rec;    
  }
 template<typename Signature>
  void RemoteCell<Signature>::UpdateReader::notify(std::istream& i){
    typename Signature::UT result;
    deserializeUpdate<Signature>(result,i);

    {
      SharedUse use(owner.connAccess);
      for (typename std::list<std::pair<CellCallback*,exc::ExceptionHandler*> >::iterator it=cs.begin();it!=cs.end();it++){
	(*(it->first))( (const DataP)&result);
      }
    }

  }

   template<typename Signature>
   void RemoteCell<Signature>::UpdateReader::fail(const std::string& reason){
     try{
       throw exc::RemoteException(reason);
     }catch(exc::Exception& e){
       SharedUse use(owner.connAccess);
       for (std::list<std::pair<CellCallback*,exc::ExceptionHandler*> >::iterator it=cs.begin();
	    it!=cs.end();
	    ++it){
	 it->second->handle(e);
       }
     }
   }
  
template<typename Signature>
  void RemoteCell<Signature>::formatSignal(std::ostream& o,Cell::CellSignalData data){
    write(o,*(trust_cast<const typename Signature::UT*>(data)));
  }


  
}//helium
#endif
