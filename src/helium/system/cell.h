/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_CELL
#define HE_CELL

#include <iostream>
#include <set>
#include <sstream>
#include <helium/core/exception.h>
#include <helium/signal/ehSignal.h>
#include <helium/system/idTree.h>
#include <helium/destroy/deleteList.h>
#include <helium/util/ioUtil.h>
#include <helium/debug/trace.h>

namespace helium{
    
  typedef void* DataP;


 


  template<typename T> void copyVal(DataP d,const T &val){
    *trust_cast<T*>(d)=val;
  }

  template<typename T> void copyVal(T &val,DataP d){
    val=*trust_cast<T*>(d);
  }

  template <typename GT,typename GP,typename ST,typename SP,typename UT,typename UP,
	    bool GRP,bool SRP,bool CRP> class Volatile;//forward declaration

    class StdReply{    
  };


 inline std::ostream& operator<<(std::ostream& o,const StdReply &s){
    return o<<"ok";    
  }
  
 inline std::istream& operator>>(std::istream& i,StdReply &s){
    std::string reply;
    i>>reply;    
    if (reply!="ok"){
      throw exc::SyntaxError("Expected \"ok\", received "+reply);
    }
    return i;    
  }
  
 

  

  
  enum CellOperation{
    CELLGET=1<<0, CELLGETP=1<<1, 
    CELLSET=1<<2, CELLSETP=1<<3,
    CELLGETS=1<<4,CELLGETPS=1<<5,
    CELLSETS=1<<6,CELLSETPS=1<<7,
    CELLCONN=1<<8,CELLCONNP=1<<9,
    CELLCONNPS=1<<10,
    CELLDISC=1<<11,
    CELLFORMATDS=1<<12, //format data
    CELLFORMATPS=1<<13  //foramt param

  };
  
  inline std::ostream& printFlag(std::ostream& o,int flag,int val){
    return o<<(flag&val?"o":"x");
  }
  
  inline void printCapabilities(int c,std::ostream& o){
    o<<"get  ";
    printFlag(o,CELLGET,c);
    o<<" p ";
    printFlag(o,CELLGETP,c);
    o<<" s ";
    printFlag(o,CELLGETS,c);
    o<<" ps ";
    printFlag(o,CELLGETPS,c)<<std::endl;

    o<<"set  ";
    printFlag(o,CELLSET,c);
    o<<" p ";
    printFlag(o,CELLSETP,c);
    o<<" s ";
    printFlag(o,CELLSETS,c);
    o<<" ps ";
    printFlag(o,CELLSETPS,c)<<std::endl;
    
    o<<"conn ";
    printFlag(o,CELLCONN,c);
    o<<" p ";
    printFlag(o,CELLCONNP,c);
    o<<" s ";
    printFlag(o,CELLCONNPS,c)<<std::endl;
    o<<"disc ";
    printFlag(o,CELLDISC,c)<<std::endl;
    o<<"form ";    
    printFlag(o,CELLFORMATDS,c);
    o<<" p ";
    printFlag(o,CELLFORMATPS,c)<<std::endl;
  }

  enum CellFlags{
    VALUEPROXY=1<<0,
    CELLHOLDER=1<<1,
    SERIALIZEDDATACELL=1<<2,
    TYPELESSREMOTECELL=1<<3,
    //PROXIABLE=1<<4
  };




  class CellProperties{
  public:


    virtual int getPreferredPort() const{
      return -1;
    }
    virtual const void* directValuePointer() const{
      return NULL;
    }
    virtual int getCapabilities() const{
      return 0;
    }
    
    virtual int getCellFlags() const{
      return 0;
    }


  };
  
  template<CellFlags flag> class SingleFlagCellProperties:
    public CellProperties{
    static const SingleFlagCellProperties instance;
  public:

    static CellProperties* getInstance(){
      return const_cast<SingleFlagCellProperties*>(&instance);
    }

    virtual int getCellFlags() const{
      return flag;
    }
  };
  

  template<CellFlags flag> 
    SingleFlagCellProperties<flag> const SingleFlagCellProperties<flag>::instance= SingleFlagCellProperties<flag>();



  namespace exc{    
    static const char* CELLOPERATIONDESCRIPTION[]={
    "get","parametric get",
    "set","parametric set",
    "stream get","stream parametric get",
    "stream set","stream parametric set",
    "connection","parametric connection",
    "stream parametric connection",
    "disconnection",
    "update signal format",
    "update parameter format"
  };


    
    
