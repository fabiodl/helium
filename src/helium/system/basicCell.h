/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */


#ifndef HE_BASICCELL_
#define HE_BASICCELL_



#include <helium/system/cell.h>
#include <helium/core/exception.h>
#include <helium/core/typename.h>
#include <helium/util/ioUtil.h>
#include <helium/system/cellSignature.h>


#include <sstream>


#ifdef _WIN32
#pragma warning(disable: 4250)
#endif


namespace helium{



  class DummyCell:public Cell{
    std::string getName() const{ 
      return "DummyCell";
    }
  };

  class IDCell:public DummyCell{
    std::string getName() const{
      return "IDCell";
    }
    void streamGet(std::ostream &o){o<<"id";};
  };
 
 template <typename GT,typename GP,typename ST,typename SP,typename UT,typename UP,bool getRequiresParam, bool setRequiresParam,bool connectRequiresParam>
  struct TypedCapabilities{
    
    static const int value=
      ((VoidToTrue<GT>::value||getRequiresParam)?0:(CELLGET|CELLGETS)                )|
      (VoidToTrue<GP>::value?0:(CELLGETP|CELLGETPS)              )|
      ((VoidToTrue<ST>::value||setRequiresParam)?0:(CELLSET|CELLSETS)                )|
      (VoidToTrue<SP>::value?0:(CELLSETP|CELLSETPS)              )|
      ((VoidToTrue<UT>::value||connectRequiresParam)?0: CELLCONN|CELLDISC|CELLFORMATDS   )|
      (VoidToTrue<UP>::value?0:(CELLCONNP|CELLCONNPS|CELLFORMATPS))
      ;


      
  };


  template<typename T,bool TS=true>
  class DataCell:public Cell,public CellProperties{
  protected:
    T t;
    typename SignalType<CellSignalData,TS>::value sig;
  public:
    std::string getName() const{
      return Typename<T>::name+" DataCell";
    }
    typedef T value_type;
    void getData(DataP out){
      *(trust_cast<T*>(out))=t;
    };
    void streamGet(std::ostream& o){
      write(o,t);
    };
    void setData(CellSignalData in){
      t= *(trust_cast<const T*>(in));
      sig(in);
    };


    void setData(const T& bt){
      t=bt;
      sig(&t);
    }
    void streamSet(std::istream& i){
      readWholeShws(t,i);
      sig((DataP)&t);
    };

   
	
    void streamSet(std::ostream& o,std::istream& i){
      setData(i);
      StdReply r;
      o<<r;
    }
    void connect(CellConnection &r){
      sig.connect(r);
    }
    void disconnect(CellConnection &r){
      sig.disconnect(r);
    }

    void formatSignal(std::ostream& o,CellSignalData data){
      write(o,*(trust_cast<const T*>(data)));
    }
    DataCell(const T& pt):t(pt){
    }
    DataCell(){
    }
    const CellProperties* getProperties() const{
      return this;
    }
    int getCapabilities() const{
      return TypedCapabilities<T,Void,T,Void,T,Void,false,false,false>::value;
	
    }
    const void* directValuePointer() const{
      return &t;
    }


    DataCell& operator=(const T& v){
      t=v;
      sig(&t);
      return *this;
    }

    void assure(T& v){
      if (v!=t){
	t=v;
	sig(&t);
      }
    }
    operator  const  T&(){ return t;}

    const T& get(){
      return t;
    }

  };//DataCell
  




  template<typename T,bool TS=true>
  class HexDataCell:public DataCell<T,TS>{
    typename SignalType<Cell::CellSignalData,TS>::value sig;
  public:
    void streamGet(std::ostream& o){
      writeHex(o,this->t);
    };
    void streamSet(std::istream& i){
      readHex(this->t,i);
      sig((DataP)&this->t);
    };
    HexDataCell(){}
    HexDataCell(const T& t):DataCell<T,TS>(t){
    }
	
  };//HexCell
  

  


  template<bool TS>
  class DataCell<std::string,TS> :public Cell,public CellProperties{
    std::string t;
    typename SignalType<CellSignalData,TS>::value sig;
  public:
    std::string getName() const{
      return "string DataCell";
    }
    typedef std::string value_type;
    void getData(DataP out){
      *(trust_cast<std::string*>(out))=t;
    };
    void streamGet(std::ostream& o){
      write(o,t);
    };
    void setData(CellSignalData in){
      t= *(trust_cast<const std::string*>(in));
      sig(in);
    };


