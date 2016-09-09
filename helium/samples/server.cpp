#include <helium/system/basicCell.h>
#include <helium/system/cellServer.h>
#include <helium/system/signalledValue.h>
#include <helium/system/remoteCellExporter.h>

using namespace helium;
using namespace std;



class Cref:public ReferenceCell<int>{
public:
  SignalledValue<int> x;
  void connect(CellConnection& ref){
    x.connect(ref);
  }
  
  Cref():ReferenceCell<int>(x.val){

  }
  
};

int main(){
  
  Cref c;
  Memory mem;
  RemoteCellExporter rce(mem);
  mem.addChild("value")->setData(&c);
  
  CellServer server(2000,mem);
  int x;
  while(true){    
    cin>>x;
    c.x=x;
  }


}
