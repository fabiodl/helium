#ifndef HE_DYNAMICCLIENT
#define HE_DYNAMICCLIENT

#include <helium/system/remoteCellImporter.h>
#include <helium/system/cellDescription.h>
#include <memory>

namespace helium{
  class DynamicClient:public CellClient{
  protected:
    Memory mem;
  private:
    std::auto_ptr<RemoteCellImporter> rci;
    friend class ConnectListener;
    friend class DisconnectListener;
    
  class ConnectListener:public Callback<void>{
    DynamicClient& owner;    
    void operator()(){
      //std::cout<<"here"<<std::endl;
      owner.rci.reset(new RemoteCellImporter(owner.mem,owner));
      //std::cout<<"here"<<std::endl;
      //std::cout<<owner.mem<<std::endl;
    }
  public:
    ConnectListener(DynamicClient& powner):
      owner(powner){}
  } connListener;
    
    class DisconnectListener: public Callback<void>{
      DynamicClient& owner;    
      void operator()(){
	owner.rci.reset(NULL);
      }
      
    public:
      DisconnectListener(DynamicClient& powner):
	owner(powner){  }
    } discListener;
  public:
    Memory& getMem(){
      return mem;
    }
    
    DynamicClient():connListener(*this),
		    discListener(*this){
      onConnect.connect(&connListener);
      onDisconnect.connect(&discListener);
    }
       
  };//DynamicClient
  
}


#endif