    void setData(const std::string& bt){
      t=bt;
      sig(&t);
    }
    void streamSet(std::istream& i){
      readWholeShws(t,i);
      sig((DataP)&t);
    };	

    void streamSet(std::stringstream& i){
      t=i.str();
      sig((DataP)&t);
    };

    void streamSet(std::ostream& o,std::istream& i){
      setData(i);
      StdReply r;
      o<<r;
    }
    void connect(CellConnection &r){
      sig.connect(r);
    }
    void disconnect(CellConnection &r){
      sig.disconnect(r);
    }

    void formatSignal(std::ostream& o,CellSignalData data){
      write(o,*(trust_cast<const std::string*>(data)));
    }
    DataCell(const std::string& pt):t(pt){
    }
    DataCell(){
    }
    const CellProperties* getProperties() const {
      return this;
    }
int getCapabilities() const{
  return TypedCapabilities<std::string,Void,std::string,Void,std::string,Void,false,false,false>::value;
	
    }
    const void* directValuePointer() const{
      return &t;
    }


  };//DataCell
  
  


  template<typename T,bool TS=true>
class ReferenceCell:public Cell,public CellProperties{
  protected:
  T& t;
  typename SignalType<CellSignalData,TS>::value sig;

public:
  std::string getName() const{
    return Typename<T>::name+" ReferenceCell";
  }
  typedef T value_type;
  void getData(DataP out){
    *(trust_cast<T*>(out))=t;
  };
    void streamGet(std::ostream& o){
      write(o,t);
    };
    void setData(CellSignalData in){
      t= *(trust_cast<const T*>(in));
      sig((CellSignalData)&t);
    };
    void streamSet(std::istream& i){
      readWholeShws(t,i);
      sig((CellSignalData)&t);
    };	
    void streamSet(std::ostream& o,std::istream& i){
      readWholeShws(t,i);
      sig((CellSignalData)&t);
      StdReply r;
      o<<r;
    }
    void formatSignal(std::ostream& o,CellSignalData data){
      write(o,*(trust_cast<const T*>(data)));
    }
    
    void connect(Cell::CellConnection& ref){
      sig.connect(ref);
    }
    
    void disconnect(Cell::CellConnection& ref){
      sig.disconnect(ref);
    }


    ReferenceCell(T& pt):t(pt){
    }
    
    void sendSignal(){
      sig((CellSignalData)&t);
    }
    
    const CellProperties* getProperties() const {
      return this;
    }
    int getCapabilities() const{
      return TypedCapabilities<T,Void,T,Void,T,Void,false,false,false>::value;
      
    }
    const void* directValuePointer() const{
      return &t;
    }
  
};//ReferenceCell

  template<typename T,bool TS=true>
  class HexReferenceCell:public ReferenceCell<T,TS>{


   void streamGet(std::ostream& o){
      o<<std::hex<<this->t<<std::dec;
    };
    void streamSet(std::istream& i){
      i>>std::hex>>this->t>>std::dec;
      this->sig((DataP)&this->t);
    };

  public:

    HexReferenceCell(T& t):ReferenceCell<T,TS>(t){
    }

  };  

  template<typename T>
  class ConstDataCell:public Cell,public CellProperties{
  protected:
    const T t;
  public:
    typedef T value_type;
    
    std::string getName() const{
      return Typename<T>::name+" ConstDataCell";
     }
   
    void getData(DataP out){
      *trust_cast<T*>(out)=t;
    };
   
    /*void setData(const T& pt){
      t=pt;
      }*/


    void streamGet(std::ostream& o){
      //std::cout<<"Getting from the parent!"<<std::endl;
      write(o,t);
    };
    
   
    ConstDataCell(const T& pt):t(pt){
    }

    void formatSignal(std::ostream& o,CellSignalData data){
      write(o,*trust_cast<const T*>(data));
    }

    
    int getCapabilities() const{
      return TypedCapabilities<T,Void,Void,Void,Void,Void,false,false,false>::value;
	
    }

    virtual const CellProperties* getProperties() const {
      return this;
    }

    const T& getReference(){
      return t;
    }
    
  };



    template<typename T>
  class ConstReferenceCell:public Cell,public CellProperties{
  protected:
    const T& t;
  public:
    typedef T value_type;
    
