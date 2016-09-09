/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_MOTORACTIVEMODE
#define HE_MOTORACTIVEMODE

#include <helium/robot/motorSpooler.h>
#include <helium/signal/valueWaiter.h>

namespace helium{
  
  
  

  enum TargetEndReason{TARGETREACHED,TARGETCHANGED,TARGETERROROCCURRED};


  template<typename Target> struct TargetCellBaseSignature:public CellSignature{
    typedef Void GT; 
    typedef Void GP; 
    typedef Target ST;
    typedef TargetEndReason SP; 
    typedef Void UT;
    typedef Void UP;
    
    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;
    
    
  };
  



  template<typename MotorSign,typename Mode,typename Target> 
  class TargetCellVirtualBase:
    public TypedCell<TargetCellBaseSignature<Target> >{      
  protected:

    typedef MotorSpooler<MotorSign> Spool;
    typedef typename Spool::Motors Motors;
    typedef std::pair<Callback<TargetEndReason>*,const Target*> ReachTargetParam;

    Mode& owner;   

    std::string getName() const;     
    void do_setWholeTarget(Motors& m,const Target& st);       
    void do_reachWholeTarget(Motors& m,const ReachTargetParam* t);    



  public:
    void setData(const Target& st);      
    void setData(TargetEndReason& sp,const Target& st);

    TargetCellVirtualBase(Mode& powner):owner(powner){
    }
    
  };

  //this calls owner->getWholeTarget/ getJointTarget to construct the target  
  template<typename MotorSign,typename Mode,typename Target> 
  class TargetCellBase:public TargetCellVirtualBase<MotorSign,Mode,Target>{            
    void streamSet(std::istream& i);    
    void streamSet(std::ostream& o,std::istream& i);
  public:
    TargetCellBase(Mode& powner):TargetCellVirtualBase<MotorSign,Mode,Target>(powner){}    
  };//TargetCellBase


  
  template<typename MotorSign,typename Mode,typename JointTarget> 
  class JointTargetCellBase:
    public TypedCell<TargetCellBaseSignature<JointTarget> >{      
  protected:
    Mode& owner;   
    int j;

    typedef MotorSpooler<MotorSign> Spool;
    typedef typename Spool::Motors Motors;       
    typedef std::pair<Callback<TargetEndReason>*,const JointTarget*> ReachTargetParam;

    std::string getName() const;
             
    void do_setJointTarget(Motors& m,const JointTarget& st);    
    void setData(const JointTarget& st);    
    void streamSet(std::istream& i);
        
    void do_reachJointTarget(Motors& m,const ReachTargetParam* t);
    void setData(TargetEndReason& sp,const JointTarget& st);
    void streamSet(std::ostream& o,std::istream& i);
    
  public:
    JointTargetCellBase(Mode& powner,int pj);
    
  };




  //TargetCells has (if the type is not void) members TargetCell wholeTarget and  Array<JointTargetCell> jointTarget

  template<typename Mode,           //mode is the owner, which is passed to the constructor o of the objects 
	   typename TargetCell,     //this is the type or void to set none
	   typename JointTargetCell //this is the type or void to set none
	   >
  struct TargetCells; //we need the definition underneath because motorActiveMode has a TargetCells field
  
  template<typename Mode>
  struct TargetCells<Mode,void,void>{    
    TargetCells(Mode& owner,int n){}
  };
  
  template<typename Mode,typename TargetCell>
  struct TargetCells<Mode,TargetCell,void>{
    TargetCell wholeTarget;
    TargetCells(Mode& owner,int n):wholeTarget(owner){}
  };

  template<typename Mode,typename JointTargetCell>
  struct TargetCells<Mode,void,JointTargetCell>{
    Array<JointTargetCell,
	  array::RESIZABLE,
	  array::InternalAllocator<
	    ParamIdxAllocator<JointTargetCell,Mode&>
	    > > jointTarget;
  public:
    TargetCells(Mode& owner,int n):jointTarget(owner,n){}
  };

  template<typename Mode,typename TargetCell,typename JointTargetCell>
  struct  TargetCells:
    public TargetCells<Mode,TargetCell,void>,
    public TargetCells<Mode,void,JointTargetCell>	 
  {    
    TargetCells(Mode& owner,int n):TargetCells<Mode,TargetCell,void>(owner,n),
				   TargetCells<Mode,void,JointTargetCell>(owner,n){
    }			     
  };

  
  
  
  template<typename MotorSign,motor::MotorLevel L> struct mpAdapter;
  



