/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_CELLMAPPING_IMPL
#define HE_CELLMAPPING_IMPL

#include <helium/system/volatile.h>
#include <helium/system/valueProxy.h>
//#include <helium/system/proxiableCell.h>
#include <helium/system/cellClient.h>
#include <helium/core/isBaseOf.h>
#include <helium/system/cell.h>
#include <helium/system/basicCell.h>
#include <helium/destroy/autoDestroy.h>
#include <helium/destroy/deleteList.h>
#include <helium/system/indexableProxy.h>
namespace helium{

  namespace exc{
    class MissingPath:public Exception{
    public:
      

      MissingPath(const IDPath &p):Exception(toString(p)){
      }

      MissingPath(Exception &e):Exception(e){
      }

      void rethrow(){
	throw *this;
      }
      Exception* clone() const{
	return new MissingPath(*this);
      }
      
    };
  }

  //enum defined in NodeType

template <bool n,bool c>
  struct nodeMappingFlag{
  static const NodeType value = n?NODEMAPPING:(c?CELL:GENERIC);
};

 


  template <typename C,int flags,NodeType b=nodeMappingFlag<
				   is_base_or_same_of<NodeMapping,C>::value,
				   is_base_or_same_of<Cell,C>::value
			 >::value

	  > class RecursiveMap{
	  };


  
  


  template <typename GT,typename GP,typename ST,typename SP,typename UT,typename UP,bool GRP,bool SRP,bool CRP,typename ReplacementCell,int flags> 
  Cell*  fixCell(Memory& mem,const IDPath& p){
    IDNode<Cell>* n;
    if (flags&SILENTFAIL){
      size_t matched;
      n=mem.downMaximumPathNull(matched,p);
      if (n==NULL) return NULL;
    }else{
      //std::cout<<"Down the whole "<<p<<std::endl;
      n=mem.downPath(p);//throws exception      
    }
    Cell* cell=n->getDataP();    
    if (cell==NULL){
      if ((flags&SILENTFAIL)!=0){
	return NULL;
      }else{
	throw exc::MissingData(p);
      }
    }

    if (isCellFlagSet(cell,CELLHOLDER)){
      CellHolder* ch= dynamic_cast<CellHolder*> (cell);
      if (isCellFlagSet(ch->getRealCell(),TYPELESSREMOTECELL)){	
	RemoteCell<UnspecifiedRemoteCellSignature>* rc=dynamic_cast<RemoteCell<UnspecifiedRemoteCellSignature>*> (ch->getRealCell());
	Cell* nc=new RemoteCell<DefaultSignature<GT,GP,ST,SP,UT,UP,GRP,SRP,CRP> >(rc);
	ch->replaceCell(nc);
	//mem.addForDeletion(nc);   no need,deleted when deleting the holdercell
      }
      if (isCellFlagSet(ch->getRealCell(),SERIALIZEDDATACELL)){
	std::stringstream ss;
	cell->streamGet(ss);
	ReplacementCell* nc=new ReplacementCell();
	try{
	  nc->streamSet(ss);
	}catch(exc::InvalidDataValue& e){
	  throw exc::InvalidDataValue(toString(p),toString(e));
	}
	ch->replaceCell(nc);
      }
  
      return ch->getRealCell();
    }
    return cell;
  }
  

  template<typename C,int flags,bool remapCell=((flags&AVOIDCELLTRANSFORM)==0)> struct genericImport;

  template<typename C,int flags> struct genericImport<C,flags,true>{
    static void import(C& c,Memory& mem,const IDPath &path=IDPath()){
      //std::cout<<"true import"<<std::endl;
      Cell* cell=fixCell<C,Void,C,StdReply,Void,Void,false,false,false,DataCell<C>,flags >(mem,path);
      if ((flags&SILENTFAIL)==0||cell!=NULL){ //if it is not silent fail it MUST be there, we have no need to check if it is NULL
	cell->get(c);
      }           
    }
  };
  
  template<typename C,int flags> struct genericImport<C,flags,false>{
     static void import(C& c,Memory& mem,const IDPath &path=IDPath()){
       //std::cout<<"false import"<<std::endl;
       IDNode<Cell>* n;
       if (flags&SILENTFAIL){
	 size_t matched;
	 n=mem.downMaximumPathNull(matched,path);
	 if (n==NULL) return;
       }else{
	 n=mem.downPath(path);//throws exception      
       }
       Cell* cell=n->getDataP();    
       std::stringstream ss;
       cell->streamGet(ss);
       convert(c,ss.str());
     }
  };

  template<typename C,bool copyData> struct RefCellType{
    typedef ReferenceCell<C> value;
  };

