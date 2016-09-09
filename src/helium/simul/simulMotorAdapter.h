#ifndef HE_SIMULMOTORADAPTER
#define HE_SIMULMOTORADAPTER

#include <helium/signal/ehSignal.h>
#include <helium/robot/motorInterface.h>
#include <helium/robot/motorStructures.h>
#include <helium/robot/deletableList.h>

namespace helium{

  struct SimulMotorSign{
    typedef double Pos;
    typedef double Prop;
    
    static const Pos INVALIDPOS;
    static const Prop INVALIDPROP;
    
    static const Pos MOVINGPOS;
    static const Pos FREEPOS;
  };
  //RobType is the practical implementation of the simulated robot, e.g. ODERobot


  template<typename RobType,typename Sign=SimulMotorSign>
  class SimulMotorAdapter;

  template<typename RobType,typename Sign>
  class SimulPosNormalizer:public LinearPosNormalizer<Sign> { //default implementation, specialize when necessary
  public:   
    typedef  motor::Calib<Sign>* ParamPtr;
    SimulPosNormalizer(SimulMotorAdapter<RobType,Sign>& owner,ParamPtr c):LinearPosNormalizer<Sign>(*c){
      if (c==NULL){	
	throw exc::Bug("NULL calib for SimulPosNormalzier");
      }
    }
  };

  
  template<typename RobType,typename Sign=SimulMotorSign>
  class SimulPropNormalizer:public motor::Raw<Sign>::PropNormalizer{	//default implementation, specialize when necessary
  public:    
    typedef void* ParamPtr;
    motor::Interface::Property normProp(typename Sign::Prop value,const motor::PropertyRequest& r);
    typename Sign::Prop unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r);
    SimulPropNormalizer(SimulMotorAdapter<RobType,Sign>& owner,ParamPtr);

 };

  

  template<typename RobType>
  class SimulPosNormalizer<RobType,SimulMotorSign>:public motor::Raw<SimulMotorSign>::PosNormalizer{
  public:
    typedef void* ParamPtr;
    motor::Interface::Position normPos(SimulMotorSign::Pos value,int joint);
    SimulMotorSign::Pos unnormPos(motor::Interface::Position value,int joint);
    SimulPosNormalizer(SimulMotorAdapter<RobType,SimulMotorSign>& owner,ParamPtr);
  };

 
    

 
  template<typename RobType,typename Sign>
  class SimulMotorAdapter:public motor::Raw<Sign >::Hardware{
    typedef motor::Interface::Position NormPosition;
    typedef typename Sign::Pos RawPosition;
    typedef double NormProperty;
    typedef typename Sign::Prop RawProperty;
    static const char* IFACEID;
    
    RobType* r; //for the potentiometer
    Array<double,array::RESIZABLE,array::LINK> m;  
    Array<Signal<NormPosition> > changed;
 public:
    int getMotorNum();
    
    void rotate(int j,NormPosition val);
    void rotate(const NormPosition* d);
    void rotate(const bool* b,const NormPosition* d);

    NormPosition getPosition(int joint);
    void getPosition(NormPosition* p);
    
    void connect(ConnReference<NormPosition>& conn,int j);
    void disconnect(ConnReference<NormPosition>& conn,int j);
    

    void rawRotate(int j,RawPosition val);
    void rawRotate(const RawPosition* d);
    void rawRotate(const bool* b,const RawPosition* d);    
    void hwRotate(int j,RawPosition val);    
    void hwRotate(const RawPosition* d);
    void hwRotate(const bool* b,const RawPosition* d);
    
    RawPosition rawGetPosition(int joint);
    void rawGetPosition(RawPosition* r);

    void rawConnect(ConnReference<RawPosition>& conn,int j);
    
    void rawDisconnect(ConnReference<RawPosition>& conn,int j);
    
    SimulPropNormalizer<RobType,Sign> propNorm;
    SimulPosNormalizer<RobType,Sign> posNorm;
    
    SimulPropNormalizer<RobType,Sign>& convProp();    
    SimulPosNormalizer<RobType,Sign>& convPos();
    
    RawPosition getPotentiometer(int joint,int reads);
    
    void rawGetProperty(RawProperty& o, const motor::PropertyRequest & r);
    
    void rawSetProperty(const RawProperty& i, const motor::PropertyRequest & r);
    
    typedef typename motor::Raw<Sign>::PropertyList PropertyList;
    typedef typename motor::Raw<Sign>::Token Token;
    typedef DeletablePayload<PropertyList > DeletablePropertyList;
   
    void rawGetGetterToken(Token& tk,const PropertyList &info);
    void rawGetOptimizedSplit(Array <Token>& tk,const PropertyList &info);

    void rawGetProperty(const Token& tk);
    void setPowerState(int j,bool state=true);

    bool getPowerState(int j);    
    void getPowerState(bool* b);
    
    HwState isWorking(int m);
    void isWorking(Array<HwState>& a,const HwIdList& l);
    
    
    void getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList & l) const;
    
    void isWorking(Callback<const HwStateNotification&>* cb,const HwInterface::WorkingToken&);
    
 
    motor::IfaceId getIfaceId();
    SimulMotorAdapter(RobType* pr,double* pm,int pn, 
		      typename SimulPropNormalizer<RobType,Sign>::ParamPtr prop=NULL,
		      typename SimulPosNormalizer<RobType,Sign>::ParamPtr pos=NULL
		      );


     /*void connect(ConnReference<Position>& conn,int j){std::cerr << "DEBUG: define SimulInter connect" << std::endl;}
    void disconnect(ConnReference<Position>& conn,int j){std::cerr << "DEBUG: define SimulInter disconnect" << std::endl;}
    void rawConnect(ConnReference<SimulMotorSign::Pos>& conn,int j){std::cerr << "DEBUG: define SimulInter rawconnect" << std::endl;}
    void rawDisconnect(ConnReference<SimulMotorSign::Pos>& conn,int j){std::cerr << "DEBUG: define SimulInter rawdisconnect" << std::endl;}
    */
    
  };//SimulMotorAdapter
  
  




}

#include <helium/simul/simulMotorAdapter.hpp>

#endif
