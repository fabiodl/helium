#include <helium/simul/simulTouchAdapter.h>
#include <helium/robot/deletableList.h>

namespace helium{

  
  void SimulTouchAdapter::odeWorldCallback(){
    lease.producerLease();
  }


  double SimulTouchAdapter::rawRead(int idx){
      lease.consumerGet();
      double d=forces[idx];
      lease.consumerRemit();
      return d;    
    }
  
  void SimulTouchAdapter::rawRead(double* t){
    lease.consumerGet();
    assign(t,forces,n);
    lease.consumerRemit();
  }
 
  void SimulTouchAdapter::rawRead(double* r, const HwIdList& idx){
    lease.consumerGet();
    for (size_t i=0;i<idx.ids.size();i++){
      r[idx.ids[i]]=forces[idx.ids[i]];
    }
    lease.consumerRemit();
  }
  
  void SimulTouchAdapter::rawRead(SensorValList<double>& r, const HwIdList& idx){
    lease.consumerGet();
    for (size_t i=0;i<idx.ids.size();i++){
      r.vals[i]=forces[idx.ids[i]];
    }
    lease.consumerRemit();
  }




  void SimulTouchAdapter::getToken(Token& t, const HwIdList& toRead){
    t=new DeletableList(toRead);
   }

  void SimulTouchAdapter::rawRead(double* r,const Token& tok){
    lease.consumerGet();
    const DeletableList& t=static_cast<const DeletableList&>(*tok);
    for (std::vector<int>::const_iterator it=t.ids.begin();
	 it!=t.ids.end();++it){
      r[*it]=forces[*it];
    }       
    lease.consumerRemit();
  }

  HwState SimulTouchAdapter::isWorking(int id){
    return WORKINGHWSTATE;
  }
  
  void SimulTouchAdapter::isWorking(Array<HwState>& a,const HwIdList& l){
    a.resize(l.ids.size());    
    setAll(a,WORKINGHWSTATE);
  }
  
  void SimulTouchAdapter::getWorkingOptimizedSplit(Array <WorkingToken>& tk,const HwIdList & l) const{
    tk.resize(1);
    tk[0]=new Deletable();
  }

  void SimulTouchAdapter::isWorking(Callback<const HwStateNotification&>* cb,const WorkingToken&){
    for (int i=0;i<n;i++){
      (*cb)(std::make_pair(i,WORKINGHWSTATE));
    }
  };


  size_t SimulTouchAdapter::size() const{
    return n;
  }

  SimulTouchAdapter::SimulTouchAdapter(double* pforces,int pn,int everyN):ODEWorldCallback(everyN),forces(pforces),n(pn){    
  }


}
