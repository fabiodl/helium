/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */


#ifndef HE_REMOTECELL
#define HE_REMOTECELL

#include <map>
#include <list>
#include <helium/system/cellClient.h>
#include <helium/system/basicCell.h>
#include <helium/thread/sharedAccess.h>

namespace helium{

  class UnspecifiedRemoteCellSignature:public CellSignature{
    typedef Void GT;
    typedef Void GP;
    typedef Void ST;
    typedef Void SP;
    typedef Void UT;
    typedef Void UP;
    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;
  };

  //CellProperties * TYPELESSREMOTECELL();

  

  template<typename Signature>
  class RemoteCell:public Cell,
  //public ProxiableCell,
		   public CellProperties{
    SharedAccess connAccess;
    CellClient* client; 
    std::string remotename;

    class UpdateReader:public GenericSignalInterpreter{     
      RemoteCell& owner;
    public:
      std::list<std::pair<CellCallback*,exc::ExceptionHandler*> > cs;
      void notify(std::istream& i);
      std::string par;
      UpdateReader(RemoteCell& powner,const std::string& s):
	owner(powner),par(s){
      }
      void fail(const std::string& reason);
      //void addHandler(){
      //}
    };//UpdateReader

    class DInfo:public DisconnectInfo{
      friend class RemoteCell;
      UpdateReader* ur;
      std::list<std::pair<CellCallback*,exc::ExceptionHandler*> >::iterator it;
      DInfo(UpdateReader* pur,const std::list<std::pair<CellCallback*,exc::ExceptionHandler*> >::iterator &pit):
      ur(pur),it(pit){
      }
    };


    std::map<std::string,UpdateReader*>  conns;
   
    virtual int getCapabilities() const{
      //std::cout<<"getRP is "<<getRP<<std::endl;
      return TypedCapabilities<typename Signature::GT,
			       typename Signature::GP,
			       typename Signature::ST,
			       typename Signature::SP,
			       typename Signature::UT,
			       typename Signature::UP,
			       Signature::getRequiresParam,
			       Signature::setRequiresParam,
			       Signature::connectRequiresParam>::value;
	
    }
 
  

  public:    
    std::string getName() const{return 
	Typename<typename Signature::GT>::name+","+Typename<typename Signature::GP>::name+
	Typename<typename Signature::ST>::name+","+Typename<typename Signature::SP>::name+
	Typename<typename Signature::UT>::name+","+Typename<typename Signature::UP>::name+" RemoteCell";}
    
    CellClient* getClient(){return client;}
    
    virtual const CellProperties* getProperties() const{
      return this;
    }
	
    void getData(DataP d);
    void getData(DataP out,CellSignalData in);
    void setData(CellSignalData d);
    void setData(DataP out,CellSignalData in);    
    void streamGet(std::ostream& o);
    void streamGet(std::ostream& o,std::istream& i);  
    void streamSet(std::istream& i);
    void streamSet(std::ostream& o,std::istream& i);
    void formatSignal(std::ostream& o,CellSignalData data);
    void connect(CellConnection &c);
    void connect(CellConnection &c,CellSignalData p);
    void disconnect(CellConnection &d);

    RemoteCell(CellClient* c,const std::string& remotename);
    RemoteCell(RemoteCell<UnspecifiedRemoteCellSignature>* base);
    virtual ~RemoteCell();
  };//RemoteCell

  template<> class RemoteCell<UnspecifiedRemoteCellSignature>:public Cell{
    friend class RemoteCellImporter;
    template<typename Signature>
      friend class RemoteCell;
    CellClient* client; 
    std::string remotename;    

    class TypelessRemoteProperties:public CellProperties{
      int capabilities;
      int getCapabilities() const{
	return capabilities;
      }
      int getCellFlags() const{
	return TYPELESSREMOTECELL;
      }
    public:
      TypelessRemoteProperties(int pcapabilites):capabilities(pcapabilites){}

    };
    TypelessRemoteProperties capabilities;
    
    RemoteCell(CellClient* pc,std::string &premotename,int pcapabilities):client(pc),remotename(premotename),capabilities(pcapabilities){  //just the importer should create these cells that are deleted by the typed constructor
    //std::cout<<"new typeless remote cell @"<<this<<std::endl;
    }




    const CellProperties* getProperties() const{
      return &capabilities;
    }
    std::string getName() const{
      return "Typeless remote cell";
    }
    void streamGet(std::ostream& o);
    void streamGet(std::ostream& o,std::istream& i);  
    void streamSet(std::istream& i);
    void streamSet(std::ostream& o,std::istream& i);
  };//RemoteCell<void,void,void,void,void,void>


  
//bool isTypelessRemoteCell(Cell* c);

 
}//helium


#include "remoteCell.hpp"

#endif
