/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_FRAMEPLAYMOTORMODE
#define HE_FRAMEPLAYMOTORMODE

#include <helium/robot/motionParameter.h>
#include <helium/robot/motorActiveMode.h>
#include <helium/robot/timeSource.h>

namespace helium{




  template<typename MotorSign>
  class MotorFramePlayMode;

  namespace framePlay{
    


    struct Target{
      MotionParameter mp;

      std::vector<Time> ticks;           
      Array<std::vector<std::pair<int,motor::Interface::Position> > > frames;
      void clear();
     
      Target(size_t n):frames(n){}
      Target(){} //for reading
    };


    inline std::ostream& operator<<(std::ostream& o,const Target& t);


    inline std::istream& operator>>(std::istream& i,Target& t);

  
    
    template<typename MotorSign>
    struct ModeSign{
      typedef framePlay::Target Target;
      typedef void JointTarget;
      typedef TargetCellVirtualBase<MotorSign,MotorFramePlayMode<MotorSign>,Target> TargetCell;
      typedef void JointTargetCell;
    };

    




  class RequestStop{    
  };
  
  inline std::ostream& operator<<(std::ostream& o,const RequestStop& s){
    return o;
  }

  inline std::istream& operator>>(std::istream& i,RequestStop& s){
    return i;
  }

    enum PlayState{PLAYING,STOPPING,STOPPED};


  struct FramePlayControlSignature:public CellSignature{
    typedef std::pair<PlayState,Time> GT;
    typedef Void GP;
    
    typedef RequestStop ST; 
    typedef Void SP;
    
    typedef GT UT;
    typedef Void UP;

    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;
    
  };
  }//framePlay

  template<typename MotorSign>
  class MotorFramePlayMode:public MotorActiveMode<MotorSign,motor::IFACELEVEL,
					     MotorFramePlayMode<MotorSign>,framePlay::ModeSign<MotorSign>					     
					     >{
    typedef MotorSpooler<MotorSign> Spool;
    typedef typename Spool::Motors Motors;
    typedef motor::Interface::Position Position;
    typedef MotorActiveMode<MotorSign,motor::IFACELEVEL,
					     MotorFramePlayMode<MotorSign>,framePlay::ModeSign<MotorSign>					     
			    > Base;

    framePlay::Target target;
    bool running;
    int currIter;
    int wholeNext,wholePrev;
    Array<int> jointNext,jointPrev;

    TimeSource& time;
    Time zeroTime;
    Time endTime;    
    Time now;//for the prev time when stopping
   

    void resetCursors();
    
 public:

    static const int wholeTargetSetPriority=MotSpoolPriority::NORMAL;
    static const int jointTargetSetPriority=MotSpoolPriority::NORMAL;
    static const int spoolingPriority=MotSpoolPriority::NORMAL;


    class ControlCell:public TypedCell<framePlay::FramePlayControlSignature>{
      MotorFramePlayMode& owner;
      SignalledValue<std::pair<framePlay::PlayState,Time> > state;
      
      std::string getName() const;

      void setData(const framePlay::RequestStop& r);      
      void do_getData(std::pair<framePlay::PlayState,Time>& p);
      
      void getData(std::pair<framePlay::PlayState,Time>& p);
      void connect(Cell::CellConnection& c);

      void disconnect(Cell::CellConnection& c);

    public:

      ControlCell(MotorFramePlayMode& powner);

      void setCurrFrame(Time t);
      void setPlayingState(framePlay::PlayState p);
      
      void setState(framePlay::PlayState p,Time t);

      bool isStopping();

    };
    //DataCell<Time> currFrame;
    //DataCell<bool> playing;


    ControlCell stateCell;
    

    void setWholeTarget(Motors& m,const framePlay::Target& t); 
    void getPosture(bool* newMoving,Position* newPos,Motors&);
    
    static motor::ModeId modeID();
    
    MotorFramePlayMode(Spool& pspool,TimeSource& ptime);

  };//MotorFramePlayMode



  template<>  struct EnumStrings<framePlay::PlayState>{
    static const int NUM=3;
    static const char* strings[NUM];
  };
	
  template <typename MotorSign>
  struct DefDescr<MotorFramePlayMode<MotorSign> >:public Description<MotorFramePlayMode<MotorSign> > { 

    DefDescr(MotorFramePlayMode<MotorSign>& mlm);
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
    
  };

}//helium;



    /*class TargetCell:public TargetCellVirtualBase<MotorSign,MotorFramePlayMode,Motion>{


      
      void streamSet(std::istream &i){
	Motion motion(this->owner.info.n);
	i>>motion;
	setData(motion);
      }

	  
      void streamSet(std::ostream& o,std::istream& i){      
	TargetEndReason sp;
	Motion motion(this->owner.info.n);
	i>>motion;
	setData(sp,motion);      
      }//streamSet
      
      };*/

#include <helium/robot/motorFramePlayMode.hpp>

#endif