    std::string getName() const{
      return Typename<T>::name+" ConstDataCell";
     }
   
    void getData(DataP out){
      *trust_cast<T*>(out)=t;
    };
   
    /*void setData(const T& pt){
      t=pt;
      }*/


    void streamGet(std::ostream& o){
      //std::cout<<"Getting from the parent!"<<std::endl;
      write(o,t);
    };
    
   
    ConstReferenceCell(const T& pt):t(pt){
    }

    void formatSignal(std::ostream& o,CellSignalData data){
      write(o,*trust_cast<const T*>(data));
    }

    
    int getCapabilities() const{
      return TypedCapabilities<T,Void,Void,Void,Void,Void,false,false,false>::value;
	
    }

    virtual const CellProperties* getProperties() const {
      return this;
    }

    const T& getReference(){
      return t;
    }

      const void* directValuePointer() const{
	return &t;
      }
  };





  /*  CellProperties * SERIALIZEDDATACELL();
  bool isSerializedDataCell(Cell* c);


  CellProperties * CELLHOLDER();*/

  //bool isCellHolder(Cell* c);

  class SerializedDataCell:public ConstDataCell<std::string>{

    int getCellFlags() const{
      return SERIALIZEDDATACELL;
    }

    int getCapabilities() const{
      return CELLGETS;
    }



  public:

    const CellProperties* getProperties() const {
      return this;

    }


    SerializedDataCell(const std::string& s):ConstDataCell<std::string>(s){      
      //std::cout<<"Created serdata "<<this<<std::endl;
	}

    /*~SerializedDataCell(){
	   std::cout<<this<<" dead"<<std::endl;
	   }*/

    void streamGet(std::ostream& o){
      //std::cout<<"getting from "<<this<<std::endl;
      write(o,t);
    };

  };


  class CellHolder:public Cell,public CellProperties{
    Cell* realCell;
  public:
    std::string getName() const {
      return "Holder for"+realCell->getName();
    }
    CellHolder(Cell* prealCell):realCell(prealCell){
      //std::cout<<"created cellHolder"<<this<<std::endl;
    }

    Cell* getRealCell(){
      return realCell;
    }

    void replaceCell(Cell* c){
      delete realCell;
      realCell=c;
    }

    void getData(DataP out){
      realCell->getData(out);
    }
    void getData(DataP out,CellSignalData in){
      realCell->getData(out,in);      
    }
    void setData(CellSignalData in){
      realCell->setData(in);
    }
    void setData(DataP out,CellSignalData in){
      realCell->setData(out,in);
    }
    void streamGet(std::ostream& o){
      realCell->streamGet(o);
    }
    void streamGet(std::ostream& o,std::istream& i){
      realCell->streamGet(o,i);
    }
    void streamSet(std::istream& i){
      realCell->streamSet(i);
    }
    
    void streamSet(std::ostream& o,std::istream& i){
      realCell->streamSet(o,i);
    }

    void connect(CellConnection& ref){
      realCell->connect(ref);
    }

    void connect(CellConnection& ref,CellSignalData p){
      realCell->connect(ref,p);
    }
    virtual void connect(CellConnection& ref,std::istream& i){
      realCell->connect(ref,i);
    }
    virtual void disconnect(CellConnection &d){
      realCell->disconnect(d);
    }

    virtual void formatSignal(std::ostream& o,CellSignalData data){
      realCell->formatSignal(o,data);
    }
    virtual void formatParam(std::ostream& o,CellSignalData data){
      realCell->formatParam(o,data);
    }

    int getCapabilities() const{
      if (realCell->getProperties()==NULL) return 0;
      return realCell->getProperties()->getCapabilities();
    }

    int getPreferredPort() const{
      if (realCell->getProperties()==NULL) return 0;
      return realCell->getProperties()->getPreferredPort();
    }

    const void* directValuePointer() const{
      if (realCell->getProperties()==NULL) return 0;
      return realCell->getProperties()->directValuePointer();
    }

    int getCellFlags() const{
      return CELLHOLDER;
    }

    const CellProperties* getProperties() const {
      return this;
    }
    virtual ~CellHolder(){
      //std::cout<<"deleting cellholder "<<this<<std::endl;
      delete realCell;
    }
  };


  inline Cell* getRealCell(Cell* c){
    if (c!=NULL&&c->getProperties()!=NULL&&(c->getProperties()->getCellFlags()&CELLHOLDER)!=0){
      return ((CellHolder*)c)->getRealCell();
    }
    return c;
  }
  
