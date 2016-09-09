#ifndef HE_RAMMOTOR
#define HE_RAMMOTOR


#include <helium/robot/motorInterface.h>

namespace helium{

  template<typename Sign,typename FullSign=Sign>
class RamMotor:public motor::Raw<Sign>::Hardware{
  
    typedef motor::Raw<Sign> RawMotor;
    
    int n;
    typename RawMotor::PropNormalizer& pr;
    typename RawMotor::PosNormalizer& ps;
    PosLimiter<FullSign>& limiter;
  

    std::map<motor::PropertyRequest,typename RawMotor::Property> props;
  
  public:
  RamMotor(int pn,typename RawMotor::PropNormalizer& ppr,typename RawMotor::PosNormalizer& pps,PosLimiter<FullSign>& plimiter):
    n(pn),pr(ppr),ps(pps),limiter(plimiter)
  {
  }
  

  typename RawMotor::PropNormalizer& convProp(){
    return pr;
  }

  typename RawMotor::PosNormalizer& convPos(){
    return ps;
  }

  typename Sign::Pos getPotentiometer(int j,int checks){

   return  props[typename motor::PropertyRequest(j,motor::POTENTIOPOSITION)];
  }


  void rawGetProperty(typename Sign::Prop& o, const  motor::PropertyRequest & r){
    o=props[r];
  }

  void rawSetProperty(const typename RawMotor::Property& i, const motor::PropertyRequest & r){
    props[r]=i;
  }
  
  void rawGetGetterToken(typename RawMotor::Token& tk,const typename RawMotor::PropertyList &info){
    tk.reset(new DeletableData<typename RawMotor::PropertyList>(info)); 
  }

  void rawGetOptimizedSplit(Array <typename RawMotor::Token>& tk,const typename RawMotor::PropertyList &info){
    tk.resize(1);
    tk[0].reset(new DeletableData<typename RawMotor::PropertyList>(info)); 
  }

  void rawGetProperty(const typename RawMotor::Token& tk){
    const DeletableData<typename RawMotor::PropertyList>* d=trust_cast<const DeletableData<typename RawMotor::PropertyList>* >(tk.get());
    for (typename RawMotor::PropertyList::const_iterator it=d->data.begin();it!=d->data.end();++it){
      typename RawMotor::PropertyNotify p(props[*it->second],it->second,&pr);
      (*it->first)(&p);
    }
  }
  
  void setPowerState(int j,bool state=true){
    props[typename motor::PropertyRequest(j,motor::POWER)]=state;
  }
  
  bool getPowerState(int j){
    return props[typename motor::PropertyRequest(j,motor::POWER)];
  }
  void getPowerState(bool* b){
      fromAllMotors<bool,&RamMotor::getPowerState>(b);
  }
  
  void hwRotate(int j,typename Sign::Pos p){
    typename motor::PropertyRequest pr(j,motor::TARGETPOSITION);
    props[pr]=p;
    pr.id=motor::POTENTIOPOSITION;
    props[pr]=p;
    }
  
  void hwRotate(const typename Sign::Pos* p){
    onAllMotors<typename Sign::Pos,&RamMotor::hwRotate>(p);
    }

  void hwRotate(const bool* b,const typename Sign::Pos* p){
    onAllMotors<typename Sign::Pos,&RamMotor::hwRotate>(b,p);
    }
    
  void rawRotate(int j, typename Sign::Pos p){      
    hwRotate(j,limiter.limit(p,j));
  }

  void rawRotate(const typename Sign::Pos* p){
    onAllMotors< typename Sign::Pos,&RamMotor::rawRotate>(p);
    }

  void rawRotate(const bool* b,const typename Sign::Pos* p){
    onAllMotors< typename Sign::Pos,&RamMotor::rawRotate>(b,p);
    }
  

  void rotate(int j, motor::Interface::Position p){
      rawRotate(j,ps.unnormPos(p,j));
    }
    
    void rotate(const motor::Interface::Position* p){
      onAllMotors<const typename motor::Interface::Position,&RamMotor::rotate>(p);
    }


    void rotate(const bool* b,const motor::Interface::Position* p){
      onAllMotors<const typename motor::Interface::Position,&RamMotor::rotate>(b,p);
    }

  typename Sign::Pos rawGetPosition(int j){
    return  props[motor::PropertyRequest(j,motor::POTENTIOPOSITION)];
  }
    
  void rawGetPosition(typename Sign::Pos* p){
    fromAllMotors<typename Sign::Pos,&RamMotor::rawGetPosition>(p);
    }

  
    motor::Interface::Position getPosition(int j){
      return ps.normPos(rawGetPosition(j),j);
    }

    void getPosition(motor::Interface::Position* p){
      fromAllMotors<motor::Interface::Position,&RamMotor::getPosition>(p);
    }


    void connect(ConnReference<motor::Interface::Position>&, int){
      throw exc::InvalidOperation("RamMotor::Not implemented");
    }

    void disconnect(ConnReference<motor::Interface::Position>&,int j){
      throw exc::InvalidOperation("RamMotor::Not implemented");
    }
  void rawConnect(ConnReference<typename Sign::Pos>& conn,int j){
      throw exc::InvalidOperation("RamMotor::Not implemented");
    }
  void rawDisconnect(ConnReference<typename Sign::Pos>& conn,int j){
      throw exc::InvalidOperation("RamMotor::Not implemented");
    }
    

  motor::IfaceId getIfaceId(){
      return "ramMotor";
    }
  
    int getMotorNum(){
      return n;
    }
    
    void getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList &list) const{
      tk.resize(1);
      DeletableData<HwIdList>* d=new DeletableData<HwIdList>(list);
      tk[0].reset(d);
      
    }
  
    void isWorking(Array<HwState>& s,const HwIdList& id){
      s.resize(id.ids.size());
      setAll(s,WORKINGHWSTATE);
    };
    
    HwState isWorking(int id){
      return WORKINGHWSTATE;
    }
    
    void isWorking(Callback<const HwStateNotification&>* cb,const HwInterface::WorkingToken& tk){
      const DeletableData<HwIdList>* l=trust_cast<const DeletableData<HwIdList>*>(tk.get());
      HwStateNotification n;
      n.second=	WORKINGHWSTATE;
      for (size_t i=0;i<l->data.ids.size();i++){	
	n.first=l->data.ids[i];
	(*cb)(n);
      }
    }
    
    
    template<typename T,T (RamMotor::*m)(int)> void fromAllMotors(T* t){
      for (int j=0;j<n;j++){
	t[j]=(this->*m)(j);
      }
    }
    
    
    
    template<typename T,void (RamMotor::*m)(int,T)> void onAllMotors(const T* t){
    for (int j=0;j<n;j++){
      (this->*m)(j,t[j]);
    }
  }

    template<typename T,void (RamMotor::*m)(int,T)> void onAllMotors(const bool* b,const T* t){
    for (int j=0;j<n;j++){
      if (b[j]){
      (this->*m)(j,t[j]);
      }
    }
  }

  




  

};//ramMotor

}//ns helium


#endif
