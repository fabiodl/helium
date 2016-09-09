#ifndef _HE_VECTORPROXY_
#define _HE_VECTORPROXY_

#include <helium/system/cellVector.h>
#include <helium/system/valueProxy.h>
#include <helium/system/cellSignature.h>
#include <helium/system/cellDescription.h>
namespace helium{

  template<typename T>
  class VectorIndividualProxy:
    public ValueProxy<CellVectorIndividualSignature<T> >
  {

  public:
    /*class Element{
      int id;
      VectorIndividualProxy& owner;
      Element operator=(Element& e){
	
      }
      operator const T&{
      }
      };*/

    VectorIndividualProxy():
      ValueProxy<CellVectorIndividualSignature<T> >(Void()),sizeCell(-1){}
    ValueProxy<DefaultSignature<size_t> > sizeCell;


    void resize(size_t n){
      sizeCell.set(n);
    }
    
    size_t size(){
      return sizeCell.get();
    }

    void get(std::vector<T>& t){
      t.resize(size());
      for (size_t i=0;i<t.size();i++){
	t[i]=ValueProxy<CellVectorIndividualSignature<T> >::parGet(i);
      }
    }

    const T operator[](int idx){
      return ValueProxy<CellVectorIndividualSignature<T> >::parGet(idx);
    }

    void set(int idx,T value){
       ValueProxy<CellVectorIndividualSignature<T> >::set(std::make_pair(idx,value));
    }


  };

  
  template<typename T> 
  class DefDescr<VectorIndividualProxy<T> >:public Description<VectorIndividualProxy<T> >{  
  public:
    DefDescr(VectorIndividualProxy<T>& v):Description<VectorIndividualProxy<T> >(v){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(scalarMapping(this->obj,""));
      members.push_back(scalarMapping(this->obj.sizeCell,"size"));
    }
  };



}


#endif
