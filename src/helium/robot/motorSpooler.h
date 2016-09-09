/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.02.18
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_MOTORSPOOLER
#define HE_MOTORSPOOLER

#include <helium/spooling/spooler.h>
#include <helium/robot/motorInterface.h>
#include <helium/vector/operations.h>
#include <helium/system/signalledValue.h>
#include <helium/system/basicCell.h>
#include <helium/system/cellSignal.h>
#include <helium/system/cellDescription.h>

#include <map>
#include <sstream>

namespace helium{


  namespace motor{
    
    typedef std::string ModeId;
        


    template<typename MotorSign>
    struct MotorAccess{      
      typedef typename MotorSign::Prop PT;
      typedef typename MotorSign::Pos PS;
      motor::Interface* high; //currently selected iface      
      typename motor::Raw<MotorSign>::Set* raw; //currently selected raw
      typename motor::Raw<MotorSign>::Hardware* hw;
      MotorAccess();      
      void get(std::pair<motor::IfaceId*,motor::MotorLevel>& p);
    };
    
    template<typename MotorSign>
    std::ostream& operator<<(std::ostream&,const MotorAccess<MotorSign>&); 

    typedef std::pair<MotorLevel,motor::IfaceId> IfaceSelection;
    
    struct PositionCellSignature:public CellSignature{
      
      typedef Interface::Position GT;
      typedef Void GP;
      
      typedef Void ST;
      typedef Void SP;
      
      typedef Interface::Position UT;
      typedef Void UP;

      static const bool getRequiresParam=false;
      static const bool setRequiresParam=false;
      static const bool connectRequiresParam=false;
      
    };//PositionCellSignature

    struct ExactPositionCellSignature:public PositionCellSignature{
      
    };//PositionCellSignature

  }    //ns motor
 


  template<>
  void deserializeGet<motor::ExactPositionCellSignature>( motor::ExactPositionCellSignature::GT&,std::istream&);

  
 template<>
  void deserializeUpdate<motor::ExactPositionCellSignature>( motor::ExactPositionCellSignature::UT&,std::istream&);


  namespace motor{  
    typedef DefaultSignature<ModeId> MotorModeCellSignature;
    
    struct MultipleMotorModeCellSignature:public CellSignature{
     typedef Void GT;
     typedef Void GP;
      typedef std::pair<HwIdList,motor::ModeId> ST;
     typedef StdReply SP;
     typedef Void UT;
     typedef Void UP;
     static const bool getRequiresParam=false;
     static const bool setRequiresParam=false;
     static const bool connectRequiresParam=false;
    };
						       

  }//ns motor
  
  template<>
  struct DefaultAccessOptions<motor::MotorModeCellSignature>{
    static const int value=UPDATEUSESREFERENCE;
  };
  
  namespace spt{
    template<typename MotorSign,motor::MotorLevel L> struct manSelect;
  }//fw decl


  struct MotorInterfaceSignature:public CellSignature{
    typedef motor::IfaceId GT;
    typedef motor::MotorLevel GP;

    typedef motor::IfaceSelection ST;
    typedef StdReply SP;
    
    typedef motor::IfaceSelection UT;
    typedef Void UP;

    static const bool getRequiresParam=true;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;
        
  };

  template<>
  struct DefaultAccessOptions<MotorInterfaceSignature>{
    static const int value=UPDATEUSESREFERENCE;
  };


  struct MotSpoolPriority{
    enum Priority {IDLE,NORMAL,HIGH,PRIORITIES};
  };

  template<typename MotorSign>
  class MotorSpooler:public Spooler<motor::MotorAccess<MotorSign> >{    
             

  public:   
      
    typedef typename motor::MotorAccess<MotorSign> Motors; 
    typedef typename MotorSign::Prop PT;
    typedef typename MotorSign::Pos PS;

    typedef typename motor::Raw<MotorSign>::Position RawPosition;
    typedef typename motor::Raw<MotorSign>::Property RawProperty;
    
    typedef typename motor::Interface::Position NormPosition;
    typedef typename motor::Interface::Property NormProperty;
       

    class MotorMode{
    private:
      motor::ModeId motorModeId;
      friend class MotorSpooler;
      virtual void addMotor(Motors& m,int j,motor::ModeId prevMode)=0;
      virtual void removeMotor(Motors& m,int j,motor::ModeId newMode)=0;
      virtual NormPosition getPosition(Motors& m,int j)=0;            
      virtual void connect(Cell::CellConnection& ref,int j)=0;   // no need to be thread safe,spooler thread only
      virtual void disconnect(Cell::CellConnection& ref,int j)=0;        
    public:
      motor::ModeId getMotorModeId(){return motorModeId;}
      MotorMode(motor::ModeId pid):motorModeId(pid){
      }    
      virtual ~MotorMode(){}
    };//MotorMode