  template<typename C,bool copyData> struct  RefCellType<const C,copyData>{
    typedef ConstReferenceCell<C> value;
  };


  template<typename C> struct RefCellType<C,true>{
    typedef DataCell<C> value;
  };

  template<typename C> struct RefCellType<const C,true>{
    typedef DataCell<C> value;
  };


template <typename C,int flags>
class RecursiveMap<C,flags,GENERIC>{
public:
  static void import(C& c,Memory& mem,const IDPath &path=IDPath()){
    //std::cout<<"generic import"<<std::endl;
    genericImport<C,flags>::import(c,mem,path);    
  }  


  static void exportRef(C& c,Memory& mem,const IDPath &path=IDPath()){
    try{
      Cell* cell=new typename RefCellType<C,(flags&USEDATACELL)!=0>::value(c);
      mem.assurePath(path)->setData(cell);
      mem.addForDeletion(cell);      
    }catch(exc::MissingData m){
      throw exc::MissingData(path);
    }
  }  
  /*static void exportData(C& c,Memory& mem,const IDPath &path=IDPath()){
    try{
      mem.assurePath(path)->setData(new ConstDataCell<C>(c));
    }catch(exc::MissingData m){
      throw exc::MissingData(path);
      }
      }    */
};//RecursiveMap<C,GENERIC>

template <typename C,int flags>
class RecursiveMap<C,flags,NODEMAPPING>{
public:
  static void import(C& c,Memory& mem,const IDPath &path=IDPath()){
    //std::cout<<"node mapping import"<<std::endl;
    c.import(mem,path);
  }  
  static void exportRef(C& c,Memory& mem,const IDPath &path=IDPath()){
    c.exportRef(mem,path);
  }  
  /*static void exportData(C& c,Memory& mem,const IDPath &path=IDPath()){
    c.exportData(mem,path);
    } */   
};//RecursiveMap<C,NODEMAPPING>


  /*  template<typename C,bool isProxiable=is_base_or_same_of<helium::ProxiableCell,C>::value >
  struct ProxiableCellAdapter{
    static void setVP(C&, GenericValueProxy*){
      throw exc::InvalidOperation("Attaching a non ProxiedCell over a ValueProxy");
    }
  };

  template<typename C>
  struct ProxiableCellAdapter<C,true>{
    static void setVP(C& c,GenericValueProxy* vp){
      c.setProxy(vp);
    }
  };
  */

  template<typename C,bool isProxy=is_base_or_same_of<helium::GenericValueProxy,C>::value>
  struct ProxyCellImporter{    
  };
  
  template<typename C>
  struct ProxyCellImporter<C,false>{    
    static void import(Cell& c,Memory& mem,const IDPath &path){
      std::stringstream ss;
      mem.downPath(path)->getData().streamGet(ss);
      c.streamSet(ss);  
    }
  };

  template<typename C>
  struct ProxyCellImporter<C,true>{    
    static void import(GenericValueProxy& c,Memory& mem,const IDPath &path){
      IDNode<Cell>* n=mem.assurePath(path);
      if (n->getDataP()==NULL){
	n->setData(&c);
      }else{
	c.setRemote(n->getDataP());
      }//getDataP!=NULL
    }//import
  };



  template<typename C,int flags>
  class RecursiveMap<C,flags,CELL>{
public:
  static void import(C& c,Memory& mem,const IDPath &path=IDPath()){        
    //std::cout<<"cell import"<<std::endl;
    ProxyCellImporter<C>::import(c,mem,path);
  }//import
  
  static void exportRef(C& c,Memory& mem,const IDPath &path=IDPath()){
    IDNode<Cell>* n=mem.assurePath(path);
    if (isCellFlagSet(n->getDataP(),VALUEPROXY)){
	GenericValueProxy* vp=dynamic_cast<GenericValueProxy*>(n->getDataP());
	vp->setRemote(&c);
	//ProxiableCellAdapter<C>::setVP(c,cell);	
    }else{
      n->setData(&c);      
    }
  }

  /*static void exportData(C& c,Memory& mem,const IDPath &path=IDPath()){
    typename C::gt v;
    c.get(v);
    mem.assurePath(path)->setData(new ConstDataCell<typename C::gt>(v));
    }*/

};//RecursiveMap<C,CELL>


  template<typename GT,typename GP,typename ST,typename SP,typename UT,typename UP,bool GRP,bool SRP,bool CRP,int flags>
  class RecursiveMap<Volatile<GT,GP,ST,SP,UT,UP,GRP,SRP,CRP>,flags,GENERIC>{
public:
    static void import(Volatile<GT,GP,ST,SP,UT,UP,GRP,SRP,CRP>& v,Memory& mem,const IDPath &path=IDPath()){
    v.setLink(
	      fixCell<GT,GP,ST,SP,UT,UP,GRP,SRP,CRP,DataCell<GT>,flags >(mem,path));
  }
  
