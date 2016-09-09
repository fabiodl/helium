
#include <helium/vector/array.h>
#include <helium/util/ioUtil.h>

#include <iostream>
using namespace std;
using namespace helium;

class A{
  char x;
  int y;
  double z;
  friend std::ostream& operator<<(std::ostream& o,const A& a);
public:
  A(char px,int py,double pz):
    x(px),y(py),z(pz){
    cout<<"cted "<<x<<","<<y<<","<<z<<endl;    
  }

};



class B{
public:
  B(double x){
    cout<<"cted B "<<x<<endl;    
  };
};


std::ostream& operator<<(std::ostream& o,const A& a){
  return o<<a.x<<","<<a.y<<","<<a.z;;
}

class C{

  void allocA(A* a,int i){
    new (a) A('0'+i,i*2,i*3);//placement new
  }

  Array<A, //the type of objects in the array
	array::RESIZABLE, //how many, a constant number or RESIZABLE
	array::InternalAllocator< //Store the allocator data in the array
	  ClassPlacerAllocator<A, //the objects to construct
			       C, //the class that can construct the objects
			       &C::allocA //the function that can construct the objects
			       > 
	  > 
	> arr;

  Array<B,
	array::RESIZABLE,
	array::InternalAllocator<
	  ParamAllocator<B, //the class to construct
			 double //the parameter type
			 >
	  >
	>  arrAllSame;
			 

public:
  
  C():arr(this,4),arrAllSame(3.14,3) //pass the pointer to the class that constructs the objects,and the number of objects to construct
      
  {    
    cout<<arr<<endl;
    
  }


};



int main(){
  C c;
  
}
