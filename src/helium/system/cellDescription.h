/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_CELLDESCRIPTION
#define HE_CELLDESCRIPTION

#include <helium/core/deletable.h>
#include <helium/system/cellMapping.h>
#include <helium/core/isSpecialized.h>


namespace helium{

  template <typename T> class DefDescr;
  template <typename T,int flags> class FDefDescr;
  
  template<typename A,typename ElemDesc=DefDescr<typename A::value_type>,bool resiz=IsResizable<A>::value >
  class VectorDesc;


  //A is the "array-like" object,
  //ElemDesc is the object used for its description
  //IndexableProxy is an interface

  template<typename A,typename ElemDesc>
  class VectorDesc<A,ElemDesc,false>:public Deletable,public IndexableProxy<A>{  
  protected:
    typedef A value_type;
    typedef VectAllocator<ElemDesc,A> Alloc;

    Alloc alloc;
   
    Array<ElemDesc,array::RESIZABLE, Alloc> d;
  public:
    VectorDesc(A& pt):IndexableProxy<A>(pt),alloc(pt),d(alloc,pt.size()){
      //std::cout<<"new vectordesc"<<pt.size()<<" vs "<<d.size()<<std::endl;
    }
    
    //the interface would require just a NodeMapping& 
    
    //NodeMapping& operator[](int i){ 
    ElemDesc& operator[](int i){ 
      return d[i];
    }
    
     size_t size(){
      return this->t.size();
      
    }
    
  };

  
  template<typename A,typename ElemDesc>
    class VectorDesc<A,ElemDesc,true>:public VectorDesc<A,ElemDesc,false>{
  public:
    void resize(size_t n){
      this->t.resize(n);
      this->d.resize(n);
    }
   

  VectorDesc(A& pt):VectorDesc<A,ElemDesc,false>(pt){
    }
    
  };

 

  template<typename T> class Description:public FClassMapping<NOFLAGS>,public Deletable{
  protected:
    T& obj;
    typedef T value_type;
    std::vector<Deletable*> ds;



  public:



    template <typename Inner> Inner& desc(typename Inner::value_type& obj){
      Inner* i=new Inner(obj);
      ds.push_back(i);
      return *i;      
    }

    template<typename E> NodeMapping& d(E& e);
    template<typename E> NodeMapping* dMapping(E& e);
    template<int flags,typename E> NodeMapping& fd(E& e);
    template<typename D,typename E> NodeMapping& cd(E& e);

    template<typename A> IndexableProxy<A>& vd(A& a);
    template<typename D,typename A> IndexableProxy<A>& cvd(A& a);

    template <typename Elem,typename A> VectorDesc<A,Elem>& vdesc(A& vobj){
      VectorDesc<A,Elem>* vd=new VectorDesc<A,Elem>(vobj);
      ds.push_back(vd);
      return *vd;
    }


    
    Description(T& pobj):obj(pobj){
      //std::cout<<"new description @"<<this<<std::endl;
      //double* n=new double[10000];
    }

    virtual ~Description(){
      for (size_t i=0;i<ds.size();i++) delete ds[i];
      //std::cout<<"description @"<<this<<"gone"<<std::endl;
    }


  };

  template<typename T>
  inline NodeMapping* dmapMapping(T& t,const GenericIDPath &prefix=GenericIDPath(),const GenericIDPath &valuePath=GenericIDPath()){
    return ffmapMapping<0,T,DefDescr<typename T::mapped_type> >(t,prefix,valuePath);
  }



  template<typename T>
  void dimport(T& obj,Memory& mem,const IDPath &path=IDPath()){
    DefDescr<T> d(obj);
    d.import(mem,path);
  }

  template<typename T>
  T dget(Memory& mem,const IDPath &path=IDPath()){
    T t;
    dimport(t,mem,path);
    return t;
  }


  template<typename D,typename T>
  void cimport(T& obj,Memory& mem,const IDPath &path=IDPath()){
    D d(obj);
    d.import(mem,path);
  }


  template<int flags,typename T>
  void fdimport(T& obj,Memory& mem,const IDPath &path=IDPath()){
    typename FDefDescr<T,flags>::value d(obj);
    helium::recursiveMemToField<flags>(d,mem,path);
  }


  template<int flags,typename D,typename T>
  void fcimport(T& obj,Memory& mem,const IDPath &path=IDPath()){
    D d(obj);
    helium::recursiveMemToField<flags>(d,mem,path);
  }


  template<typename T>
  void dexport(Memory& mem,T& obj,const IDPath &path=IDPath()){
    DefDescr<T> d(obj);
    d.exportRef(mem,path);
  }
  
