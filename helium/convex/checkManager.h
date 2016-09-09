/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <memory>
#include <list>
#include <set>
#include <helium/vector/array.h>
#include <helium/thread/semaphore.h>
#include <helium/signal/signal.h>
#include <helium/gtk/gtkSignal.h>
#include <helium/core/time.h>
#include <helium/robot/frame.h>
#include <helium/convex/convexModel.h>
namespace coldet{
  
  enum TurboSetting{TurboOFF,TurboUNDECIDED,TurboON};
  
  class TimeNotification{
    helium::Time t;
    bool isColliding;
  };
  
  class SegmentState{
  public:
    static const helium::Time NOCOLLISION=-1,CHECKING=-2;
    //enum SegmentStateType{OKSTATE,BADSTATE,CHECKINGSTATE};
    //SegmentStateType state;

    helium::Time start,end,coll;
    //SegmentState(){}
    SegmentState(helium::Time b,helium::Time e,helium::Time c):
      start(b),end(e),coll(c){}    
  };

  

  typedef std::vector<SegmentState> SegmentNotification;
}//ns coldet



namespace helium{
std::ostream& operator<<(std::ostream& o,const coldet::SegmentState&);
  
namespace convexCheck{







  static const double UNKNOWN=2,NONE=4;//chosen "high" so that min(invalid,val) returns val
  class Segment{
  public:
    Array<double> begin,end;
    mutable double collTime; 
    Segment(const double* b,const double* e,int n);
  };


class SegmentCheck;
//workers should set collTime,and then just delete the work

class Work{
  protected:
    SegmentCheck& check;
  public:
    std::list<Work*>::iterator it;
    enum State {ONWORK,REPLACED,DEAD} state;
    const Segment* s;    
    double collTime;
    Work(SegmentCheck& pwm,const Segment* ps);
    virtual double getNext()=0;
    virtual Work* clone()=0;
    ~Work();
  };



  class SegmentCheck{
  protected:
    Mutex segMutex;
    std::list<Work*> onWork;
    Callback<double>* stateChanged;
    bool isProcessingEnded();
  public:
    const Segment* s;
    void report(Work*);
    bool getGhost(std::auto_ptr<Work>& work);
    virtual void endWork()=0;//called when a collision was found
    virtual bool isWorkGenerationEnded()=0;
    virtual void pushCollision(double)=0;    
    SegmentCheck(const Segment* s,Callback<double>* cb);
    virtual ~SegmentCheck(){}
    void disconnect();//disconnect
    void connect(Callback<double>* cb);
    bool canBeDeleted();
  };

  class LinearCheck:public SegmentCheck{
    int next;    
    static const double MAXDELTA;
  public:
    const int divisions;
    bool getWork(std::auto_ptr<Work>& work,int desired);  
    bool isWorkGenerationEnded();
    void pushCollision(double);
    LinearCheck(const Segment* s,Callback<double>* cb);
    void endWork();
  };


  class BisectionCheck:public SegmentCheck{
    static const double MAXDELTA;
    int p;//used to compute powers
    int divisions,next;
  public:

    bool getWork(std::auto_ptr<Work>& work,int desired);  
    bool isWorkGenerationEnded();
    void pushCollision(double);
    void endWork();
    BisectionCheck(const Segment* s,Callback<double>* cb);
  };



class BisectionWork:public Work{
  int starti,startlevel,i,maxi;      
  int count;
  double step;
public:
  BisectionWork(int pi,int level,int pcount,SegmentCheck& check,const Segment* s);
  double getNext();
  Work* clone();
};

class LinearWork:public Work{
  int start,i;    
  int end,divisions;
public:
  LinearWork(int pstart,int pend,int pdivisions,SegmentCheck& check,const Segment* s);    
  double getNext();
  Work* clone();
};

  class TransitionCheck{
    Mutex transCheckMutex;
    LinearCheck* currCheck;
    std::list<LinearCheck*> oldChecks;

    Time ghostCheck;
    int desired;
    static const Time GHOSTTIME=2000;
  public:
    void setExpectedThreads(int n);
    bool getWork(std::auto_ptr<Work>& work);
    void startCheck(const Segment* s,Callback<double>* cb);
    void cancel();
    TransitionCheck();
    void cleanup();
  };


  class MotionCheck{
    //void check();
    Mutex motCheckMutex;
    typedef std::list<std::pair<BisectionCheck*,bool> > CheckList;
    
    CheckList checks;
    std::list<BisectionCheck*> oldChecks;
    CheckList::iterator it;
    int desired;
  public:
    MotionCheck();
    

    void beginCheckList();
    void addCheck(const Segment*s ,Callback<double> *cb);
    void endCheckList();
    

    bool getWork(std::auto_ptr<Work>& work);
    void cleanup();
  };

  
}//
  


  class CheckManager:public Callback<double>{
    class FullCheck{
    public:
      bool operator()(const convexCheck::Segment& f,const convexCheck::Segment& s) const;
      
    };
    
    /*class NoOrderCheck{
      public:
      static bool operator()(const convexCheck::Segment& f,const convexCheck::Segment& s) const;
      };
      //we assume they're inserted in order
      */
    
    int n;
    Mutex checkManMutex;
    int waiters;
    bool running;
    std::set<convexCheck::Segment,FullCheck> linSegments;
    std::set<convexCheck::Segment,FullCheck> binSegments;
    convexCheck::TransitionCheck transitionCheck;
    convexCheck::MotionCheck motionCheck;
    //there's one notificationSegment for each motionNotification
    coldet::SegmentNotification motionNotification;

    class NotificationInfo{
    public:
      const convexCheck::Segment* s;
      bool reverse;
      NotificationInfo(const convexCheck::Segment* ps,bool preverse):
	s(ps),reverse(preverse){
      }
    };

    std::vector<NotificationInfo> notificationInfo;
    std::list<const convexCheck::Segment*> edgeCollidingSegments;
    void operator()(double);
    Semaphore changeOccurred;
    convex::ConvexCollisionModel ccm;
  public:
    CheckManager(int motorN,convex::ConvexCollisionModel& ccm);
    ~CheckManager();
    coldet::SegmentNotification getMotionNotification();
    bool getWork(std::auto_ptr<convexCheck::Work>& wp);  

    void checkTransition(Callback<double>* cb,const double* start,const double* end);
    void cancelTransition();
    void checkMotion(const Motion::MotionMap& m);    
    GtkSignal<coldet::SegmentNotification,coldet::SegmentNotification,false,true> notifications;
    void stop();

    helium::Time getFirstCollisionTime(); ///< returning the first frame the CheckManager found collision at, -100 if none
  };//checkmanager
}//ns helium

//inline std::ostream& operator<<(std::ostream& o,const coldet::SegmentState::SegmentStateType& p){
//  switch(p) {
//  case coldet::SegmentState::OKSTATE: o << "ok"; break;
//  case coldet::SegmentState::CHECKINGSTATE: o << "checking"; break;
//  case coldet::SegmentState::BADSTATE: o << "bad"; break;
//  default: o << "xxx";
//  }
//  return o;
//}
