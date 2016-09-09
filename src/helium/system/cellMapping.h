/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_CELLMAPPING
#define HE_CELLMAPPING

#include <helium/system/cell.h>

namespace helium{


class NodeMapping{

public:  
  virtual void import(Memory& mem,const IDPath &path)=0;
  virtual void exportRef(Memory& mem,const IDPath &path)=0;
  //virtual void exportData(Memory& mem,const IDPath &path)=0;
  virtual ~NodeMapping(){}//assure deconstruction of subclasses
};

  enum MappingFlags{NOFLAGS,DYNAMICSIZE=1,ATTACHVECTOR=2,ATTACHELEMENT=4,SILENTFAIL=8,AVOIDCELLTRANSFORM=16,USEDATACELL=32,USEDATACELLFORSIZE=64};



  template <int flags,typename T>
  inline NodeMapping* fscalarMapping(T& t,const GenericIDPath &hop);
  template <typename T>
  inline NodeMapping* scalarMapping(T& t,const GenericIDPath &hop){
    return fscalarMapping<NOFLAGS,T>(t,hop);
  }
  

  template <int flags,typename T> //put <true/false>
  inline NodeMapping* fvectMapping(T& t,const GenericIDPath &prefix=GenericIDPath(),const GenericIDPath &dataPath=GenericIDPath(),const GenericIDPath &numPath=GenericIDPath());


  template <typename T>
  inline NodeMapping* vectMapping(T& t,const GenericIDPath &prefix=GenericIDPath(),const GenericIDPath &dataPath=GenericIDPath(),const GenericIDPath &numPath=GenericIDPath()){
    return fvectMapping<NOFLAGS,T>(t,prefix,dataPath,numPath);
    
  }



  template<int flags,typename T>
  inline NodeMapping* fpsvectMapping(T& t,				    
				    const GenericIDPath &plural=GenericIDPath(),
				    const GenericIDPath &singular=GenericIDPath(),
				    const GenericIDPath &prefix=GenericIDPath()
				    ){
    IDPath p=prefix;
    p.downPath(plural).downPath(singular);
    IDPath n=prefix;
    n.downPath(plural).downPath("num");
    return fvectMapping<flags,T>(t,p,"",n);    
  }

  template<typename T>
  inline NodeMapping* psvectMapping(T& t,				    
				    const GenericIDPath &plural=GenericIDPath(),
				    const GenericIDPath &singular=GenericIDPath(),
				    const GenericIDPath &prefix=GenericIDPath()
				    ){
    return fpsvectMapping<NOFLAGS,T>(t,plural,singular,prefix);
  }

  template <int flags,typename T,typename Descriptor>
  inline NodeMapping* ffmapMapping(T& t,const GenericIDPath &prefix=GenericIDPath(),const GenericIDPath &valuePath=GenericIDPath());

  template <int flags,typename T>
  inline NodeMapping* fmapMapping(T& t,const GenericIDPath &prefix=GenericIDPath(),const GenericIDPath &valuePath=GenericIDPath()){
    return ffmapMapping<flags,T,typename T::mapped_type &>(t,prefix,valuePath);
  }

  template <typename T>
  inline NodeMapping* mapMapping(T& t,const GenericIDPath &prefix=GenericIDPath(),const GenericIDPath &valuePath=GenericIDPath()){
    return ffmapMapping<NOFLAGS,T,typename T::mapped_type&>(t,prefix,valuePath);
  }

  template<int flags>
class FClassMapping:public NodeMapping{
  
  void genericTransfer(Memory& mem,const IDPath &path,void (*mapf)(NodeMapping &,Memory&,const IDPath&));
  public:
  virtual void getMapMembers(GenericIDPath &hopName,std::vector<NodeMapping*> &members)=0;        
    inline void import(Memory& mem,const IDPath &path=IDPath());    
    inline void exportRef(Memory& mem,const IDPath &path=IDPath());
    inline void exportData(Memory& mem,const IDPath &path=IDPath());
    virtual void onInitializationFinished(Memory& mem){}
  };

  template<int flags>
  void import(NodeMapping* n,Memory& mem,const IDPath &path=IDPath());

  template<int flags>
  void exportRef(NodeMapping* n,Memory& mem,const IDPath &path=IDPath());


  typedef FClassMapping<0> ClassMapping;

}//heliim

#include "cellMapping.hpp"

#endif