    static void exportRef(Volatile<GT,GP,ST,SP,UT,UP>& v,Memory& mem,const IDPath &path=IDPath()){
    mem.assurePath(path)->setData(v.link);
  }

    /*static void exportData(Volatile<GT,GP,ST,SP,UT,UP>& v,Memory& mem,const IDPath &path=IDPath()){
    GT t;
    v.get(t);
    mem.assurePath(path)->setData(new ConstDataCell<GT>(t));
    }*/

}; //RecursiveMap<Volatile,GENERIC>


  template<int flags,typename C>
void recursiveMemToField(C &c,Memory& mem,const IDPath &path=IDPath()){
    RecursiveMap<C,flags>::import(c,mem,path);
}
  template<int flags,typename C>
void recursiveRefToMem(C &c,Memory& mem,const IDPath &path=IDPath()){
  RecursiveMap<C,flags>::exportRef(c,mem,path);
}

/*template<typename C>
void recursiveDataToMem(C &c,Memory& mem,const IDPath &path=IDPath()){
  RecursiveMap<C>::exportData(c,mem,path);
  }*/

  template <typename T,bool attach> struct Attacher{
    Attacher(T& t,Memory& mem,const IDPath& lp){
      Cell* c=new ReferenceCell<T>(t);
      mem.downPath(lp)->setData(c);
      //std::cout<<"attaching a"<<typeid(T).name()<<std::endl;
      mem.addForDeletion(c);
    }
  };



  template<bool b> struct referenceAttacher{
    template<typename T>
    static void attach(T& t,Memory& mem,const IDPath& lp){
      Cell* c=new ReferenceCell<T>(t);
      mem.assurePath(lp)->setData(c);    
      mem.addForDeletion(c);
    }
  };

  template<> struct referenceAttacher<false>{
    template<typename T>
    static void attach(T& t,Memory& mem,const IDPath& lp){
    }
  };

  
  template <typename T,int flags>
class ScalarMapping:public NodeMapping{
    T& t;
  GenericIDPath hop;


 public:
    ScalarMapping(T& pt,const GenericIDPath &phop):t(pt),hop(phop){
 }
      
  void import(Memory& mem,const IDPath &path=IDPath()){
    IDPath lp=path;
    try{
      lp.downPath(hop);
      recursiveMemToField<flags>(t,mem,lp);
      referenceAttacher<(flags&ATTACHELEMENT)!=0>::template attach(t,mem,lp);
    }catch(exc::UnknownID& m){
      throw exc::MissingPath(m);
    }
  }
      
  void exportRef(Memory& mem,const IDPath &path=IDPath()){
    IDPath lp=path;
    lp.downPath(hop);
    recursiveRefToMem<flags>(t,mem,lp);
  }
      
  /*void exportData(Memory& mem,const IDPath &path=IDPath()){	
	IDPath lp=path;
	lp.downPath(hop);
	recursiveDataToMem(t,mem,lp);
	}*/

      
};//ScalarMapping

 



  template <int flags,typename T>
  inline NodeMapping* fscalarMapping(T& t,const GenericIDPath &hop){
    return new ScalarMapping<T,flags>(t,hop);
  }


  template<typename T>
  class ResizerCallback:public Cell::CellCallback,public Cell::CellConnection,public ActionOnceNode<Cell*>{			 
    T& t;
  public:
    
    void act(Cell* c){
      delete this;
    }
    
    void operator()(Cell::CellSignalData d){
      t.resize(*trust_cast<const size_t*>(d));
      }
    ResizerCallback(T& pt):Cell::CellConnection(this),t(pt){
    }
    
    
  };


  template<bool b>
  struct conditionalResize{
    template<typename T> static void callResize(T& t,size_t n);
    //template<typename T> static void attachResizerCell(T& t,Memory& mem,const IDPath& p);
  };
  
  template<> template<typename T>
  void conditionalResize<true>::callResize(T& t,size_t n){
    //std::cout<<"calling resize "<<n<<std::endl;
    t.resize(n);
  }

  template<> template<typename T>
  void conditionalResize<false>::callResize(T& t,size_t n){
    //std::cout<<"seems not resizable"<<std::endl;
    //printTrace();
  }
  
