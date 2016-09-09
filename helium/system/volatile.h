/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_VOLATILE
#define HE_VOLATILE

#include <helium/system/cell.h>
#include <helium/core/void.h>


namespace helium{
  enum NodeType{GENERIC,NODEMAPPING,CELL};
  template<typename C,int flags,NodeType> class RecursiveMap;//forward declaration for friendship

  template <typename GT,typename GP,typename ST,typename SP,typename UT,typename UP,
	    bool GRP,bool SRP,bool CRP
	    >
  std::ostream & operator<<(std::ostream & o,const Volatile<GT,GP,ST,SP,UT,UP,GRP,SRP,CRP> &v);//forward declaration for friendship

  template<typename GT>
  struct Getter{
    virtual GT& get()=0;
  };

  template<typename GT>
  class GenericGetter:public Getter<GT>{
    Cell* link;
    GT t;
  public:
    GenericGetter(Cell* c):link(c){
    }
    inline GT& get(){
      link->get(t);
      return t;
    }
    
  };//class GenericGetter

  template<typename GT>
  class PointerGetter:public Getter<GT>{
    GT* t;
  public:
    PointerGetter(void* v){
      t=trust_cast<GT*>(v);
    }

    inline GT& get(){
      //std::cout<<"using the pointer shorcut"<<std::endl;
      return *t;
    }

  };//class PointerGetter
  
  

  template <typename GT,typename GP=Void,typename ST=GT,typename SP=StdReply,typename UT=GT,typename UP=Void,
	    bool getRequiresParam=false, bool setRequiresParam=false, bool connectRequiresParam=false
	    >
    class Volatile{
    Cell* link;
    Getter<GT>* getter;
    template <typename C,int type,NodeType>
      friend class RecursiveMap;
    friend std::ostream & operator<< <GT,GP,ST,SP,UT,UP,getRequiresParam,setRequiresParam,connectRequiresParam>(std::ostream & o,const Volatile &v);
    
    void setLink(Cell* plink){
      link=plink;
      if (link->getProperties()!=NULL&&link->getProperties()->directValuePointer()!=NULL){
	getter=new PointerGetter<GT>(link->getProperties()->directValuePointer());
      }else{
	getter=new GenericGetter<GT>(link);
      }	
    }

  public:    
    typedef GT gt;
    typedef GP gp;
    typedef ST st;
    typedef SP sp;
    typedef UT ut;
    typedef UP up;


    inline GT& get(){      
      return getter->get();
    }
    inline void get(GT& t,const GP &p){
      link->get(t,p);
    }
    inline void set(const ST& t){
      link->set(t);
    }
    inline void set(SP&p,const ST& t){
      link->set(p,t);
    }
    
    inline const ST& operator=(const ST& t){
      link->set(t);
      return t;
    }
    

    template <typename C>
    class Connection:
    public Cell::CellConnection
    {
      struct CB:public Callback<const DataP>{
	C* c;
	void (C::*m)(const UT&);
	void operator()(DataP d){
	  (c->*m)( *(trust_cast<UT*>(d)));
	}
	Callback<const DataP>* clone() const{
	  return new CB(c,m);
	}       
    CB(C* pc,void (C::*pm)(const UT&)):c(pc),m(pm){
	}

      };//CB
    public:
    Connection(C* pc,void (C::*pm)(const UT&)):
      Cell::CellConnection(CB(pc,pm).clone())
      {
	
      }



    ~Connection(){
      delete cb;
      if (disc!=NULL){
	std::cout<<"Bug, this is"<<this<< "with disc "<<disc<<std::endl;
	throw exc::Bug("Connection to volatile destroyed before disconnection");
      }
    }

    };//Connection

    
    void connect(Cell::CellConnection &c){
      //std::cout<<"connecting "<<&c<<std::endl;
      link->connect(c);
      //std::cout<<"disc is "<<c.disc<<std::endl;
    }

    
    
    void connect(Cell::CellConnection &c,const UP &p){
      //std::cout<<"paramconnecting "<<&c<<std::endl;
	link->connect(c,(const DataP)&p);
	//std::cout<<"disc is "<<c.disc<<std::endl;
    }


    void disconnect(Cell::CellConnection &r){
      //std::cout<<"disconnecting "<<&r<<std::endl;
      link->disconnect(r);
      //std::cout<<"disc is "<<r.disc<<std::endl;
    }
    
    Volatile():link(NULL),getter(NULL){
    }

    ~Volatile(){
      delete getter;
    }
  };//class Volatile



}//helium

#endif
