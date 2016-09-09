#ifndef HE_MOTORPOLICYMODE
#define HE_MOTORPOLICYMODE

#include <helium/robot/motorSpooler.h>



namespace helium{
  
  namespace helium{
    typedef std::string PolicyID;
  };


  class ActivityNotification{
    int j;
    bool b;
  };

  class ExtendedPolicy{
  public:   
    TsSignal<ActivityNotification> activeMotors;
    void addMotor(int j,motor::Interface::Position currPos)=0;
    void removeMotor(int j)=0;  
    void getPosture(motor::Interface::Position* p)=0;
  };
  


  
  class StillPolicy:public ExtendedPolicy{
    Array<motor::Interface::Position> pos;
  public:
    StillPolicy(int n):pos(n){
    }

    void addMotor(int j,motor::Interface::Position currPos){
      pos[j]=currPos;
    }

    int getPosture(bool* finished, motor::Interface::Position* p){
      assign(p,pos);
      return 0;
    }

  };


  



template<typename PT,typename PS>
class MotorPolicyMode:public MotorSpooler<PT,PS>::MotorMode{
  typedef MotorSpooler<PT,PS> Spool;
  typedef typename Spool::Motors Motors;

  typedef motor::Interface::Position NormPosition;
  Spool& ms;

  std::set<int> onReaching,enabled;
    
  static const int POTENTIOCHECKS=1;
  
  class MotorInfo{
  public:
    enum State{DISABLED,MOVING,IDLE};
    State state;
    SignalledValue<NormPosition> signalledPos;
    MotorInfo():state(DISABLED){
    }    
  };
    
  Array<MotorInfo> info;
  std::map<motor::PolicyID,ExtendedPolicy*> policies;


  ExtendedPolicy* currPolicy;



  void selectPolicy(const PolicyID& pid){
    std::map<PolicyID,ExtendedPolicy*>::iterator it=
      policies.find(pid);
    if (it=policies.end()) throw exc::InvalidValue(toString(pid)+" is not a registered policy. Choose one of "+
						   keysToString(policies));
    currPolicy=it.second;    
  }



  void addMotor(Motors& m,int j,motor::ModeId prevMode){
    enabled.insert(j);
    info[j].state=MotorInfo::IDLE;

    NormPosition p= hw->convPos().normPos(m.hw->getPotentiometer(j,POTENTIOCHECKS)
					  ,j);
    m.high->rotate(j,p);	    
    m.hw->setPowerState(j,true);
    currPolicy->addMotor(j,p);     
  }
  
  void removeMotor(Motors& m,int j,motor::ModeId newMode){
    enabled.erase(j);
    currPolicy->removeMotor(j,p);
  }
  
  NormPosition getPosition(Motors& m,int j){
    switch(info[i].state){
    case IDLE:
      return m.hw->getPosition(j);      
    case MOVING:
      return motor::Interface::MOVINGPOS;
    case DISABLED:
      return motor::Interface::INVALIDPOS;
    }
  }

  void connect(Cell::CellConnection& ref,int j){
    info[j].signalledPos.connect(ref);
  }

  void disconnect(Cell::CellConnection& ref,int j){
    info[j].signalledPos.disconnect(ref);
  }
  

  void operator()(Motors& m){
  }
  
  
  

};
}//helium


#endif