  template<typename V,bool Resizable=true,bool TS=true>
  class SizeCell:public Cell{
    V& v; 
    typename SignalType<CellSignalData,TS>::value sig;
  public:
    typedef size_t value_type;

    std::string getName() const {
      return Typename<V>::name+" SizeCell";
     }

    void getData(DataP out){
      *trust_cast<size_t*>(out)=v.size();
    };

    void setData(CellSignalData in){
      size_t nsize=*trust_cast<const size_t*>(in);
      v.resize(nsize);
      sig(in);
    };
    
    void streamGet(std::ostream& o){
      o<<v.size();
    };
    void streamSet(std::istream& i){
      size_t n;
      i>>n;
      v.resize(n);
      sig((CellSignalData)(&n));
    };

    void streamSet(std::ostream& o,std::istream& i){
      setData(i);
      StdReply r;
      o<<r;
    }
    
    void set(size_t n){
      setData(&n);
    }

    void connect(CellConnection &r){
      sig.connect(r);
    }
    void disconnect(CellConnection &r){
      sig.disconnect(r);
    }

    

    SizeCell(V& pv):v(pv){
    }
    void formatSignal(std::ostream& o,CellSignalData data){
      write(o,*trust_cast<const size_t*>(data));
    }
    
  };


  template<typename V,bool TS>
  class SizeCell<V,false,TS>:public Cell{
    V& v; 
    typename SignalType<CellSignalData,TS>::value sig;
  public:
    typedef size_t value_type;

    std::string getName() const {
      return Typename<V>::name+" SizeCell";
     }

    void getData(DataP out){
      *trust_cast<size_t*>(out)=v.size();
    };
   
    
    void streamGet(std::ostream& o){
      o<<v.size();
    };
    

  SizeCell(V& pv):v(pv){
    }
    void formatSignal(std::ostream& o,CellSignalData data){
      write(o,*trust_cast<const size_t*>(data));
    }
    
  };

 

  enum TypedCellAct{NOACT,FULLACT,NOINST};
  
 template<typename T,TypedCellAct A>
 class GettableCell;
  

  template<typename T>
  class GettableCell<T,NOACT>{    
  };


  template<typename T>
  class GettableCell<T,FULLACT>:public virtual Cell{
  public:
    virtual void getData(T& t)=0;

    void getData(DataP out){
      getData(as<T>(out));
    }
    void streamGet(std::ostream& o){
      //std::cout<<" //gettable cell"<<std::endl;
      T t;
      getData(t);
      write(o,t);
    }

  };

  template<typename T>
  class GettableCell<T,NOINST>:public virtual Cell{    

    virtual void getData(T& t)=0;
    void streamGet(std::ostream& o)=0;

    void getData(DataP out){
      getData(as<T>(out));
     }
    
  };









  /*template<typename T,typename C,void (C::*gf)(T&)>
  struct GetFunction{
    C* gc;
    GetFunction(C* pc):gc(pc){
    }
  };



  
  template<typename T,typename C,void (C::*gf)(T&)>
  class GettableCell<GetFunction<T,C,gf> >{
    void getData(DataP out){
      (gc->*gf)(as<T>(out));
    }
    };*/


  template<typename T,typename P,TypedCellAct A>
  class ParamGettableCell;

  template<typename T,typename P>
  class ParamGettableCell<T,P,NOACT>{
  };
  
  template<typename T,typename P>
  class ParamGettableCell<T,P,FULLACT>:public virtual Cell{
  public:
    virtual void getData(T& t,const P& p)=0;
    void getData(DataP out,CellSignalData in){
      getData(as<T>(out),asConst<P>(in));
    }
    void streamGet(std::ostream& o,std::istream& in){        
      T t;
      P p;
      readWholeShws(p,in);
      getData(t,p);
      write(o,t);
    }
    
  };


  template<typename T,typename P>
  class ParamGettableCell<T,P,NOINST>:public virtual Cell{
  public:
    virtual void getData(T& t,const P& p)=0;
    virtual void streamGet(std::ostream& o,std::istream& in)=0;
    void getData(DataP out,CellSignalData in){
      getData(as<T>(out),asConst<P>(in));
    }    
  };
  
   
  template<typename T,TypedCellAct A>
  class SettableCell;


  template<typename T>
  class SettableCell<T,NOACT>{
    
  };
  