    class MotorModeCell:public TypedCell<motor::MotorModeCellSignature>{      
      MotorSpooler& owner;      
      int j;

      std::string getName() const;      
      void getData(typename motor::ModeId& m);      
      void setData(const typename  motor::ModeId& m);      
      void setData(StdReply& r,const typename motor::ModeId& m);
      void connect(Cell::CellConnection& ref);
      void disconnect(Cell::CellConnection& ref);
    public:
      MotorModeCell(MotorSpooler& powner,int pj);
    }; //MotorModeCell

    class MultipleMotorModeCell:public TypedCell<motor::MultipleMotorModeCellSignature>{
      MotorSpooler& owner;      
      std::string getName() const;      
      void setData(const std::pair<HwIdList,motor::ModeId>& m);      
      void setData(StdReply& r,const std::pair<HwIdList,motor::ModeId>& m);
    public:
      MultipleMotorModeCell(MotorSpooler& powner);
    };




    class MotorInterfaceCell:public TypedCell<MotorInterfaceSignature
					      >{
      MotorSpooler&  ms;
      std::string getName() const;
      void getData(motor::IfaceId& id,const motor::MotorLevel& l);
      void setData(const motor::IfaceSelection& isel);
      void setData(StdReply& r,const motor::IfaceSelection& isel);
      void connect(Cell::CellConnection& c);
      void disconnect(Cell::CellConnection& c);
    public:
      MotorInterfaceCell(MotorSpooler& pms);      
    };
    
    class MotorInterfaceListCell:public TypedCell<Array<motor::IfaceId>, motor::MotorLevel,
						  Void,Void,
						  Void, Void,true>{
      MotorSpooler&  ms;
      std::string getName() const;
      void getData(Array<motor::IfaceId>& a,const motor::MotorLevel& l);
      //void connect(Cell::CellConnection& c);
      //void disconnect(Cell::CellConnection& c);
    public:
      MotorInterfaceListCell(MotorSpooler& pms);
    };
    
    class MotorPositionCell:public TypedCell<motor::PositionCellSignature>{
      MotorSpooler& owner;
      int j;      
      std::string getName() const;
      void getData(motor::Interface::Position& p);      
      void connect(Cell::CellConnection& c);
      void disconnect(Cell::CellConnection& c);
    public:
      MotorPositionCell(MotorSpooler& powner,int pj):owner(powner),j(pj){
      }      	
    };

    class MotorExactPositionCell:public TypedCell<motor::ExactPositionCellSignature>{
      MotorSpooler& owner;
      int j;      
      std::string getName() const;
      void getData(motor::Interface::Position& p);      
      void connect(Cell::CellConnection& c);
      void disconnect(Cell::CellConnection& c);
      void formatSignal(std::ostream& o,CellSignalData data);
      void streamGet(std::ostream& o);
    public:
      MotorExactPositionCell(MotorSpooler& powner,int pj):owner(powner),j(pj){
      }      	

    };

  
    //class ExactMotorPositionCell:public 


    template<motor::MotorLevel L> class IfaceManager{
      typedef typename motor::motorTypes<MotorSign,L>::Motor Motor;
      typedef std::map<motor::IfaceId,Motor*> IfaceMap;
      IfaceMap ifaces;
    public:
      void do_registerIface(Motor* m);
      void do_unregisterIface(Motor* m);      
      void throwIfaceError(const motor::IfaceId& id);      
      Motor* get(const motor::IfaceId& id);
      MotorSpooler& owner;
    public:
      IfaceManager(MotorSpooler& powner):owner(powner){
      }
      void do_listIfaces(Array<motor::IfaceId>*);
      void registerIface(Motor* m); //start the spooler before calling
      void unregisterIface(Motor* m); //start the spooler before calling     
    };

    class IfaceManagerSet{
      friend class MotorInterfaceCell;
      friend struct helium::spt::manSelect<MotorSign,motor::IFACELEVEL>;
      friend struct helium::spt::manSelect<MotorSign,motor::RAWLEVEL>;
      friend struct helium::spt::manSelect<MotorSign,motor::HWLEVEL>;
      CellSignal<MotorInterfaceSignature::UT,false> update;

      motor::IfaceSelection sel;
      IfaceManager<motor::IFACELEVEL>  high;
      IfaceManager<motor::RAWLEVEL>  raw;
      IfaceManager<motor::HWLEVEL>  hw;   
    public:
      //Signal<Cell::SignalData> update;
      void set(Motors& m,motor::MotorLevel l,const motor::IfaceId& id);

