/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * Release_flags s
 * \version 0.1
 */

#ifndef HE_MOTORLINEARMODE
#define HE_MOTORLINEARMODE

#include <helium/robot/motorActiveMode.h>
#include <helium/robot/timeSource.h>

#include <helium/math/tAbs.h>

namespace helium{

  namespace mlin{
    enum TargetType {PVTARGET,EPVTARGET,HALTTARGET};
    
    template<typename P>
    struct BasicTarget{
      //typedef typename mpAdapter<MotorSign,L>::Position P;
      P position;
      Time reachTime;
      int j;
      BasicTarget();
      BasicTarget(P pposition,Time preachTime, int pj);
    };

    template<typename P>
    struct BasicJointTarget{
      //typedef typename mpAdapter<MotorSign,L>::Position P;
      P position;
      Time reachTime; //this is absolute time

      BasicJointTarget(){}
      BasicJointTarget(P pposition,Time preachTime);
      BasicJointTarget(const BasicTarget<P>& t);
    };
    

    template<typename MotorSign,motor::MotorLevel L>
    class Target{
      typedef typename mpAdapter<MotorSign,L>::Position P;
   public:
      
      virtual void compute(std::list<BasicTarget<P> >& target,P* currPos) const=0; 
      virtual void serialize(std::ostream& o) const=0;
      virtual void deserialize(std::istream& i)=0;
      virtual TargetType getType() const=0;
      virtual ~Target(){}
    };
    
 

    template<typename MotorSign,motor::MotorLevel L>
    class JointTarget{
      typedef typename mpAdapter<MotorSign,L>::Position P;
    public:
      virtual void compute(std::list<BasicJointTarget<P> >& target,P currPos) const=0; 
      virtual void serialize(std::ostream& o) const=0;
      virtual void deserialize(std::istream& i)=0;
      virtual TargetType getType() const=0;
      virtual ~JointTarget(){}
    };

  }//ns mlin


  template<>
  struct EnumStrings<mlin::TargetType>{
     static const int NUM=3;
     static const char* strings[NUM];     
  };


  template<typename MotorSign,motor::MotorLevel L>
  struct TypedCellUnistantiable<mlin::Target<MotorSign,L> >{
      static const bool value=true;
  };
   
   
  template<typename MotorSign,motor::MotorLevel L>
  struct TypedCellUnistantiable<mlin::JointTarget<MotorSign,L> >{
      static const bool value=true;
  };


  namespace mlin{

    //these methods call the getType and serialize method of the target
    template<typename MotorSign,motor::MotorLevel L>
    inline std::ostream& operator<<(std::ostream& o,const Target<MotorSign,L>& t);

    template<typename MotorSign,motor::MotorLevel L>
    inline std::ostream& operator<<(std::ostream& o,const JointTarget<MotorSign,L>& t);
    
   
    template<typename MotorSign,motor::MotorLevel L>
      class JointPVTarget:public JointTarget<MotorSign,L>{
       typedef typename mpAdapter<MotorSign,L>::Position P;
    public:
      P pos;
      double v;
      JointPVTarget();
      JointPVTarget(P ppos,double pv);
      void serialize(std::ostream& o) const;
      void deserialize(std::istream& i);      
      void compute(std::list<BasicJointTarget<P> >& target,P currPos) const;
      TargetType getType() const;
    };

    //exact position target
    template<typename MotorSign,motor::MotorLevel L>
    class JointEPVTarget:public JointPVTarget<MotorSign,L>{
      void serialize(std::ostream& o) const;
      void deserialize(std::istream& i);    
      TargetType getType() const;
    };


    template<typename MotorSign,motor::MotorLevel L>    
    class JointHaltTarget:public JointTarget<MotorSign,L>{
      typedef typename mpAdapter<MotorSign,L>::Position P;
      void serialize(std::ostream& o) const{}
      void deserialize(std::istream& i){}    
      TargetType getType() const;
      void compute(std::list<BasicJointTarget<P> >& target,P currPos) const;
    };