  template<int flags,typename T>
  void fdexport(Memory& mem,T& obj,const IDPath &path=IDPath()){
    typename FDefDescr<T,flags>::value d(obj);
    helium::recursiveRefToMem<flags>(d,mem,path);
  }


  template<typename D,typename T>
  void cexport(Memory& mem,T& obj,const IDPath &path=IDPath()){
    D d(obj);
    d.exportRef(mem,path);
  }


  //}


template<typename T> class DefDescr:public helium::Deletable,public helium::NodeMapping{
  T& obj;
  friend void helium::dimport<T>(T& obj,Memory& mem,const IDPath &path);
  friend void helium::dexport<T>(Memory& mem,T& obj,const IDPath &path);
public:
  void import(helium::Memory& mem,const helium::IDPath &path){
    helium::recursiveMemToField<NOFLAGS>(obj,mem,path);
  }
  void exportRef(helium::Memory& mem,const helium::IDPath &path){
    helium::recursiveRefToMem<NOFLAGS>(obj,mem,path);
    }

    DefDescr(T& pobj):obj(pobj){
      //std::cout<<"new default description"<<std::endl;
      //printTrace();
    }
};



  template<typename T,int flags>

  class FDefDescr:public helium::Deletable,public helium::NodeMapping{
  T& obj;
    friend void helium::dimport<T>(T& obj,Memory& mem,const IDPath &path);
    friend void helium::dexport<T>(Memory& mem,T& obj,const IDPath &path);
  public:

    typedef typename selectFirstOnTrue<FDefDescr<T,flags>,DefDescr<T>,isSpecialized<DefDescr<T> >::value>::value value;

    void import(helium::Memory& mem,const helium::IDPath &path){
      helium::recursiveMemToField<flags>(obj,mem,path);
    }
    void exportRef(helium::Memory& mem,const helium::IDPath &path){
      helium::recursiveRefToMem<flags>(obj,mem,path);
    }    
    FDefDescr(T& pobj):obj(pobj){
    }
};



  //namespace helium{

  template<typename T> template <typename E> NodeMapping& Description<T>::d(E& e){
    DefDescr<E>* dd=new  DefDescr<E>(e);
    //std::cout<<"this is "<<this<<" and dd is"<<dd<<std::endl;
    ds.push_back(dd);
    return *dd;
  }

  template<typename T> template <typename E> NodeMapping* Description<T>::dMapping(E& e){
    return new DefDescr<E>(e);
  }

  template<typename T> template <typename D,typename E> NodeMapping& Description<T>::cd(E& e){
    D* dd=new  D(e);
    ds.push_back(dd);
    return *dd;
  }

  template<typename T> template <int flags,typename E> NodeMapping& Description<T>::fd(E& e){
    FDefDescr<E,flags>* dd=new  FDefDescr<E,flags>(e);
    ds.push_back(dd);
    return *dd;
  }

  template<typename T> template<typename A> IndexableProxy<A>& Description<T>::vd(A& a){
    VectorDesc<A,DefDescr<typename A::value_type> >* ad=new VectorDesc<A,DefDescr<typename A::value_type> >(a);
    ds.push_back(ad);
    return *ad;
  }

  template<typename T> template<typename D,typename A> IndexableProxy<A>& Description<T>::cvd(A& a){
    VectorDesc<A,D>* ad=new VectorDesc<A,D>(a);
    ds.push_back(ad);
    return *ad;
  }
  
  template <typename T>
  inline std::istream& operator>>(std::istream& i,Description<T> &e){
    return i>>e.obj;
  }
  
  template <typename T>
  inline std::ostream& operator<<(std::ostream& o,const Description<T> &e){
    return o<<e.obj;
  }
    
 
#define DECLARE_DEFAULT_DESCRIPTION(t) template<> class DefDescr<t >:public Description<t >{ \
  public:								\
  DefDescr<t >(t &obj ) :Description<t >(obj){}				\
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);  };									



#define DECLARE_DEFAULT_DESCRIPTION_WITH_ONFINISHED(t) template<> class DefDescr<t >:public Description<t >{ \
  public:								\
  DefDescr<t >(t &obj ) :Description<t >(obj){}				\
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);  \
  void onInitializationFinished(helium::Memory& mem); };									


#define DECLARE_CUSTOM_DESCRIPTION(t,c)  class c:public Description<t >{ \
  public:								\
  c(t &obj ) :Description<t >(obj){}					\
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members); }; 






}//helium

#endif
