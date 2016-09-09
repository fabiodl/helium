/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_HEXMAPPING
#define HE_HEXMAPPING

#include <helium/system/cellMapping.h>
#include <helium/system/basicCell.h>
namespace helium{

  template<typename T,int flags>
  class HexMapping:public NodeMapping{
    T& t;
    GenericIDPath hop;
  public:    
    HexMapping(T& pt,const GenericIDPath& phop):t(pt),hop(phop){
    }
    
    void import(Memory& mem,const IDPath &path=IDPath()){
      IDPath lpath=path;
      lpath.downPath(hop);
      try{
	fixCell<T,Void,T,StdReply,Void,Void,false,false,false,HexDataCell<T>,flags >(mem,lpath)->get(t);      
      }catch(exc::MissingData m){	
	throw exc::MissingData(path);
      }
    }

    void exportRef(Memory& mem,const IDPath &path=IDPath()){      
      IDPath lpath=path;
      lpath.downPath(hop);
      Cell* cell=new HexReferenceCell<T>(t);
       mem.assurePath(lpath)->setData(cell);
      mem.addForDeletion(cell);      
    }  
    
  };


  template<int flags,typename T> 
  inline NodeMapping* fhexMapping(T& t,const GenericIDPath& hop=GenericIDPath()){
    return new HexMapping<T,flags>(t,hop);
  }

  
  template<typename T> 
  inline NodeMapping* hexMapping(T& t,const GenericIDPath& hop=GenericIDPath()){
    return fhexMapping<NOFLAGS,T>(t,hop);
  }
  

}

#endif
