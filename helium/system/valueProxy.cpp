/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#include <helium/system/valueProxy.h>


namespace helium{

namespace exc{

    
    OperationOnDisconnectedProxy::OperationOnDisconnectedProxy(const std::string& s):InvalidOperation(s){
    }
    
   void OperationOnDisconnectedProxy::rethrow(){
	throw *this;
    }
    

    Exception* OperationOnDisconnectedProxy::clone(){
	return new OperationOnDisconnectedProxy(*this);
      }

  }//ns exc


void GenericValueProxy::action(Cell* c){
      unsetRemote();
  }

  
  GenericValueProxy::GenericValueProxy():remote(NULL){
  }

  void GenericValueProxy::setRemote(Cell* c){      
      acc.enterShared();      
      try{
	if (remote!=NULL){
	  if (c!=NULL){
	    std::cerr<<"HEY, setting a remote while the previous one is still here..."<<std::endl;
	  }
	  remoteUnset();
	}
      }catch(...){
	acc.leaveShared();
	throw;
      }
      acc.upToExclusive();
      
      c->onDelete.push_front(this);
      remote=c;
      //std::cout<<"setting remote cell "<<c->getName()<<std::endl;
      acc.downToShared();
      try{
	if (remote!=NULL){
	  remoteAvailable();
	}
      }catch(...){
	acc.leaveShared();
	throw;
      }
      acc.leaveShared();
    }


  Cell* GenericValueProxy::getRemote(){      
    return remote;
  }


    
    void GenericValueProxy::unsetRemote(){
      acc.enterShared();
      try{
	if (remote!=NULL){
	  remoteUnset();
	}      
      }catch(...){
	acc.leaveShared();

	acc.enterExclusive();
	remote=NULL;
	acc.leaveExclusive();
	
	throw;
      }
      acc.leaveShared();

      acc.enterExclusive();
      //std::cout<<"setting "<<remote<<" as NULL"<<std::endl;
      remote=NULL;
      acc.leaveExclusive();
    }
    

      void GenericValueProxy::getData(DataP out){      
      SharedUse use(acc);
      if (remote==NULL)
	throw exc::OperationOnDisconnectedProxy("getData");
      remote->getData(out);

    }      
    void GenericValueProxy::getData(DataP out,CellSignalData in){
      SharedUse use(acc);
      if (remote==NULL)
	throw exc::OperationOnDisconnectedProxy("getDataParam");
      remote->getData(out,in);
    }     
    void GenericValueProxy::setData(CellSignalData in){
      SharedUse use(acc);
      if (remote==NULL)
	throw exc::OperationOnDisconnectedProxy("setData");
      remote->setData(in);      
    }       

    void GenericValueProxy::safeSetData(CellSignalData in){
      SharedUse use(acc);
      if (remote!=NULL){
	remote->setData(in);      
      }
    }       
    

     void GenericValueProxy::setData(DataP out,CellSignalData in){      
      SharedUse use(acc);
      if (remote==NULL)
	throw exc::OperationOnDisconnectedProxy("setDataParam");
      remote->setData(out,in);
    }  
    

   void GenericValueProxy::safeSetData(DataP out,CellSignalData in){      
      SharedUse use(acc);
      if (remote!=NULL){
	remote->setData(out,in);
      }
    }  


    void GenericValueProxy::streamGet(std::ostream& o){      
      SharedUse use(acc);
      if (remote==NULL)
	throw exc::OperationOnDisconnectedProxy("streamGet");
      remote->streamGet(o);
    }
  
    void GenericValueProxy::print(std::ostream& o){ 
      SharedUse use(acc);
      if (remote==NULL){
	o<<"Proxy of disconnected remote Cell";
      }else{
	remote->streamGet(o);
      }
    }


    void GenericValueProxy::streamGet(std::ostream& o,std::istream& i){
      SharedUse use(acc);      
      if (remote==NULL)
	throw exc::OperationOnDisconnectedProxy("streamGetParam");
      remote->streamGet(o,i);
    }  

     void GenericValueProxy::streamSet(std::istream& i){
       SharedUse use(acc);
      if (remote==NULL)
	throw exc::OperationOnDisconnectedProxy("streamSet");
      remote->streamSet(i);
    }

     void GenericValueProxy::streamSet(std::ostream& o,std::istream& i){
       SharedUse use(acc);
      if (remote==NULL)
	throw exc::OperationOnDisconnectedProxy("streamSetParam");
      remote->streamGet(o,i);
    }

     void GenericValueProxy::connect(CellConnection& ref){
       SharedUse use(acc);       
       if (remote==NULL)
	 throw exc::OperationOnDisconnectedProxy("connect");

       /*if (this==remote){
	 throw 8;
	 }*/

       remote->connect(ref);       
    }

     void GenericValueProxy::connect(CellConnection& ref,CellSignalData p){
       SharedUse use(acc);
      if (remote==NULL)
	throw exc::OperationOnDisconnectedProxy("connectParam");
      remote->connect(ref,p);
    }

     void GenericValueProxy::connect(CellConnection& ref,std::istream& i){
       SharedUse use(acc);
       if (remote==NULL)
	 throw exc::OperationOnDisconnectedProxy("connectStream");
       remote->connect(ref,i);
     }

    void GenericValueProxy::disconnect(CellConnection &d){
      SharedUse use(acc);
       if (remote==NULL)
	 throw exc::OperationOnDisconnectedProxy("disconnect");
       remote->disconnect(d);
    }

     void GenericValueProxy::formatSignal(std::ostream& o,CellSignalData data){
       SharedUse use(acc);
       if (remote==NULL)
	 throw exc::OperationOnDisconnectedProxy("format");
       remote->formatSignal(o,data);
    }

     void GenericValueProxy::formatParam(std::ostream& o,CellSignalData data){
       SharedUse use(acc);       
       if (remote==NULL)
	 throw exc::OperationOnDisconnectedProxy("formatStream");
       remote->formatParam(o,data);
    }
     
     bool GenericValueProxy::isConnected(){
       SharedUse use(acc);       
       return remote!=NULL;
     }

    int GenericValueProxy::getRemoteFlagSet(CellFlags f){
      SharedUse use(acc);       
      if (remote==NULL)
	throw exc::OperationOnDisconnectedProxy("getFlags");
      if (remote->getProperties()==NULL) return 0;
      return remote->getProperties()->getCellFlags();
	//isCellFlagSet(remote,f);
    }

    int GenericValueProxy::getRemoteCapabilities(){
      SharedUse use(acc);       
      if (remote==NULL)
	throw exc::OperationOnDisconnectedProxy("getCapabilities");
      if (remote->getProperties()==NULL) return 0;
      return remote->getProperties()->getCapabilities();
    }

  


 const CellProperties* GenericValueProxy::getProperties() const{
      return SingleFlagCellProperties<VALUEPROXY>::getInstance();//VALUEPROXY();
    }



}
