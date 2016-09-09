#include <helium/wrap/ode/odeStructure.h>
#include <helium/wrap/ode/simulGraphics.h>
#include <helium/graphics/glDraw.h>
#include <helium/vstone/V4Gstructure.h>
#include <helium/math/random.h>

#include <helium/system/cellServer.h>
#include <helium/system/keyboardCellServer.h>

#include <helium/thread/exitHandler.h>
#include <helium/graphics/pixelReader.h>
#include <helium/graphics/ppm.h>

#include <helium/robot/application.h>
#include <helium/robot/motorSpooler.h>
#include <helium/robot/motorOffMode.h>
#include <helium/robot/motorLinearMode.h>
#include <helium/robot/motorFramePlayMode.h>

#include <helium/robot/hwInterface.h>
#include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorStructures.h>

#include <helium/thread/resourceLease.h>
#include <helium/simul/mouseSimulatedTouchSensor.h>
#include <helium/robot/genericFilter.h>

using namespace helium;


struct SimulMotorSign{
  typedef double Pos;
  typedef double Prop;

  static const Pos INVALIDPOS;
  static const Prop INVALIDPROP;
  
  static const Pos MOVINGPOS;
  static const Pos FREEPOS;
};

const SimulMotorSign::Pos SimulMotorSign::INVALIDPOS=NAN;
const SimulMotorSign::Pos SimulMotorSign::INVALIDPROP=-100;

const SimulMotorSign::Pos SimulMotorSign::MOVINGPOS=-200;
const SimulMotorSign::Pos SimulMotorSign::FREEPOS=-300;

class SimulInter:public motor::Raw<SimulMotorSign>::Hardware{
  typedef Interface::Position NormPosition;
  typedef double RawPosition;
  typedef double NormProperty;
  typedef double RawProperty;
  static const char* IFACEID;

  ODERobot* r;
  Array<double,array::RESIZABLE,array::LINK> m;  
  Array<Signal<NormPosition> > changed;
  

  void rotate(int j,NormPosition val){
    m[j]=val;
    changed[j](val);
    
  }
  void rotate(NormPosition* d){
    assign(m,d);
    for (size_t i=0;i<m.size();i++){
      changed[i](d[i]);
    }
  }

  void rotate(bool* b,NormPosition* d){
    for (size_t i=0;i<m.size();i++){
      if (b[i]){
	rotate(i,d[i]);
      }
    }
  }


  
  NormPosition getPosition(int joint){
    return m[joint];
  }
    
  void getPosition(NormPosition* p){
    assign(p,m);
  }
  
  void connect(ConnReference<NormPosition>& conn,int j){
    changed[j].connect(conn);
  }

  void disconnect(ConnReference<Position>& conn,int j){
    changed[j].disconnect(conn);
  };

 


  void rawRotate(int j,RawPosition val){
    rotate(j,val);
  }

  void rawRotate(RawPosition* d){
    rotate(d);
  }

  void rawRotate(bool* b,RawPosition* d){
    rotate(b,d);
  }

  void hwRotate(int j,RawPosition val){
    rotate(j,val);
  }

  void hwRotate(RawPosition* d){
    rotate(d);
  }

  void hwRotate(bool* b,RawPosition* d){
    rotate(b,d);
  }

  
  RawPosition rawGetPosition(int joint){
    return m[joint];
  }

  void rawGetPosition(RawPosition* r){
    assign(r,m);
  }

  void rawConnect(ConnReference<RawPosition>& conn,int j){
    changed[j].connect(conn);
  }
  void rawDisconnect(ConnReference<RawPosition>& conn,int j){
    changed[j].disconnect(conn);
  }

  class PropNormalizer:public motor::Raw<SimulMotorSign>::PropNormalizer{	
  public:

    NormProperty normProp(RawProperty value,const motor::PropertyRequest& r){
      throw exc::InvalidOperation("Property "+toString(r)+" not available for "+toString(IFACEID));
    }
    RawProperty unnormProp(NormProperty value,const motor::PropertyRequest& r){
      throw exc::InvalidOperation("Property "+toString(r)+" not available for "+toString(IFACEID));
    }
  };