    class UnsupportedCellOperation:public exc::InvalidOperation{
      
      std::string getMessage(CellOperation t,const std::string& s){
	int i;
	for (i=0;(1<<i)!=t;i++);//they are flags
	return std::string(CELLOPERATIONDESCRIPTION[i])+" on "+s;
      }
    public:
    UnsupportedCellOperation(CellOperation t,const std::string& s):InvalidOperation(getMessage(t,s)){

      }
    };
  }//namespace exc
 
  template<typename T>
  class ActionOnceList;


  template<typename T>
  class ActionOnceNode{
    friend class ActionOnceList<T>;    
    ActionOnceNode* next;//we only push front, no need to init
    virtual void action(T)=0;

  };

  template<typename T>
  class ActionOnceList{
    ActionOnceNode<T>* head;
    static CountMutex m;
    static exc::MultipleExceptionHolder meh;
  public:
    ActionOnceList():head(NULL){
    }
    

    void act(T t){
      m.lock();
      for (;head!=NULL;head=head->next){
	try{
	  head->action(t);
	}catch(exc::Exception& e){
	  meh.handle(e);
	}
      }
      meh.check();
      m.unlock();
    }


    void push_front(ActionOnceNode<T>* n){
      m.lock();
      n->next=head;
      head=n;
      m.unlock();
    }

    void remove(ActionOnceNode<T>* tn){
      m.lock();
      if (head==tn){
	head=NULL;
	m.unlock();
	return;
      }
      for (ActionOnceNode<T>* n=head;n!=NULL;n=n->next){
	if (n->next==tn){
	  n->next=n->next->next;
	}
      }
      m.unlock();
    }//remove
  };

  template<typename T> CountMutex ActionOnceList<T>::m("actionLock");
  template<typename T> exc::MultipleExceptionHolder ActionOnceList<T>::meh;

  class Memory;
  class Cell{        
  public:
    typedef  const void* CellSignalData;
    typedef  Callback<CellSignalData> CellCallback;
    typedef EhConnReference<CellSignalData>  CellConnection;
    
  private:
    template <typename GT,typename GP,typename ST,typename SP,typename UT,typename UP,
	      bool GRP,bool SRP,bool CRP
	      > friend class Volatile;        
    friend std::ostream&  operator<<(std::ostream& o,const Cell &t);
    friend std::istream& operator>>(std::istream& i,Cell &t);
    
    //friend class CellHolder;
    
    


    //protected:
  public:    
    virtual void getData(DataP out){
      throw exc::UnsupportedCellOperation(CELLGET,getName());
    }      
    virtual void getData(DataP out,CellSignalData in){
      throw exc::UnsupportedCellOperation(CELLGETP,getName());
    }     
    virtual void setData(CellSignalData in){
      throw exc::UnsupportedCellOperation(CELLSET,getName());
    }       
    virtual void setData(DataP out,CellSignalData in){      
      throw exc::UnsupportedCellOperation(CELLSETP,getName());
    }      		          

    ActionOnceList<Cell*> onDelete;
  public:    

    

    virtual std::string getName() const=0;
    virtual void streamGet(std::ostream& o){
      throw exc::UnsupportedCellOperation(CELLGETS,getName());
    }

    virtual void print(std::ostream& o){
      std::stringstream ss;
      streamGet(ss);
      if (ss.str().size()<500){
	o<<ss.str();
      }else{
	o<<"...";
      }
    }
    virtual void streamGet(std::ostream& o,std::istream& i){
      throw exc::UnsupportedCellOperation(CELLGETPS,getName());
    }  
    virtual void streamSet(std::istream& i){
      throw exc::UnsupportedCellOperation(CELLSETS,getName());
    }
    virtual void streamSet(std::ostream& o,std::istream& i){
      throw exc::UnsupportedCellOperation(CELLSETPS,getName());
    }
    virtual void connect(CellConnection& ref){
      throw exc::UnsupportedCellOperation(CELLCONN,getName());
    }
    virtual void connect(CellConnection& ref,CellSignalData p){
      throw exc::UnsupportedCellOperation(CELLCONNP,getName());
    }
    virtual void connect(CellConnection& ref,std::istream& i){
      throw exc::UnsupportedCellOperation(CELLCONNPS,getName());
    }
    virtual void disconnect(CellConnection &d){
      throw exc::UnsupportedCellOperation(CELLDISC,getName());
    }