  template<typename MotorSign> struct mpAdapter<MotorSign,motor::IFACELEVEL>{
    typedef MotorSpooler<MotorSign> Spool;
    typedef typename motor::Interface Motor;
    typedef typename motor::Interface::Position Position;
    typedef typename MotorSpooler<MotorSign>::Motors Motors;    
    static const motor::ModeId modeId;

    static void rotate(Motors& m,bool* moving,Position* newPos);
    static void getPosture(Motors& m,Position* p);  
    static Position getPosture(Motors& m,int j);
    static void setCurrentPosition(Motors& m,int j);
    inline static motor::Interface::Position normalize(Motors& m,int j,Position& p);

    };


  template<typename MotorSign> struct mpAdapter<MotorSign,motor::RAWLEVEL>{
    typedef typename motor::Raw<MotorSign> Motor;
    typedef typename MotorSign::Pos Position;
    typedef typename MotorSpooler<MotorSign>::Motors Motors;
    
    static const motor::ModeId modeId;

    inline static void rotate(Motors& m,bool* moving,Position* newPos);
    inline static void getPosture(Motors& m,Position* p);  
    inline static Position getPosture(Motors& m,int j);
    inline static void setCurrentPosition(Motors& m,int j);    
    inline static motor::Interface::Position normalize(Motors& m,int j,Position& p);
 
    };  


  template<typename MotorSign> struct mpAdapter<MotorSign,motor::HWLEVEL>{
      typedef typename motor::Raw<MotorSign> Motor;
      typedef typename MotorSign::Pos Position;
      typedef typename MotorSpooler<MotorSign>::Motors Motors;
      
      static const motor::ModeId modeId;

    inline static  void rotate(Motors& m,bool* moving,Position* newPos);  
    inline static  void getPosture(Motors& m,Position* p);
    inline static Position getPosture(Motors& m,int j);
    inline static void setCurrentPosition(Motors& m,int j);
    
    inline static motor::Interface::Position normalize(Motors& m,int j,Position& p);

    };


  template<typename MotorSign,typename Mode,typename Target> 
  std::string TargetCellVirtualBase<MotorSign,Mode,Target>::getName() const{
      return "whole target cell";
    }

  template<typename MotorSign,typename Mode,typename Target> 
  void TargetCellVirtualBase<MotorSign,Mode,Target>::do_setWholeTarget(Motors& m,const Target& st){
      if (!owner.controlledMotors){
	std::cerr<<"Warning: setting target with no controlled motor"<<std::endl;
      }
      owner.wholeTargetEnd.signalAndClear(TARGETCHANGED);
      owner.setWholeTarget(m,st);
     
    }
  

  




  /*Super must have the member
    void getPosture(bool* newMoving,adapter::Position* ,MotorSpooler<MotorSign>::Motors &m);     
    
    setWholeTarget(Motors& m,const Target& t); 
    setJointTarget(m,st,j);


    ModeSignature constains two typedefs,
    TargetCell,and JointTargetCell, used for defining  TargetCells<Super,
		typename ModeSignature::TargetCell,
		typename ModeSignature::JointTargetCell> cells;
		
    
   */

  template<typename MotorSign,motor::MotorLevel L,
	   typename Super,typename ModeSignature
	   >
    class MotorActiveMode:public MotorSpooler<MotorSign>::MotorMode,		       
			   private Job<typename MotorSpooler<MotorSign>::Motors>			  
			  
  {

  protected:
    typedef mpAdapter<MotorSign,L> adapter;
    typedef typename adapter::Position P;
    typedef MotorSpooler<MotorSign> Spool;
    typedef typename   MotorSpooler<MotorSign>::Motors Motors;



     struct JointInfo{
	int n;
	P *currPos, *newPos;
	bool *currMoving, *newMoving,*mask;

       CellSignal<motor::PositionCellSignature::UT,false> *hlPos;
       Signal<TargetEndReason>* targetEnd;

    
       JointInfo(int pn);
       ~JointInfo();
     };
        
  public: //for the cells
    Spool& spool;
    JointInfo info;
    Signal<TargetEndReason> wholeTargetEnd;
    TargetCells<Super,
		typename ModeSignature::TargetCell,
		typename ModeSignature::JointTargetCell> cells;
    
    int controlledMotors;
  private:
    bool isMoving;    
    void do_assureSpool();
    bool operator()(Motors& m);
    bool isToCancel();
    void updateControlledMotors();           

    motor::Interface::Position getPosition(Motors& m,int j);    
    void connect(Cell::CellConnection& ref,int j);        
    void disconnect(Cell::CellConnection& ref,int j);

       
  protected:
    void addMotor(Motors& m,int j,motor::ModeId prevMode);    
    void removeMotor(Motors& m,int j,motor::ModeId prevMode);
    
    void assureSpool();
    
  public:
    MotorActiveMode(Spool& pspool,motor::ModeId motorMode);
    

  };//MotorActiveMode
  

  template<>  struct EnumStrings<TargetEndReason>{
      static const int NUM=3;
    static const char* strings[NUM];
  };



}//helium


#include <helium/robot/motorActiveMode.hpp>
#endif
