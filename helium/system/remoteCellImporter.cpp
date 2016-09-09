/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/system/remoteCellImporter.h>
#include <helium/system/cellProtocol.h>
#include <helium/debug/debugOptions.h>
namespace helium{


  //for debug
  //std::map<GenericValueProxy*,IDNode<Cell>* > onTree;

  
  void RemoteCellImporter::readResult(std::istream& in){
      char oper;
      char transparent;
      std::string alias,cellID;
     
      int nalias;
      while(in){
	in>>oper;
	if (in.fail()) return;
	

	if (oper== mdescChar[MDPUSHID]){
	  in>>transparent;
	  in>>nalias;
	  in>>alias;		
	  path.downPath(alias);
	  IDNode<Cell>* n=m.assurePath(path);
#ifdef DEBUG_RCI
	  std::cout<<"created path "<<path<<std::endl;
#endif
	  for (int i=0;i<nalias-1;i++){
	    in>>alias;
	    n->addAlias(alias);
	  }
	  char hasCell;
	  in>>hasCell;	  
	  in>>cellID;
	  int capabilities;
	  in>>capabilities;
	  //std::cout<<"read capabilities="<<capabilities<<std::endl;
	  n->setTransparent(transparent==mdescChar[MDTRANSPARENT]);
	  if (hasCell==mdescChar[MDHASCELL]){	    	    

	    //std::cout<<"mapping "<<path<<std::endl;

	    if (isCellFlagSet(n->getDataP(),VALUEPROXY)){
	      GenericValueProxy* vp=dynamic_cast<GenericValueProxy*>(n->getDataP());
	      proxies.push_back(vp);
	      cellIDs.push_back(cellID);
	      //onTree[vp]=n;

	      //std::cout<<"set the proxy "<<path<<std::endl;

	    }else{
	      //std::cout<<"no proxy waiting there"<<std::endl;	    
	      if (reconstructWhole&&n->getDataP()==NULL){
		Cell* cell=new CellHolder(new RemoteCell<UnspecifiedRemoteCellSignature>(c,cellID,capabilities));
	      n->setData(cell);
	      m.addForDeletion(cell);
	      }else{
		//std::cout<<"refusing to map on existing"<<std::endl;
	      }
	    }
	  }else{
	    //std::cout<<"remote has no cell for "<<path<<std::endl;
	  }
	}//oper=PUSH
	else if (oper==mdescChar[MDPOPID]){
	  path.upPath();
	}//
	else{
	  std::stringstream ss;
	  ss<<"Unexpected operator "<<oper<<" while parsing "<<TREEDESCRIPTIONLOCATION;
	  throw exc::InvalidOperation(ss.str());
	}
	
      }//while i
    }//readResult
  

  
  RemoteCellImporter::RemoteCellImporter(Memory& pm,CellClient& pc,const IDPath& ppath,
					 bool preconstructWhole
					 ):
    GenericRequest(location,&pc),location(TREEDESCRIPTIONLOCATION),m(pm),c(&pc),path(ppath),reconstructWhole(preconstructWhole){
      //std::cout<<"RemoteCellImporter,using"<<remotename<<std::endl;
    
      c->getAsciiPath(this); //the reply is managed by readResult
    }

    
 


  void RemoteCellImporter::connectProxy(GenericValueProxy* vp,const std::string& remoteName){
      Cell* cell=vp->getTypedRemote(c,remoteName);
      delList.addForDeletion(cell);

      
      /*IDPath path;
      onTree[vp]->getLocalPath(path);
      std::cout<<"dirty work for"<<path<<std::endl;*/

      vp->setRemote(cell);	 	        
    };


  void RemoteCellImporter::connectProxy(GenericValueProxy* vp){
      std::vector<GenericValueProxy*>::iterator it=std::find(proxies.begin(),proxies.end(),vp);
      if (it==proxies.end()) throw exc::InvalidOperation("Proxy to be connected has no remote match");
      int idx=it-proxies.begin();
      connectProxy(*it,cellIDs[idx]);
      proxies.erase(it);
      cellIDs.erase(cellIDs.begin()+idx);
    }

  void  RemoteCellImporter::connectProxies(){
      exc::MultipleExceptionHolder err;      
      for (size_t i=0;i<proxies.size();i++){
	try{
	  //std::cout<<"connecting proxy "<<cellIDs[i]<<std::endl;
	  connectProxy(proxies[i],cellIDs[i]);
	}catch(exc::Exception& e){
	  err.handle(e);
	}
      }
      cellIDs.clear();
      proxies.clear();
      err.check();
    }


}