  template<typename T>
  class SettableCell<T,FULLACT>:public virtual Cell{
  public:
    virtual void setData(const T& t)=0;

    void setData(CellSignalData in){
      setData(asConst<T>(in));
    }
    void streamSet(std::istream& i){
      T t;
      readWholeShws(t,i);
      setData(t);
    }
  };

  template<typename T>
  class SettableCell<T,NOINST>:public virtual Cell{
  public:
    virtual void setData(const T& t)=0;
    virtual void streamSet(std::istream& i)=0;

    void setData(CellSignalData in){
      setData(asConst<T>(in));
    }
  };

  
  template<typename T,typename R,TypedCellAct A>
  class ParamSettableCell;

  template<typename T,typename R>
  class ParamSettableCell<T,R,NOACT>{
  };


  template<typename T,typename R>
  class ParamSettableCell<T,R,FULLACT>:public virtual Cell{
    virtual void setData(R& r,const T& t)=0;
    void setData(DataP out,CellSignalData in){
      setData(as<R>(out),asConst<T>(in));
    }
    void streamSet(std::ostream& o,std::istream& in){        
      T t;
      R r;
      readWholeShws(t,in);
      setData(r,t);
      write(o,r);
    }
    
  };

  template<typename T,typename R>
  class ParamSettableCell<T,R,NOINST>:public virtual Cell{
    virtual void setData(R& r,const T& t)=0;
    void setData(DataP out,CellSignalData in){
      setData(as<R>(out),asConst<T>(in));
    }
    virtual void streamSet(std::ostream& o,std::istream& in)=0;
  };





  
  template<typename T>
  class ConnectableCell:public virtual Cell{
  public:
    virtual void connect(Cell::CellConnection& ref)=0;  
    void formatSignal(std::ostream& o,CellSignalData data){
      write(o,asConst<T>(data));
    }    
  };

  template<>
  class ConnectableCell<Void>{    
  };
  
  template<typename T,typename P,TypedCellAct A,bool requiresParam>
  class ParamConnectableCell{
  };


  template<typename T,typename P,bool requiresParam>
  class ParamConnectableCell<T,P,NOACT,requiresParam>{
  };



  
  template<typename T,typename P>
  class ParamConnectableCell<T,P,FULLACT,false>:public virtual Cell{
  public:
    virtual void connect(Cell::CellConnection& ref,const P& p)=0;
    
    void formatParam(std::ostream& o,CellSignalData data){
      write(o,asConst<P>(data));
    }
    void connect(Cell::CellConnection& ref,std::istream& i){
      P p;
      readWholeShws(p,i);
      connect(ref,p);
    }    
  };

  template<typename T,typename P>
  class ParamConnectableCell<T,P,FULLACT,true>:public virtual Cell{
  public:
    virtual void connect(Cell::CellConnection& ref,const P& p)=0;
    
    void formatParam(std::ostream& o,CellSignalData data){
      write(o,asConst<P>(data));
    }
    void connect(Cell::CellConnection& ref,std::istream& i){
      P p;
      readWholeShws(p,i);
      connect(ref,p);
    }    
    void formatSignal(std::ostream& o,CellSignalData data){
      write(o,asConst<T>(data));
    }    
  };


  template<typename T,typename P>
  class ParamConnectableCell<T,P,NOINST,false>:public virtual Cell{
  public:
    virtual void connect(Cell::CellConnection& ref,const P& p)=0;
    
    void formatParam(std::ostream& o,CellSignalData data){
      write(o,asConst<P>(data));
    }
    virtual void connect(Cell::CellConnection& ref,std::istream& i)=0;

  };

  template<typename T,typename P>
  class ParamConnectableCell<T,P,NOINST,true>:public virtual Cell{
  public:
    virtual void connect(Cell::CellConnection& ref,const P& p)=0;
    
    void formatParam(std::ostream& o,CellSignalData data){
      write(o,asConst<P>(data));
    }
    virtual void connect(Cell::CellConnection& ref,std::istream& i)=0;

    void formatSignal(std::ostream& o,CellSignalData data){
      write(o,asConst<T>(data));
    }    
  };





  template <bool b> class Disconnectable{
  };


  template<> class Disconnectable<true>{    
    virtual void disconnect(Cell::CellConnection& ref)=0;
  };
  
  template<> class Disconnectable<false>{    
  };