  class PosNormalizer:public motor::Raw<SimulMotorSign>::PosNormalizer{
  public:
    NormPosition normPos(RawPosition value,int joint){
      return value;
    }
    RawPosition unnormPos(Interface::Position value,int joint){
      return value;
    }
  };
  static PropNormalizer propNormalizer;
  static PosNormalizer posNormalizer;

  PropNormalizer& convProp(){
    return propNormalizer;
  }


  PosNormalizer& convPos(){
    return posNormalizer;
  }
    
  RawPosition getPotentiometer(int joint,int reads){
    return r->getPosition(joint);      
  }

  void rawGetProperty(RawProperty& o, const motor::PropertyRequest & r){
    throw exc::InvalidOperation("Property "+toString(r)+" not available for "+toString(getIfaceId()));
  }
    
  void rawSetProperty(const RawProperty& i, const motor::PropertyRequest & r){
    throw exc::InvalidOperation("Property "+toString(r)+" not available for "+toString(getIfaceId()));
  }

  typedef  motor::Raw<SimulMotorSign>::PropertyList PropertyList;
  typedef  motor::Raw<SimulMotorSign>::Token Token;
  class Tok:public Deletable{
    friend class SimulInter;
    PropertyList l;
    Tok(const PropertyList &pl):l(pl){
    }
  };

    
  void rawGetGetterToken(Token& tk,const PropertyList &info){
    tk=new Tok(info);
  }
  void rawGetOptimizedSplit(Array <Token>& tk,const PropertyList &info){
    tk.resize(1);
    tk[0]=new Tok(info);
  }

  void rawGetProperty(const Token& tk){
    const Deletable* d=&tk;
    const PropertyList& l=trust_cast<const Tok*>(d)->l;
    motor::Raw<SimulMotorSign>::PropertyNotify p;
    p.third=&convProp();
    for (PropertyList::const_iterator it=l.begin();it!=l.end();it++){
      rawGetProperty(p.first,*it->second);
      p.second=it->second;
	
      (*it->first)(&p);
    }
  }


  void setPowerState(int j,bool state=true){
    r->setPowerState(j,state);
  }

  bool getPowerState(int j){
    return r->getPowerState(j);
  }

  void getPowerState(bool* b){
    for (size_t j=0;j<m.size();j++){
      b[j]=getPowerState(j);
    }
  }
  
  HwState isWorking(int m){
    return WORKINGHWSTATE;
  }
  
  void isWorking(Array<HwState>& a,const HwIdList& l){
    a.reset(l.ids.size());
    setAll(a,WORKINGHWSTATE);
  }


  void getWorkingOptimizedSplit(Array <WorkingToken>& tk,const HwIdList & l) const{
    tk.resize(1);
    tk[0]=new Deletable();
  }

  void isWorking(Callback<const HwStateNotification&>* cb,const WorkingToken&){
    for (size_t i=0;i<m.size();i++){
      (*cb)(std::make_pair(i,WORKINGHWSTATE));
    }
  }
  
public:
   motor::IfaceId getIfaceId(){
    return IFACEID;
  }
  SimulInter(ODERobot* pr,double* pm,int pn):r(pr),m(pm,pn),changed(pn){    
  }

};//SimulInter



static const int SENSOREVERYN=10;
class SimulTouch:public helium::sensor::Set<double>,public ODEWorldCallback{
  double* forces;
  int n;
  ResourceLease lease;


  void odeWorldCallback(){
    lease.producerLease();
  }

  class BasicToken:public Deletable{
  public:
    std::vector<int> toRead;
    BasicToken(const std::vector<int>& ptoRead):toRead(ptoRead){
    }
  };
  

  double rawRead(int idx){
    lease.consumerGet();
    double d=forces[idx];
    lease.consumerRemit();
    return d;    
  }

  void rawRead(double* t){
    lease.consumerGet();
    assign(t,forces,n);
    lease.consumerRemit();
  }