  /*  
  template<typename T> 
  void conditionalResize<true>::attachResizerCell<T>(T& t,Memory& mem,const IDPath& p){
      IDNode<Cell>* num=mem.assurePath(p);
      if (num==NULL){
	Cell* c=new SizeCell<T>(t);
	num->setData(c);      
	mem.addForDeletion(c);
      }else{
	Cell& c=num->getData();
	ResizerCallback<T>* rc=new ResizerCallback<T>(t);
	c.connect(*rc);
	c.onDelete.push_front(rc);
      }
  }
  template<typename T> 
  void conditionalResize<false>::attachResizerCell<T>(T& t,Memory& mem,const IDPath& p){
  }
  */
 
  //bool dynamicSize,bool attachVector,bool attachElement
  template <typename T,int flags>
class VectMapping:public NodeMapping{
    T& t;
    GenericIDPath prefix;
    GenericIDPath numPath;
    GenericIDPath dataPath;
    bool silentFail;
    void import(Memory& mem,const IDPath &path=IDPath()){
      IDPath lp=path;
      lp.downPath(numPath);
      size_t num=0;

      try{
	recursiveMemToField<flags>(num,mem,lp);	
	conditionalResize<IsResizable<T>::value >::callResize(t,num);
	lp.upPath();
	//referenceAttacher<(flags&ATTACHVECTOR)!=0>::template attach(t,mem,lp);
      }catch(exc::UnknownID& m){
	throw exc::MissingPath(m);
      }
     

      //std::cout<<"resizing array to"<<num<<std::endl;
      //if (dynamicSize){
      //mem.downPath(path)->getData().connect(new ResizerCallback(t),true);
      //}
      IDPath dataBase=path;
      dataBase.downPath(prefix);      
     
      //std::cerr<<"SIZE MATCH"<<num<<" vs "<<t.size()<<std::endl;
     
      for (int i=0; i<(int)num;i++){
	lp=dataBase;
	lp.downPath(toString(i));		
	lp.downPath(dataPath);            	
	recursiveMemToField<flags>(t[i],mem,lp);	
	referenceAttacher<(flags&ATTACHELEMENT)!=0>::template attach(t[i],mem,lp);	  	
      }
    
    }//import

    void exportRef(Memory& mem,const IDPath &path=IDPath()){
      IDPath lp=path;
      lp.downPath(numPath);
      if (mem.assurePath(lp)->getDataP()!=NULL){
	size_t n;
	std::stringstream ss;
	mem.assurePath(lp)->getData().streamGet(static_cast<std::ostream&>(ss));
	read(n,ss);
	if (n!=t.size()){
	  throw exc::InvalidOperation(stringBegin()<<"Merging fields of two vectors with different size: existing "<<path<<" is "<< n<<" and should be replaced with "<<t.size()<<stringEnd());
	}
      }else{	
	
	Cell* cell=
	  flags&USEDATACELLFORSIZE?
	  static_cast<Cell*>(new DataCell<size_t>(t.size())):
	  static_cast<Cell*>(new SizeCell<typename realVector<T>::value_type,IsResizable<T>::value >(realVector<T>::get(t)));

	mem.downPath(lp)->setData(cell);     //assured by the check
	mem.addForDeletion(cell);
      }
      lp=path;
      IDPath dataBase=path;
      dataBase.downPath(prefix);
      for (size_t i=0; i<t.size();i++){
	lp=dataBase;
	lp.downPath(toString(i));
	//IDNode<Cell>* ln=mem.assurePath(lp);
	lp.downPath(dataPath);      
	recursiveRefToMem<flags>(t[(int)i],mem,lp);
	/*if (ln->getDataP()==NULL){
	  ln->setData(IDCELL());
	  }*/
      }
    }//import

    /*
    void exportData(Memory& mem,const IDPath &path=IDPath()){
      IDPath lp=path;
      lp.downPath(numPath);
      size_t num=t.size();
      recursiveDataToMem(num,mem,lp);
      IDPath dataBase=path;
      dataBase.downPath(prefix);      
      for (int i=0; i<(int)num;i++){
	lp=dataBase;
	lp.downPath(toString(i));	
	lp.downPath(dataPath);      
	recursiveDataToMem(t[i],mem,lp);
	lp.upPath();
      }
    }
    */
  public:
    VectMapping(T& pt,const GenericIDPath &pprefix,const GenericIDPath &pdataPath,const GenericIDPath &pnumPath):
      t(pt)
    {
    prefix.downPath(pprefix);
    numPath.downPath(pnumPath);
    dataPath.downPath(pdataPath);
      if (numPath.isEmpty()){
	numPath=prefix;
	numPath.downPath("num");
      }
  }

  };//VectMapping

 