  template <typename T>
  struct isNotVoid{
    static const bool value=true;
  };

  template <>
    struct isNotVoid<Void>{
    static const bool value=false;
  };


  template<typename T> struct TypedCellUnistantiable{
    static const bool value=false;
  };

  template<typename T,bool requiresP> struct cellActType{
    static const TypedCellAct value=(VoidToTrue<T>::value||requiresP)?NOACT:(TypedCellUnistantiable<T>::value?NOINST:FULLACT);
  };

  template<typename T,typename P> struct parCellActType{
    static const TypedCellAct value=(VoidToTrue<P>::value)?NOACT:
      ((TypedCellUnistantiable<T>::value||TypedCellUnistantiable<P>::value)?NOINST:FULLACT);
  };

    
 

  template <typename PGT,typename GP=typename cellTypes<PGT>::value::GP,
	    typename ST=typename cellTypes<PGT>::value::ST,typename SP=typename cellTypes<PGT>::value::SP,
	    typename UT=typename cellTypes<PGT>::value::UT,typename UP=typename cellTypes<PGT>::value::UP,
	    bool getRequiresParam=cellTypes<PGT>::value::getRequiresParam,
	    bool setRequiresParam=cellTypes<PGT>::value::setRequiresParam,
	    bool connectRequiresParam=cellTypes<PGT>::value::connectRequiresParam
	    >
  class TypedCell:
    public GettableCell<typename cellTypes<PGT>::value::GT,cellActType<typename cellTypes<PGT>::value::GT,getRequiresParam>::value >,
    public ParamGettableCell<typename cellTypes<PGT>::value::GT,GP,parCellActType<typename cellTypes<PGT>::value::GT,GP>::value>,
    public SettableCell<ST,cellActType<ST,setRequiresParam>::value>,
    public ParamSettableCell<ST,SP,parCellActType<ST,SP>::value>,
    public ConnectableCell<typename trueToVoid<UT,connectRequiresParam>::value >,
    public ParamConnectableCell<UT,UP,parCellActType<UT,UP>::value,connectRequiresParam>,
    public Disconnectable<isNotVoid<UT>::value >,
    public CellProperties
  {
  public:

 

    typedef typename cellTypes<PGT>::value::GT GT;
    /*typedef GT gt;
    typedef GP gp;
    typedef ST st;
    typedef SP sp;
    typedef UT ut;
    typedef UP up;*/

    const CellProperties* getProperties() const{
      return this;
    }

    int getCapabilities() const{
      return TypedCapabilities<GT,ST,GP,SP,UT,UP,
	getRequiresParam,setRequiresParam,connectRequiresParam>::value;
    }
    
  };


  template<typename T,bool TS> class ROCell:public TypedCell<T,Void, Void,Void, T,Void>{
  protected:
    T data;
    typename SignalType<Cell::CellSignalData,TS>::value sig;

  public:
    const T& get(){
      return data;
    }    
    std::string getName() const{
      return Typename<T>::name+" ROCell";
    }

       
    ROCell& operator=(const ROCell& t){
      data=t.data;
      sig(&data);
      return *this;
    }
    
    ROCell& operator=(const T& t){
      data=t;
      sig(&data);
      return *this;
    }
    void getData(T& t){
      onGet();
      t=data;
    }

    void connect(Cell::CellConnection &r){
      sig.connect(r);
    }
    
    void disconnect(Cell::CellConnection &r){
      sig.disconnect(r);
    }

    virtual void onGet(){}

  };//ROCell*/

  



  template<typename T,bool TS> class RWCell:public TypedCell<T,Void, Void,Void, T,Void>{
    T data;
    typename SignalType<Cell::CellSignalData,TS>::value sig;   
  public:

    const T& get(){
      return data;
    }    

    operator const T&(){
      return data;
    }
       
    RWCell& operator=(const RWCell& t){
      data=t.data;
      sig(&data);
      return *this;
    }
    
    RWCell& operator=(const T& t){
      data=t;
      sig(&data);
      return *this;
    }

    void getData(T& t){
      t=data;
    }
    
    void setData(const T& t){
      data=t;
    }

    virtual void connect(Cell::CellConnection &r){
      sig.connect(r);
    }
    
    virtual void disconnect(Cell::CellConnection &r){
      sig.disconnect(r);
    }
    virtual void onGet()=0;
    virtual void onSet()=0;

  };//ROCell


}




#endif
