#include <helium/simul/simulRototraAdapter.h>
#include <helium/robot/deletableList.h>
namespace helium{

void SimulRototraAdapter::odeWorldCallback(){
  lease.producerLease();
}




double  SimulRototraAdapter::rawRead(int idx){
  double v;
  lease.consumerGet();
  r->getPivotRototranslation(m);
  v=m[idx];
  lease.consumerRemit();
  return v;    
}

void SimulRototraAdapter::rawRead(double* t){
  lease.consumerGet();
  r->getPivotRototranslation(t);
  lease.consumerRemit();  
}

void SimulRototraAdapter::rawRead(double* v, const HwIdList& idx){
  lease.consumerGet();
  r->getPivotRototranslation(m);  
  for (size_t i=0;i<idx.ids.size();++i){
    v[idx.ids[i]]=m[idx.ids[i]];
  }
  lease.consumerRemit();
}


  void SimulRototraAdapter::rawRead(SensorValList<double>& v, const HwIdList& idx){
    lease.consumerGet();
    r->getPivotRototranslation(m);  
    for (size_t i=0;i<idx.ids.size();i++){
      v.vals[i]=m[idx.ids[i]];
    }
    lease.consumerRemit();
 }


void SimulRototraAdapter::getToken(Token& t, const HwIdList& toRead){
  t=new DeletableList(toRead);
}


void SimulRototraAdapter::rawRead(double* ret,const Token& tok){
  const DeletableList& t=static_cast<const DeletableList&>(*tok);
  lease.consumerGet();
  r->getPivotRototranslation(m);  
  for (std::vector<int>::const_iterator it=t.ids.begin();
       it!=t.ids.end();++it){
    ret[*it]=m[*it];
  }   
  lease.consumerRemit();
}




HwState SimulRototraAdapter::isWorking(int id){
  return WORKINGHWSTATE;
}


  void SimulRototraAdapter::isWorking(Array<HwState>& a,const HwIdList& l){
    a.resize(l.ids.size());    
    setAll(a,WORKINGHWSTATE);
  }
  
void SimulRototraAdapter::getWorkingOptimizedSplit(Array <WorkingToken>& tk,const HwIdList & l) const{
    tk.resize(1);
    tk[0]=new DeletableList(l);
}

  
  void SimulRototraAdapter::isWorking(Callback<const HwStateNotification&>* cb,const WorkingToken& tok){
    const DeletableList& t=static_cast<const DeletableList&>(*tok);
    for (size_t i=0;i<t.ids.size();i++){
      (*cb)(std::make_pair(t.ids[i],WORKINGHWSTATE));
    }
  };

 size_t SimulRototraAdapter::size() const{
   return 16;
 }

  SimulRototraAdapter::SimulRototraAdapter(ODERobot* pr,int everyN):ODEWorldCallback(everyN),r(pr){    
  }

}//ns helium