      template<motor::MotorLevel L> void registerIface(typename motor::motorTypes<MotorSign,L>::Motor* m);
      template<motor::MotorLevel L> void unregisterIface(typename motor::motorTypes<MotorSign,L>::Motor* m);
      void do_listIfaces(std::pair<Array<motor::IfaceId>*,motor::MotorLevel>&);
      void do_registerAll(typename motor::Raw<MotorSign>::Hardware* hwif);
      void registerAll(typename motor::Raw<MotorSign>::Hardware* hwif);
      
      IfaceManagerSet(MotorSpooler& ms):high(ms),raw(ms),hw(ms){
      }

    };    
   
    

  private:

    class NoMotorMode:public MotorMode{      
      void addMotor(Motors& m,int j,motor::ModeId prevMode){}
      void removeMotor(Motors& m,int j,motor::ModeId newMode){}      
      void connect(Cell::CellConnection& ref,int j){}      
      void disconnect(Cell::CellConnection& ref,int j){}      
      NormPosition getPosition(Motors& m,int j);
    public:
      NoMotorMode();
    };//NoMotorMode

    struct Conns:public Cell::CellConnection{ //the callback signals all connections to sig

      void handle(exc::Exception& e){
	sig.handle(e);
      }
      CellSignal<motor::PositionCellSignature::UT,false,true> sig;
      Conns():Cell::CellConnection(&sig){
      }
    };

    friend  class DefDescr<MotorSpooler<MotorSign> >;
  public:
    static NoMotorMode NOMOTORMODE; 
  private:
    Motors m;
    typedef std::map<motor::ModeId,MotorMode*> ModeMap;
    ModeMap modes;    
    Array<SignalledValue<typename motor::ModeId> > motorModes;        
    Array<Conns> posConns;
    typedef array::InternalAllocator<ParamIdxAllocator<MotorModeCell,MotorSpooler&,MotorSpooler&> >  ModeCellAlloc;
    typedef array::InternalAllocator<ParamIdxAllocator<MotorPositionCell,MotorSpooler&,MotorSpooler&> >  MotorPosCellAlloc;    
    typedef array::InternalAllocator<ParamIdxAllocator<MotorExactPositionCell,MotorSpooler&,MotorSpooler&> >  MotorExactPosCellAlloc;    
    Array<MotorModeCell,array::RESIZABLE,ModeCellAlloc> modeCells;

    MultipleMotorModeCell multipleCell;

    Array<MotorPositionCell,array::RESIZABLE,MotorPosCellAlloc> posCells;
    Array<MotorExactPositionCell,array::RESIZABLE,MotorExactPosCellAlloc> eposCells;
    MotorInterfaceCell ifaceCell;
    MotorInterfaceListCell ifaceListCell;

    void throwMotModeError(motor::ModeId);

    void do_getPosition(Motors& m,motor::Interface::Position&,int);
    void do_PositionConnect(Motors& m,Cell::CellConnection&,int);    
    void do_PositionDisconnect(Motors& m,Cell::CellConnection&,int);    
    void do_getMode(typename motor::ModeId&,int);
    void do_setMotorMode(Motors& m,int,const typename motor::ModeId&);
    void do_MotModeConnect(Motors& m,Cell::CellConnection&,  int);
    void do_MotModeDisconnect(Motors& m,Cell::CellConnection&,int);   
    void do_setAllMotorMode(Motors& m,const motor::ModeId& mm);   
    void do_setMultipleMotorMode(Motors& m,const HwIdList&,const typename motor::ModeId&);

    void do_registerMode(MotorMode* mm);
    void do_unregisterMode(Motors& m,MotorMode* mm);    
    void do_setIface(const motor::IfaceSelection& isel);
    

  public:   
    MotorSpooler(int n,bool autostart);  
    size_t getMotorNum();
    motor::ModeId getMotorMode(int j);
    void setMotorMode(typename motor::ModeId mode,int j);     
    void setMultipleMotorMode(const typename motor::ModeId& mode,const HwIdList& h);
    void setMotorMode(typename motor::ModeId mode);  //of all motors
    void setAndWaitMotorMode(typename motor::ModeId mode,int j);    
    void setAndWaitMultipleMotorMode(const typename motor::ModeId& mode,const HwIdList& h);
    void registerMode(MotorMode* mm);     //start the spooler before calling
    void unregisterMode(MotorMode* mm); //start the spooler before calling
    IfaceManagerSet interfaces;
    void setInterface(const motor::IfaceSelection& isel);
    void setInterface(const motor::IfaceId& iface);//all levels
    void getPosition(motor::Interface::Position& p,int j);
    void positionConnect(Cell::CellConnection& c,int j);
    void positionDisconnect(Cell::CellConnection& c,int j);
    
  };//motorSpooler

  template<typename MotorSign> class DefDescr<MotorSpooler<MotorSign> >;
    
}//helium

#include <helium/robot/motorSpooler.hpp>

#endif
