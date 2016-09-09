#include <helium/wrap/ode/odeStructure.h>
#include <helium/thread/resourceLease.h>
#include <helium/robot/sensorInterface.h>


namespace helium{
  class SimulRototraAdapter:public helium::sensor::Set<double>,public ODEWorldCallback{
    ODERobot* r;
    double m[16];
    ResourceLease lease;   
  public:    
    void odeWorldCallback();   
    double rawRead(int idx);
    void rawRead(double* t);
    void rawRead(double* r, const HwIdList& idx);
    void rawRead(SensorValList<double>& r, const HwIdList& idx);
    void getToken(Token& t, const HwIdList& toRead);
    void rawRead(double* r,const Token& tok);
    HwState isWorking(int id);
    void isWorking(Array<HwState>& a,const HwIdList& l);
    void getWorkingOptimizedSplit(Array <WorkingToken>& tk,const HwIdList & l) const;
    void isWorking(Callback<const HwStateNotification&>* cb,const WorkingToken&);
    size_t size() const;

    SimulRototraAdapter(ODERobot* pforces,int everyN);


  };


}
