#ifndef _CELLARRAY_
#define _CELLARRAY_


#include <helium/system/cellSignature.h>
#include <helium/system/cellSignal.h>
#include <helium/signal/wrapInfo.h>
#include <helium/system/basicCell.h>
#include <helium/system/cellDescription.h>

//The companion class is found in <helium/system/vectorProxy.h>



namespace helium{

  
  template<typename T>
  struct CellVectorIndividualSignature:public CellSignature{
    typedef T GT;
    typedef int GP;
    typedef std::pair<int,T> ST;
    typedef Void SP;
    static const bool getRequiresParam=true;
    typedef T UT;
    typedef int UP;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=true;
  };

  


  template <typename T> class CellVectorIndividual:
    public TypedCell<CellVectorIndividualSignature<T> >{
 
  public:
    class Element{
      T data;
      CellSignal<T,true,false> cellSignal;
      typename SignalType<T,true,false>::value signal;
    public:
      Element(){}
      Element(const Element& e):data(e.data){}
      Element& operator=(const Element& v){
	data=v.data;
	return *this;
      }

      
      Element& operator=(const T& v){
	data=v;
	signal(v);
	cellSignal(v);
	return *this;
      }
      
      operator const T& (){
	return data;
      }


      void connect(ConnReference<T>& cr){signal.connect(cr);}
      void disconnect(ConnReference<T>& cr){signal.disconnect(cr);}
     
      void cellConnect(Cell::CellConnection& cr){cellSignal.connect(cr);}
      void cellDisconnect(Cell::CellConnection&  cr){cellSignal.disconnect(cr);}
      
    };



    
    CellVectorIndividual(int n=0):sizeCell(data),data(n){}

    SizeCell<std::vector<Element> > sizeCell;

    Element& operator[](int idx){
      return data[idx];
    }

    size_t size(){
      return data.size();
    }

    void resize(size_t n){
      sizeCell.set(n);
    }

    
    std::string getName() const{
      return Typename<T>::name+" vector cell";
    }
    
    void getData(T& v,const int& idx){
      v=data[idx];
    }


    void setData(const std::pair<int,T>& v){
      data[v.first]=v.second;
    }
    
    
    void connect(Cell::CellConnection& ref,const int& idx){
      data[idx].cellConnect(ref);
      wrapInfo(ref,idx);
    }

    void disconnect(Cell::CellConnection& ref){
      int idx;
      unwrapInfo(ref,idx);
      data[idx].cellDisconnect(ref);
    }           
    
    void operator=(const std::vector<T>& src){
      if (data.size()!=src.size()){
	resize(src.size());
      }
      for (size_t i=0;i<src.size();i++){
	data[i]=src[i];
      }
    }
    
  private:
    std::vector<Element> data;

  };

 template<typename T> 
  class DefDescr<CellVectorIndividual<T> >:public Description<CellVectorIndividual<T> >{  
  public:
    DefDescr(CellVectorIndividual<T>& v):Description<CellVectorIndividual<T> >(v){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(scalarMapping(this->obj,""));
      members.push_back(scalarMapping(this->obj.sizeCell,"size"));
    }
  };


}

#endif
