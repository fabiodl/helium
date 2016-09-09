#ifndef HE_MOTORINTERPMODENEW
#define HE_MOTORINTERPMODENEW

#include <helium/robot/motorSpooler.h>


namespace helium{

  namespace intrp{   
    enum NotificationType {NOTIFYREQUEST=1,NOTIFYREACHED=2};    
    enum TargetType {PTTARGET,RPTTARGET,PVTARGET,VTTARGET};//when adding here, modify findTTType default parameter in the .hpp
    enum TargetState{DISABLED,REACHING,REACHED};
   


    template<typename T>

    struct TargetInterface{      
      //virtual void update(T& target,Time& reachTime,T& currPos,int j)=0;
      virtual bool get(T& pos,);

      virtual void setPos(
      virtual void out(std::ostream& o)=0;
      virtual void in(std::istream& i)=0;
      virtual void finalize()=0;
      virtual intrp::TargetType getTypeID()=0;
      virtual ~TargetInterface(){}
    };


    template<typename T,typename ME,bool D> class TargetOption:public TargetInterface<T>{ //D= auto delete
      void out(std::ostream& o);
      void in(std::istream& i);
      void finalize(){}
      intrp::TargetType getTypeID();
    };

    template<typename T,typename ME> class TargetOption<T,ME,true>:public TargetInterface<T>{
      void out(std::ostream& o);
      void in(std::istream& i);
      void finalize(){delete this;}
      intrp::TargetType getTypeID();
    };

    template<typename T> class Target;//fw decl

    template<typename T>
    std::ostream& operator<<(std::ostream& o,const Target<T>& t);//fw decl
      

    template<typename U,intrp::TargetType TT> inline void streamIn(std::istream& i,Target<U>& t);//fw decl

    template<typename T>
    class Target{
      friend std::ostream& operator<< <>(std::ostream& o,const Target<T>& t);
      template<typename U,intrp::TargetType TT> friend inline void streamIn(std::istream& i,Target<U>& t);
      TargetInterface<T>* real;    
    public:
      operator const TargetInterface<T>*(){return real;}
      Target():real(NULL){}
      Target& operator=(TargetInterface<T>* ti){real=ti;return *this;}
      TargetInterface<T>* dataPtr() const{return real;}
    };
    
    template<typename T>
    std::ostream& operator<<(std::ostream& o,Target<T>& t);
    template<typename T>
    std::istream& operator>>(std::istream& i,Target<T>& t);
   
    template<typename T> struct TargetProps{
      TargetState state;
      T val;
      //Time remaining;
    };

    template<typename T> std::ostream& operator<<(std::ostream& o,const TargetProps<T>& tp);
    template<typename T> std::istream& operator>>(std::istream& i,TargetProps<T>& tp);

    template<typename T> struct TargetNotification{
      NotificationType notifyType;
      T val;
    };

    template<typename T> std::ostream& operator<<(std::ostream& o,const TargetNotification<T>& tn);
    template<typename T> std::istream& operator>>(std::istream& i,TargetNotification<T>& tn);
    
    template<typename T,bool D,intrp::TargetType TT> struct getTTClass{
    };


    // ----------Target types begin-----------
    
    //PT
   
    template<typename T,bool D> struct PositionTimeTarget:public TargetOption<T,PositionTimeTarget<T,D>,D >{
      T val;
      Time t;            
      void update(T& target,Time& reachTime,T& currPos,int j){
	target=val;
	reachTime=t;
      }          
    };

    template<typename T,bool D> 
    std::ostream& operator<<(std::ostream& o,const PositionTimeTarget<T,D>& pt){
      return o<<pt.val<<" "<<pt.t;
    }

    template<typename T,bool D> 
    std::istream& operator>>(std::istream& i,PositionTimeTarget<T,D>& pt){
      read(pt.val,i);
      return i>>pt.t;
    }

    template<typename T,bool D> struct getTTClass<T,D,PTTARGET>{
      typedef PositionTimeTarget<T,D> value;
    };

    //RPT

    template<typename T,bool D> struct RelativePositionTimeTarget:public TargetOption<T,RelativePositionTimeTarget<T,D>,D >{
      T val;
      Time t;      
      
      void update(T& target,Time& reachTime,T& currPos,int j){
	target=currPos+val;
	reachTime=t;
      }          
    };

    template<typename T,bool D> 
    std::ostream& operator<<(std::ostream& o,const RelativePositionTimeTarget<T,D>& pt){
      return o<<pt.val<<" "<<pt.t;
    }

    template<typename T,bool D> 
    std::istream& operator>>(std::istream& i,RelativePositionTimeTarget<T,D>& pt){
      read(pt.val,i);
      return i>>pt.t;
    }