    virtual void formatSignal(std::ostream& o,CellSignalData data){
      throw exc::UnsupportedCellOperation(CELLFORMATDS,getName());
    }

    //not used
    virtual void formatParam(std::ostream& o,CellSignalData data){
      throw exc::UnsupportedCellOperation(CELLFORMATPS,getName());
    }

    template<typename T> inline  void get(T &t){
      getData((DataP)&t);
    }
    template<typename T,typename P> inline  void get(T &t,const P &p){
      getData((DataP)&t,(CellSignalData)&p);
    }
    template<typename T> inline void set(const T &t){
      setData((CellSignalData)&t);
    }

    template<typename T,typename P> inline void set(P& p,const T &t){
      setData((DataP)&p,(CellSignalData)&t);
    }

    template<typename T> bool operator ==(const T& b) const{
      std::stringstream as;
      std::stringstream bs;
      const_cast<Cell*>(this)->streamGet(as);
      bs<<b;
      return as.str()==bs.str();      
    }

    template<typename T> bool operator !=(const T& b) const{
     std::stringstream as;
      std::stringstream bs;
      const_cast<Cell*>(this)->streamGet(as);
      bs<<b;
      return as.str()!=bs.str(); 
    }

    virtual const CellProperties* getProperties() const {
      return NULL;
    }

    virtual ~Cell(){
      onDelete.act(this);
    }
  
  };//Cell


  inline bool isCellFlagSet(Cell* c,CellFlags f){
    if ((c==NULL)||(c->getProperties()==NULL)) return false;
    return (c->getProperties()->getCellFlags()&f)!=0;
  }

  inline bool hasCapability(Cell* c,CellOperation f){
    if ((c==NULL)||(c->getProperties()==NULL)) return false;
    return (c->getProperties()->getCapabilities()&f)!=0;
  }


  
  template <typename T> T& as(DataP p){
    return *(trust_cast<T*>(p));
  }

  template <typename T> const T& asConst(Cell::CellSignalData p){
    return *(trust_cast<const T*>(p));
  }

  template <> inline std::string ptoString<Cell>(const Cell* t){
    if (t==NULL)
      return "NULL";   
    std::stringstream ss;
    try{
#ifdef TRACEDEBUG
      exc::ExceptionsAreSilent=true;
#endif      
      const_cast<Cell*>(t)->streamGet(ss);
#ifdef TRACEDEBUG
      exc::ExceptionsAreSilent=false;
#endif
      std::string s=ss.str();
      return s;
      //return ss.str();
    }catch(exc::Exception&){
      return "unprintable cell";
    }

  }//toString
  
  inline std::ostream& operator<<(std::ostream& o,const Cell &t){
    const_cast<Cell*>(&t)->print(o);
    //o<<" "<<t.getName();
    return o;
  }

  inline std::istream& operator>>(std::istream& i,Cell &t){
    t.streamSet(i);
    return i;
  }




  class Memory:public IDNode<Cell>,public ActionOnceNode<Cell*>{
    DeleteList<Cell> toDel;
    


    virtual void action(Cell* c){
      toDel.removeFromDeletion(c);
    }

  public:
    void addForDeletion(Cell* c){
      toDel.addForDeletion(c);
      c->onDelete.push_front(this);

     
    }       
    void removeFromDeletion(Cell* c){
      c->onDelete.remove(this);
      action(c);
    }
    
    ~Memory(){
      for (std::set<Cell*>::iterator i=toDel.toDelete.begin();
	   i!=toDel.toDelete.end();i++){
	(*i)->onDelete.remove(this);
      }
    }

    /*std::string get(IDPath& p){
      std::stringstream ss;
      mem.downPath(p).streamGet(ss);
      return ss.str();
      }*/
    
  };//Memory


 
  class CellReference{
  public:
    Cell* p;      
  CellReference(Cell* pp=NULL):p(pp){}
    Cell* operator=(Cell* c){
      p=c;
      return c;
    }
    Cell* operator->(){
      return p;
    }
    Cell& operator*(){
      return *p;
    }
    bool isNull(){
      return p==NULL;
    }
  };//CellUid
  
  inline std::ostream& operator<<(std::ostream &o,const CellReference &uid){
    o<<uid.p;
    return o;
  }
  inline std::istream& operator>>(std::istream &i,CellReference &uid){
    void* p;
    i>>p;
    uid.p=trust_cast<Cell*>(p);
    return i;
  }





}//helium
#endif