  void rawRead(double* r, const HwIdList& idx){
    lease.consumerGet();
    for (size_t i=0;i<idx.ids.size();i++){
      r[idx.ids[i]]=forces[idx.ids[i]];
    }
    lease.consumerRemit();
  }

  void rawRead(SensorValList<double>& r, const HwIdList& idx){
    lease.consumerGet();
    for (size_t i=0;i<idx.ids.size();i++){
      r.vals[i]=forces[idx.ids[i]];
    }
    lease.consumerRemit();
  }




  void getToken(Token& t, const HwIdList& toRead){
    t=new BasicToken(toRead.ids);
  }

  void rawRead(double* r,const Token& tok){
    lease.consumerGet();
    const BasicToken& t=static_cast<const BasicToken&>(*tok);
    int c=0;
    for (std::vector<int>::const_iterator it=t.toRead.begin();
	 it!=t.toRead.end();++it){
      r[*it]=forces[*it];
      c++;
    }       
    lease.consumerRemit();
  }

 
  HwState isWorking(int id){
    return WORKINGHWSTATE;
  }
  
  void isWorking(Array<HwState>& a,const HwIdList& l){
    a.resize(l.ids.size());    
    setAll(a,WORKINGHWSTATE);
  }
  
  void getWorkingOptimizedSplit(Array <WorkingToken>& tk,const HwIdList & l) const{
    tk.resize(1);
    tk[0]=new Deletable();
  }
  
  void isWorking(Callback<const HwStateNotification&>* cb,const WorkingToken&){
    for (int i=0;i<n;i++){
      (*cb)(std::make_pair(i,WORKINGHWSTATE));
    }
  };


public:
   size_t size(){
    return n;
  }
  
  SimulTouch(double* pforces,int pn):ODEWorldCallback(SENSOREVERYN),forces(pforces),n(pn){    
  }

};


class CombineFilterComp:public sensor::GenericFilterComputation<double,double>{
  MouseSimulatedTouchSensor& msts;
  
  double saturate(double x){
    return 1-exp(-2*x);
  }

  void compute(double* r,const double *s,const HwIdList& what){    
    for (size_t i=0;i<what.ids.size();i++){
      int idx=what.ids[i];
      r[idx]=saturate(s[idx]);
    }
    msts.addTo(r,what.ids);
  }

  void  getRequired(HwIdList& r,const HwIdList& s){
    r=s;
  }


public:
  CombineFilterComp(MouseSimulatedTouchSensor& pmsts):msts(pmsts){
  }


};


class SimulRawTsComp:public sensor::GenericFilterComputation<int,double>{

  int denorm(double x){
    return 300+x*1400;
  }
  
  void compute(int* r,const double *s,const HwIdList& what){    
    for (size_t i=0;i<what.ids.size();i++){
      int idx=what.ids[i];
      r[idx]=denorm(s[idx]);
    }
  }

   void  getRequired(HwIdList& r,const HwIdList& s){
    r=s;
  }
  
};


template<typename T>
class ThresholdCB:public Callback<T*>{
  int n;
  
  void operator()(T* t){        
    //std::cout<<"update"<<std::endl;
    std::stringstream ss;
    for (int i=0;i<n;i++){
      if (t[i]>0.2){
	ss<<" "<<i<<"="<<t[i];
      }      
    }
    if (ss.str().length()>0){
      std::cout<<"update "<<ss.str()<<std::endl;
    }
  }
public:

  ThresholdCB(int pn):n(pn){
  }

};




const char* SimulInter::IFACEID("ODE");
SimulInter::PropNormalizer SimulInter::propNormalizer;
SimulInter::PosNormalizer SimulInter::posNormalizer;



class PSimul:public Application{

