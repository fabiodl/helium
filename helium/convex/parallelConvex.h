#include <helium/core/time.h>
#include <helium/robot/frame.h>
#include <helium/signal/signal.h>
#include <helium/spooling/spooler.h>


namespace helium{

  typedef Array<double> RobotConfig;

class Segment{
  static int nextId;    
  Time step,targetStep;
  int i, maxi;
  
public: 
 typedef int Id;
  Id id;
  Time start;
  RobotConfig posture;
  enum CheckState{OK,BAD,ONCHECK} checkState;
  Time getWorkTime(); 
  Segment(Time start,const RobotConfig& startPosture);//for the last sentinel frame
  Segment(Time start,const RobotConfig& startPosture,Time end,const RobotConfig& endPosture,double maxStep);
  void reset(Time end,const RobotConfig& endPosture,double maxStep);
  static int peekNextId();
};






class JobManager{

public:  
  

  JobManager (double MaxStep,int motorNum);

  void setMotion(const helium::Motion& m);
  Signal<Time> collidingPosture;
  Signal<void> segmentStateChange;


  typedef Array<RobotConfig> Work;
  typedef Array<RobotConfig,array::RESIZABLE,array::LINK> WorkLink;
  typedef int WorkerId;
  static const int INVALIDWORKID=-1;

  WorkerId getWorkerId(int desiredSize);

  WorkLink getWork(WorkerId id);

  void report(const std::pair<Time,WorkerId>& collision);    

private:

  class Worker{
  public:
    Segment::Id segmentId;
    Array<Time > times;
    Work work;
    WorkerId isRecovering;//own work=-1, other's work=original worker id
    Worker(int workSize,int configSize);
    void invalidate();
    int getWorkNum();
  };



  class State{       
    typedef std::list<Segment > SegList;  
    SegList currSegs;
    typedef std::list<SegList::iterator> PtrList;
    PtrList onCheck;
    PtrList::iterator cyclicIterator;
  public:
   void setMotion(const helium::Motion& m);
    bool markBadSegment(Segment::Id id);//returns if the segment was still in use
    void markGoodSegment(PtrList::iterator);
    size_t fillWorks(Worker&);//return the number of valid works
    JobManager& owner;
    State(JobManager& owner);
  };

  
  class Sleepers{
    JobManager& owner;
    std::list<Job<State>*> sleepers;      
  public:
    void queue(Job<State>*);
    void wake();
    Sleepers(JobManager& owner);
  };
  

  class WorkGetterJob:public WaitableJob<JobManager::State>{
    Semaphore sem;
    JobManager::Worker& w;
    JobManager::WorkLink wl;    
    bool operator()(JobManager::State& s);
    bool isToCancel();
    void wait();
  public:
    WorkGetterJob(JobManager:: Worker& pw);
  };


  void copyFirstWork(JobManager::Worker& w);

  
  void do_setMotion(State& s,const helium::Motion& m);
  void do_report(JobManager::State& s,const std::pair<Time,WorkerId>& n);
  enum {WORKERPRIORITY,MOTIONCHANGEPRIORITY,SPOOLERPRIORITIESNUM};
  State state;
  std::vector<Worker> workers;

  Sleepers sleepers;
  friend class Sleepers;
  friend class WorkGetterJob;
  Spooler<State> spool;
  double maxStep;
  int motorNum;
};


}//ns
