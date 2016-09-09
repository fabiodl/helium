/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_REMOTECELLIMPORTER
#define HE_REMOTECELLIMPORTER

#include <helium/system/cellClient.h>
#include <helium/system/remoteCell.h>
#include <helium/system/valueProxy.h>
//#include <helium/vector/destroy.h>
#include <helium/system/basicCell.h>
namespace helium{
  
  class RemoteCellImporter:public CellClient::GenericRequest{
    std::string location;
    Memory &m;
    CellClient* c;
    std::vector<GenericValueProxy*> proxies;
    std::vector<std::string> cellIDs;    
    DeleteList<Cell> delList;
    IDPath path;
    void readResult(std::istream& in);
    void connectProxy(GenericValueProxy* vp,const std::string& remoteName);
    bool reconstructWhole;
  public:  
    RemoteCellImporter(Memory& pm,CellClient& pc,const IDPath& ppath=IDPath(),bool reconstructWhole=false);//cells are deleted when the importer is destroyed

    void connectProxy(GenericValueProxy* vp);
    void connectProxies();

    
    //no need,done by the deleter
    /*~RemoteCellImporter(){
      //printTrace();
      //for (size_t i=0;i<proxies.size();i++){
      //proxies[i]->unsetRemote(); 
      //}	
      //std::cout<<delList<<std::endl;


      }*/


  };
}//helium


#endif