    template<typename T,bool D> struct getTTClass<T,D,RPTTARGET>{
      typedef RelativePositionTimeTarget<T,D> value;
    };

    //PV

    template<typename T,bool D> struct PositionVelocityTarget:public TargetOption<T,PositionVelocityTarget<T,D>,D >{
      T val;
      double v; //in units/ms
      void update(T& target,Time& reachTime,T& currPos,int j){
	target=val;
	reachTime=tAbs(val-currPos)/v;
      }
    };

    template<typename T,bool D> 
    std::ostream& operator<<(std::ostream& o,const PositionVelocityTarget<T,D>& pt){
      return o<<pt.val<<" "<<pt.v;
    }

    template<typename T,bool D> 
    std::istream& operator>>(std::istream& i,PositionVelocityTarget<T,D>& pt){
      return i>>pt.val>>pt.v;
    }

    template<typename T,bool D> struct getTTClass<T,D,PVTARGET>{
      typedef PositionVelocityTarget<T,D> value;
    };

    //VT
    
    template<typename T,bool D> struct VelocityTimeTarget:public TargetOption<T, VelocityTimeTarget<T,D>,D >{
       double v;
       Time t;
       void update(T& target,Time& reachTime,T& currPos,int j){
	 target=currPos+v*t;
	 reachTime=t;
       }
    };

    template<typename T,bool D> 
    std::ostream& operator<<(std::ostream& o,const VelocityTimeTarget<T,D>& pt){
      return o<<pt.v<<" "<<pt.t;
    }

    template<typename T,bool D> 
    std::istream& operator>>(std::istream& i,VelocityTimeTarget<T,D>& pt){
      read(pt.v,i);
      return i>>pt.t;
    }

    template<typename T,bool D> struct getTTClass<T,D,VTTARGET>{
      typedef VelocityTimeTarget<T,D> value;
    };


    // ----------Target types end-----------
    
    namespace spt{
      template<typename PT,typename PS,motor::MotorLevel L> struct tmAdapter{      
      };
    }   

    template<typename Position>
    struct TargetCellSignature:public CellSignature{
      typedef  intrp::TargetProps<Position> GT;
      typedef Void GP;
      
      typedef intrp::Target<Position> ST;
      typedef intrp::TargetNotification<Position> SP;
      
      typedef intrp::TargetNotification<Position> UT;
      typedef int UP;

      static const bool getRequiresParam=false;
      static const bool setRequiresParam=false;
      static const bool connectRequiresParam=false;
      
      typedef DefaultAccessPolicy<GT,GP,ST,SP,UT,UP,
				  getRequiresParam,setRequiresParam,connectRequiresParam> Access;

    };

  }//ns intrp

  
 

  
  template<typename T> class DefDescr;//fw decl

