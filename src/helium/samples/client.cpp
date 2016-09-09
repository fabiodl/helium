#include <helium/system/remoteCellImporter.h>

using namespace helium;

struct Data{
  GtkProxy<int> x;
  GtkProxy<Plan> plan;

};

template<>
class DefDescr<Data>{

  DefDescr(Data& d):Description<Data>(d){}
  
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(d(obj.x),"value"));
  }
}
;


struct Change{

  void changed(int x){
    cout<<x<<endl;
  }
};

int main(){
  Memory mem;
  Data data;
  dexport(mem,data);
  CellClient c;
  c.connect("localhost",5001);
  RemoteCellImporter rci(mem,c);
 
  Change change;
  data.x.connect<Change,&Change::changed>(&change); 
  
  Plan plan[10];


  data.plan=plan[3];
  data.motor[0]

  data.motor[1].mode="headTracking";

}
