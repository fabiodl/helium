#ifndef HE_DYNAMICSERVER
#define HE_DYNAMICSERVER

#include <helium/system/remoteCellExporter.h>
#include <helium/system/cellServer.h>
#include <helium/system/helpCell.h>
namespace helium{

  class DynamicServer:public CellServer{
  protected:
    Memory mem;
    RemoteCellExporter tree;
    TreeHelp help;


  public:
    
    template<typename T> 
    void share(T& t,const IDPath& p){
      dexport(mem,t,p);
    }
    
    DynamicServer(int port,bool autoStart):CellServer(port,mem,autoStart),
			    tree(mem),
			    help(mem)
    {
      dexport(mem,help);
    }
    
  };


};


#endif