  template<typename PT,typename PS,motor::MotorLevel L> class InterpMode:public MotorSpooler<PT,PS>::MotorMode,
									   private Job<typename MotorSpooler<PT,PS>::Motors>,
									   private Callback<void>{
    

    typedef MotorSpooler<PT,PS> Spool;
    typedef intrp::spt::tmAdapter<PT,PS,L> tmAdapter;

    typedef typename tmAdapter::Motors Motors;
    typedef typename tmAdapter::Motor Motor;
    typedef typename tmAdapter::Position Position;

    class TargetInfo{
      friend class InterpMode;
      friend class TargetCell;

      intrp::TargetState state;      

      //Position origin;
      Position currPos;
      //Position target;
      
      //Time zeroTime;
      //Time reachTime;
      
      Signal<int> newTarget;
      Signal<int> targetReached;

      ExceptionSignal onError;
      int j;

      EhSignal<Cell::CellSignalData> hlPosition;
      
      void getProps(intrp::TargetProps<Position>* t);
      void updatePosture(Motors& m,Time now);
      void setNew(intrp::TargetInterface<Position>* tar);
  
    public:
      TargetInfo(int pj);
                      
    };//TargetInfo

    class ReachingSet{
      std::set<int> onReaching;
    public:

      Signal<void> transitionToActive; 
      Signal<void> transitionToInactive; 

      const std::set<int>& getSet(){return onReaching;}
      inline size_t size(){return onReaching.size();}

      void insert(int j);
      void erase(int j);      



      typedef scb::ClassCallback<int,ReachingSet,&ReachingSet::insert> Inserter;
      typedef scb::ClassCallback<int,ReachingSet,&ReachingSet::erase> Eraser;
      Inserter inserter; 
      Eraser eraser;  
               
                
      ReachingSet():inserter(this),eraser(this){
      }

    };//ReachingSet
    
    class TargetCell:public TypedCell<intrp::TargetCellSignature<Position> >{
      typedef std::pair<Callback<intrp::TargetNotification<Position>* >*,intrp::TargetInterface<Position>*> ReachParam; 
      
      class NotificationBuilder:public Callback<int>{
	TargetCell& owner;
	intrp::TargetNotification<Position> n;	       	
	void operator()(int j);	
      public:
	Signal<Cell::CellSignalData> eventSignal;
	NotificationBuilder(TargetCell& powner,intrp::NotificationType nt);
      };//NotificationBuilder


      InterpMode& im;
      int j;

      Signal<intrp::TargetNotification<Position>*> oneShot; //for the setData

      NotificationBuilder onNewTarget;
      NotificationBuilder onTargetReached;
      
      void do_setData(ReachParam &p);
      void do_connect(std::pair<Cell::CellConnection*,int>& p);
      void do_disconnect(Cell::CellConnection* ref);
      
      std::string getName() const;      
      void getData(intrp::TargetProps<Position>& t);
      void setData(const intrp::Target<Position>& t);
      void setData(intrp::TargetNotification<Position>& n,const intrp::Target<Position>& t);     
      void connect(Cell::CellConnection& ref){ connect(ref,intrp::NOTIFYREQUEST|intrp::NOTIFYREACHED);}
      void connect(Cell::CellConnection& ref,const int& flags);
      void disconnect(Cell::CellConnection& ref);
    public:
      TargetCell(InterpMode& pim,int pj);      
    };//TargetCell
    
    class WholeTargetCell:public TypedCell<Array<intrp::TargetProps<Position> >,Void,
					   intrp::Target<Position>,int,//return the joint that was changed, or -1 if the whole reaching is complete
					   intrp::NotificationType,int>,public Callback<void>,public Callback<int>{ 
      
      typedef std::pair<Callback<int>*,intrp::TargetInterface<Position>*> ReachParam;

      template<intrp::NotificationType NT> class NotificationBuilder:public Callback<void>{//when becomes active, inactive, a signal
	static const intrp::NotificationType value=NT; 
	void operator()(){eventSignal(&value);}
      public:
	Signal<Cell::CellSignalData> eventSignal;      
      };
     
      InterpMode &im;
      Signal<int> oneShot;//for the setData
      NotificationBuilder<intrp::NOTIFYREQUEST> onRequest;
      NotificationBuilder<intrp::NOTIFYREACHED> onReached;
    
      void do_getData(Array<intrp::TargetProps<Position> >* pos);      
      void do_setData(intrp::TargetInterface<Position>*  pos);
      void do_reachPosition(ReachParam& p);
      void do_connect(std::pair<Cell::CellConnection*,int> &p);
      void do_disconnect(Cell::CellConnection *c);

      std::string getName() const;     
      void getData(Array<intrp::TargetProps<Position> >& pos);                 
      void setData(const intrp::Target<Position>& pos);
      void setData(int &n,const intrp::Target<Position>& pos);             
      void connect(Cell::CellConnection& ref,const int& flags);      
      void connect(Cell::CellConnection& ref){connect(ref,intrp::NOTIFYREQUEST|intrp::NOTIFYREACHED);}           
      void disconnect(Cell::CellConnection& ref);

      void operator()(){ oneShot.signalAndClear(-1);}//for the transitionToInactive
      void operator()(int j){ oneShot.signalAndClear(j);}// when one target is changed
            
    public:
      
      WholeTargetCell(InterpMode& pim);
      
    };//WholeTargetCell
    
    friend class helium::DefDescr<InterpMode<PT,PS,L> >;

    Spool& ms;
    int n;

    ReachingSet onReaching;
    std::set<int> controlledMotors; //used by wholetargetcell::set
    ExceptionSignal anyError;

    Array<TargetInfo,array::RESIZABLE,array::InternalAllocator<IdxAllocator<TargetInfo> > > info;
    Array<TargetCell,array::RESIZABLE,array::InternalAllocator<ParamIdxAllocator<TargetCell,InterpMode&,InterpMode&> > > targetCells;
    WholeTargetCell wholeTarget;

    void operator()(){ ms.addJob(this,Spool::NORMAL);}//when becomes active   

    void addMotor(Motors& m,int j,motor::ModeId prevMode);
    void removeMotor(Motors& m,int j,motor::ModeId prevMode);
    
    
    motor::Interface::Position getPosition(Motors& m,int j);
    void connect(Cell::CellConnection& ref,int j);    
    void disconnect(Cell::CellConnection& ref,int j);
    bool operator()(Motors& m);        
  public:
    InterpMode(Spool& pms,int pn);          
  };//InterpMode
    
  template<typename PT,typename PS,motor::MotorLevel L> class DefDescr<InterpMode<PT,PS,L> >;

}//helium

#include <helium/robot/motorInterpModeNew.hpp>
#endif