    template<typename MotorSign,motor::MotorLevel L>
      class PVTarget:public Target<MotorSign,L>{
      typedef typename mpAdapter<MotorSign,L>::Position P; 
    public:
      Array<std::pair<P,double> > pv;
      void serialize(std::ostream& o) const;      
      void deserialize(std::istream& i);      
      void compute(std::list<BasicTarget<P> >& target,P* currPos) const;
      TargetType getType() const;
    public:
      PVTarget(int n);
    };//PVTarget

    template<typename MotorSign,motor::MotorLevel L>
    class EPVTarget:public PVTarget<MotorSign,L>{
      void serialize(std::ostream& o) const;
      void deserialize(std::istream& i);    
      TargetType getType() const;
    public:
      EPVTarget(int n);
    };

    
    template<typename MotorSign,motor::MotorLevel L>
    class HaltTarget:public PVTarget<MotorSign,L>{
typedef typename mpAdapter<MotorSign,L>::Position P;
      int n;
      void serialize(std::ostream& o) const;
      void deserialize(std::istream& i);    
      TargetType getType() const;
      void compute(std::list<BasicTarget<P> >& target,P* currPos) const;
    public:
      HaltTarget(int n);
    };



  }//ns mlin

  
  template <typename MotorSign,motor::MotorLevel L>
  class MotorLinearMode;
  
  template<typename MotorSign,motor::MotorLevel L>
  struct LinearModeSignature{
    
    typedef mlin::Target<MotorSign,L> Target;
    typedef mlin::JointTarget<MotorSign,L> JointTarget;
    
    typedef TargetCellBase<MotorSign,MotorLinearMode<MotorSign,L>,Target> TargetCell;
    typedef JointTargetCellBase<MotorSign,MotorLinearMode<MotorSign,L>,JointTarget> JointTargetCell;
  };


  
  template <typename MotorSign,motor::MotorLevel L>
  class MotorLinearMode:public MotorActiveMode<MotorSign,L,MotorLinearMode<MotorSign,L>, 
					       LinearModeSignature<MotorSign,L>
					       >{
    
    TimeSource& time;
  public:
    typedef LinearModeSignature<MotorSign,L> ModeSign;
    typedef MotorActiveMode<MotorSign,L,MotorLinearMode,
			    ModeSign
			    >  Base;
    typedef typename Base::P P;
    typedef typename Base::adapter adapter;
    typedef typename Base::Motors Motors;
    typedef typename Base::Spool Spool;
    
    typedef typename ModeSign::Target Target;
    typedef typename ModeSign::JointTarget JointTarget;


    typedef mlin::BasicTarget<P> BasicTarget;
    typedef mlin::BasicJointTarget<P> BasicJointTarget;
  private:
   

    Array<std::list<BasicJointTarget > > targets;
    Array<P> startPos;
    Array<Time> zeroTime;

    void push(int j,P pos,Time t);
    void clearTargets();
    void clearTarget(int j);        
    void pushTargets(std::list<BasicTarget >& target);
    void pushJointTargets(int j,std::list<BasicJointTarget >& target);

  public:

    static const int wholeTargetSetPriority=MotSpoolPriority::NORMAL;
    static const int jointTargetSetPriority=MotSpoolPriority::NORMAL;
    static const int spoolingPriority=MotSpoolPriority::NORMAL;


    //public because the base needs to access it
    void getPosture(bool* moving,P* newPos,Motors& m);
    void setWholeTarget(Motors& m,const Target& st);    
    void setJointTarget(Motors& m,const JointTarget& st,int j);
    Target* getWholeTarget(std::istream& i);
    JointTarget* getJointTarget(std::istream& i,int j);

    static motor::ModeId modeID();    
    MotorLinearMode(Spool& pspool,TimeSource& timeSource);
    
  };



  template <typename MotorSign,motor::MotorLevel L>
  struct DefDescr<MotorLinearMode<MotorSign,L> >:public Description<MotorLinearMode<MotorSign,L> > { 
    DefDescr(MotorLinearMode<MotorSign,L>& mlm):Description<MotorLinearMode<MotorSign,L> >(mlm){      
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
    
  };


}//helium

#include <helium/robot/motorLinearMode.hpp>

#endif