  void run(){    
    static const int n=22;   
    ODEWorld world;    

    double m[n];
    zeron(m,n);
    Array<double,V4Gstructure::BOXESNUM*6> displaySensor;    
    V4Gstructure v4s(m,displaySensor.data);  

   
    ODERobot* r=world.addRobot(&v4s);    
    SimulWin window(&world);  
    MouseSimulatedTouchSensor mouseSensor(displaySensor.data,displaySensor.size(),&v4s);
    window.addKeyboardInteraction(&mouseSensor);
    window.addMouseSensorListener(&mouseSensor);

    GlDraw::push_back(&window);
    SimulInter motorIface(r,m,n);    
    world.run(3.5E-3);    
    //PixelReader pr(window,false);

    


    MotorSpooler<SimulMotorSign> ms(n,false);
    ms.interfaces.do_registerAll(&motorIface);
    ms.start();
    OffMode<SimulMotorSign> off;
    //InterpMode<double,double,motor::IFACELEVEL> ninterp(ms,n);
    //InterpMode<double,double,motor::RAWLEVEL> rinterp(ms,n);    
    //mplan::MotorPlannerMode<SimulMotorSign,motor::IFACELEVEL> planmm(ms,n);
    SecondRefTimeSource tsrc(world.t);
    //mplan::InterpPlanner<double> iplan(tsrc,n);

    MotorLinearMode<SimulMotorSign,motor::IFACELEVEL> mlm(ms,n,tsrc);
    MotorFramePlayMode<SimulMotorSign> mfp(ms,n,tsrc);

    //planmm.setPlanner(&iplan);

    ms.registerMode(&off);
    //    ms.registerMode(&planmm);
    //ms.registerMode(&ninterp);
    //ms.registerMode(&rinterp);
    ms.registerMode(&mlm);
    ms.registerMode(&mfp);
  
    ms.setInterface(motor::IfaceSelection(motor::IFACELEVEL,motorIface.getIfaceId()));
    ms.setInterface(motor::IfaceSelection(motor::RAWLEVEL,motorIface.getIfaceId()));
    ms.setInterface(motor::IfaceSelection(motor::HWLEVEL,motorIface.getIfaceId()));
    for (int i=0;i<n;i++){
      //ms.setMotorMode(ninterp.getMotorModeId(),i);
      //ms.setMotorMode(planmm.getMotorModeId(),i);
      ms.setMotorMode(mlm.getMotorModeId(),i);
    }
    GlDraw::start(argc,argv);
    dexport(mem,ms,"robot");
    //dexport(ninterp,mem,"robot");
    //dexport(rinterp,mem,"robot");
    //dexport(planmm,mem,"robot");
    dexport(mem,mlm,"robot");
    dexport(mem,mfp,"robot");

    SimulTouch touch(v4s.forces,V4Gstructure::BOXESNUM*6);
    world.addCallback(&touch);
    SensorSpooler<double> touchSs(touch);
    SensorProcessor<double> touchSp(touchSs,touch.size());
    dexport(mem,touchSp,IDPath("robot.touch.forces","."));
    CombineFilterComp combFilterComp(mouseSensor);
    
    sensor::GenericFilter<double,double> combFilter(combFilterComp,touch.size(),touchSp,touch.size());
    dexport(mem,combFilter,IDPath("robot.touch.norm","."));

    ThresholdCB<double> tcb(touch.size());
    ConnReference<double*> tcbRef(&tcb);
    combFilter.connect(tcbRef,sensor::READ);

    SimulRawTsComp simulRawTsComp;
    sensor::GenericFilter<int,double> simulRawTsFilter(simulRawTsComp,touch.size(),combFilter,touch.size());
    dexport(mem,simulRawTsFilter,IDPath("robot.touch.raw","."));

    
    CellServer tcp(CELLPORT,mem);
    
    KeyboardCellServer keyb(mem);

    keyb.start();
    Semaphore termination;    
    DestroyableSignal<void>::SignalReference keybSigRef(&termination);
    DestroyableSignal<void>::SignalReference exitSigRef(&termination);

    keyb.onStreamTermination.connect(keybSigRef);        
    ExitHandler::get().exitRequestConnect(exitSigRef);



    GlDraw::Stopper graphics;
    termination.wait();
    graphics.stop();
    //keyb.stopReading();
    
  }

};





int main(int argc,char** argv){
  {
    PSimul psimul;
    psimul.launch(argc,argv);
    std::cout<<"Simul execute terminated"<<std::endl;
  }
  std::cout<<"Simul destroyed"<<std::endl;
  OnExitWait thisThread;
}