  template <int flags,typename T>
  NodeMapping* fvectMapping(T& t,const GenericIDPath &prefix,const GenericIDPath &dataPath,const GenericIDPath &numPath){ 
    return new VectMapping<T,flags>(t,prefix,dataPath,numPath);
  }//vectMapping


  


  //bool attachVector,bool attachElement
  template <typename T,int flags,typename Descriptor=typename T::mapped_type& > class MapMapping:public NodeMapping{
    T& m;
    GenericIDPath prefix;
    //GenericIDPath keyPath;
    GenericIDPath valuePath;    


  
  public:
    MapMapping(T& pm,const GenericIDPath &pprefix,//const GenericIDPath &pkeyPath,
	       const GenericIDPath &pvaluePath):
      m(pm),prefix(pprefix),valuePath(pvaluePath){
    }
    
    void import(Memory& mem,const IDPath &path){
      IDPath base=path;
      base.downPath(prefix);
      referenceAttacher<(flags&ATTACHVECTOR)!=0>::template attach(m,mem,base);

      //std::cout<<"base is"<<base;
      try{
	IDNode<Cell>* n;
	if (flags&SILENTFAIL){
	  size_t matched;
	  n=mem.downMaximumPathNull(matched,base);
	  if (n==NULL) return;
	}else{
	  n=mem.downPath(base);//trows exception
	  //std::cout<<"gone down to "<<base<<std::endl;
	}


	for (size_t i=0;i<n->childrenNum();i++){
	  std::string key;
	  n->getChild(i)->getMainAlias(key);
	  IDPath lp=base;
	  lp.downPath(key).downPath(valuePath);
	  typename T::mapped_type mt;
	  //std::cout<<"importing "<<lp<<" to "<<key<<std::endl;
	  try{
	    Descriptor des(mt);
	    recursiveMemToField<flags>(des,mem,lp);	
	    //std::cout<<"retrieved value ";
	    //write(std::cout,mt);
	    //std::cout<<" for key "<<key<<std::endl;
	    m.insert(make_pair(key,mt));
	    referenceAttacher<(flags&ATTACHELEMENT)!=0>::template attach(m.find(key)->second,mem,lp);
	  }catch(exc::UnknownID &m){
	    throw exc::MissingPath(m);
	  }
	}
      }catch(exc::MissingData &m){
	throw exc::MissingPath(m);
      }
    }

    void exportRef(Memory& mem,const IDPath &path){
      IDPath base=path;
      base.downPath(prefix);
      for (typename T::iterator it=m.begin();it!=m.end();it++){
	IDPath lp=base;
	lp.downPath(it->first).downPath(valuePath);
	Descriptor des(it->second);
	recursiveRefToMem<flags>(des,mem,lp);
      }
    }

  };

  template<int flags,typename T,typename Descriptor>
  inline NodeMapping* ffmapMapping(T& t,const GenericIDPath &prefix,const GenericIDPath &valuePath){
    //return new MapMapping<T>(t,prefix,keyPath,valuePath);
    return new MapMapping<T,flags,Descriptor>(t,prefix,valuePath);
  }


 template<int flags>
 inline void FClassMapping<flags>::genericTransfer(Memory& mem,const IDPath &path,void (*mapf)(NodeMapping &,Memory&,const IDPath&)){
   IDPath lp=path;      
   std::vector<NodeMapping*> members;
   GenericIDPath hopname;
   this->getMapMembers(hopname,members);
   if (hopname.size()!=0)
     lp.downPath(hopname);
   for (size_t i=0;i<members.size();i++){
     mapf(*members[i],mem,lp);
   }
   onInitializationFinished(mem);
   destroyS(members);
   
 }

 template<int flags>
 inline void FClassMapping<flags>::import(Memory& mem,const IDPath &path){
   //std::cout<<"Importing "<<path<<" with flags "<<flags<<std::endl;
   genericTransfer(mem,path,recursiveMemToField<flags>);
 }
 template<int flags>
inline void FClassMapping<flags>::exportRef(Memory& mem,const IDPath &path){
   genericTransfer(mem,path,recursiveRefToMem<flags>);
 }
 
  template<int flags>
  inline void import(NodeMapping* n,Memory& mem,const IDPath &path){
    IDPath lp=path;
    recursiveMemToField<flags>(*n,mem,path);
    delete n ;
  }
  template<int flags>
  inline void exportRef(NodeMapping* n,Memory& mem,const IDPath &path){
    IDPath lp=path;
    recursiveRefToMem<flags>(*n,mem,path);
    delete n ;
  }
/*inline void ClassMapping::exportData(Memory& mem,const IDPath &path){
   genericTransfer(mem,path,recursiveDataToMem);
   }*/

}//helium
#endif
