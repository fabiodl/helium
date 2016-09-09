#include <helium/vector/operations.h>
#include <helium/robot/motorException.h>
namespace helium{





  template<typename RobType>
  motor::Interface::Position SimulPosNormalizer<RobType,SimulMotorSign>::normPos(SimulMotorSign::Pos value,int joint){
    return value;
  }
  

  template<typename RobType>
  SimulMotorSign::Pos SimulPosNormalizer<RobType,SimulMotorSign>::unnormPos(motor::Interface::Position value,int joint){
    return value;
  }

  template<typename RobType>    
  SimulPosNormalizer<RobType,SimulMotorSign>::SimulPosNormalizer(SimulMotorAdapter<RobType,SimulMotorSign>&,void*){    
  }
  
  template<typename RobType,typename Sign>
  motor::Interface::Property SimulPropNormalizer<RobType,Sign>::normProp(typename Sign::Prop value,const motor::PropertyRequest& r){
    return value;
  }
  
  
  template<typename RobType,typename Sign>
  typename Sign::Prop SimulPropNormalizer<RobType,Sign>::unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r){
    return value;
  }

  template<typename RobType,typename Sign>    
  SimulPropNormalizer<RobType,Sign>::SimulPropNormalizer(SimulMotorAdapter<RobType,Sign>&,void*){    
  }
  
  template<typename RobType,typename Sign>
  int SimulMotorAdapter<RobType,Sign>::getMotorNum(){
    return m.size();
  }



 template<typename RobType,typename Sign>
 void SimulMotorAdapter<RobType,Sign>::rotate(int j,motor::Interface::Position val){
  
   /*std::cout<<"rotate "<<j<<" = "<<val<<std::endl;
   if (j==2){
   printTrace();
   }*/
   m[j]=val;
   changed[j](val);      
 }

  template<typename RobType,typename Sign>
 void SimulMotorAdapter<RobType,Sign>::rotate(const motor::Interface::Position* d){
      assign(m,d);
      for (size_t i=0;i<m.size();i++){
	changed[i](d[i]);
      }
    }


  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::rotate(const bool* b,const motor::Interface::Position* d){
      for (size_t i=0;i<m.size();i++){
	if (b[i]){
	  rotate(i,d[i]);
	}
      }
    }
  
  template<typename RobType,typename Sign>
  motor::Interface::Position SimulMotorAdapter<RobType,Sign>::getPosition(int joint){
    //std::cout<<"getting position["<<joint<<"]"<<m[joint]<<std::endl;
    return m[joint];
  }
  
  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::getPosition(motor::Interface::Position* p){
    assign(p,m);
  }
  
  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::connect(ConnReference<motor::Interface::Position>& conn,int j){
      changed[j].connect(conn);
    }

  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::disconnect(ConnReference<motor::Interface::Position>& conn,int j){
      changed[j].disconnect(conn);
    };

  
  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::rawRotate(int j,typename Sign::Pos val){
    rotate(j,posNorm.normPos(val,j));
  }
  
 
  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::rawRotate(const typename Sign::Pos* d){
    for (size_t j=0;j<m.size();j++){
      rotate(j,posNorm.normPos(d[j],j));
    }
  }

  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::rawRotate(const bool* b,const typename Sign::Pos* d){
    for (size_t j=0;j<m.size();j++){
      if (b[j]){
	rotate(j,posNorm.normPos(d[j],j));
      }
    }
  }
  
  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::hwRotate(int j,typename Sign::Pos val){
    rawRotate(j,val);
  }

  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::hwRotate(const typename Sign::Pos* d){
    rawRotate(d);
  }
  
  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::hwRotate(const bool* b,const typename Sign::Pos* d){
    rawRotate(b,d);
  }


  template<typename RobType,typename Sign>
  typename Sign::Pos SimulMotorAdapter<RobType,Sign>::rawGetPosition(int joint){
    return posNorm.unnormPos(m[joint],joint);
  }

  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::rawGetPosition(typename Sign::Pos* r){
    for (size_t j=0;j<m.size();j++){
      r[j]=posNorm.unnormPos(m[j],j);
    }
  }
  
  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::rawConnect(ConnReference<typename Sign::Pos>& conn,int j){
    throw exc::InvalidOperation("raw connect");
    //changed[j].connect(conn);
  }
  
  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::rawDisconnect(ConnReference<typename Sign::Pos>& conn,int j){
    throw exc::InvalidOperation("raw disconnect");
    //changed[j].disconnect(conn);
  }

  template<typename RobType,typename Sign>  
  SimulPropNormalizer<RobType,Sign>& SimulMotorAdapter<RobType,Sign>::convProp(){
    return propNorm;
  }
   
  template<typename RobType,typename Sign>  
  SimulPosNormalizer<RobType,Sign>& SimulMotorAdapter<RobType,Sign>::convPos(){
    return posNorm;
  }
 

  template<typename RobType,typename Sign>
  typename Sign::Pos SimulMotorAdapter<RobType,Sign>::getPotentiometer(int joint,int reads){
    //std::cout<<"asked the potentiometer, real"<<r->getPosition(joint)<<std::endl;
    return posNorm.unnormPos(r->getPosition(joint),joint);      
  }
  


  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::rawGetProperty(typename Sign::Prop& o, const motor::PropertyRequest & r){
    throw exc::InvalidOperation("Property "+toString(r)+" not available for "+toString(getIfaceId()));
  }

  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::rawSetProperty(const typename Sign::Prop& i, const motor::PropertyRequest & r){
      throw exc::InvalidOperation("Property "+toString(r)+" not available for "+toString(getIfaceId()));
    }

  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::rawGetGetterToken(Token& tk,const PropertyList &info){
    tk=new typename SimulMotorAdapter<RobType,Sign>::DeletablePropertyList(info);
  }

  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::rawGetOptimizedSplit(Array <Token>& tk,const PropertyList &info){
      tk.resize(1);
      tk[0]=new DeletablePropertyList(info);
      /*std::cout<<"CREATING NEW SPLIT for";
      for (typename PropertyList::const_iterator it=info.begin();it!=info.end();++it){
	std::cout<<" "<<std::endl;
	print(*it->second);
      }
      std::cout<<std::endl;
      */
    }
  
  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::rawGetProperty(const Token& tk){
      const Deletable* d=&tk;
      const PropertyList& l=*trust_cast<const DeletablePropertyList*>(d);
      typename motor::Raw<Sign>::PropertyNotify p;
      p.third=&convProp();
      for (typename PropertyList::const_iterator it=l.begin();it!=l.end();it++){
	rawGetProperty(p.first,*it->second);
	p.second=it->second;
	//std::cout<<"signalling"<<*p.second<<std::endl;
	(*it->first)(&p);
      }
    }

  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::setPowerState(int j,bool state){
    
    /*if (j==4){
      throw exc::SingleMotorException<FAULTYVALUEHWSTATE>(4,"test");
      }*/
    r->setPowerState(j,state);
  }

  template<typename RobType,typename Sign>
  bool SimulMotorAdapter<RobType,Sign>::getPowerState(int j){
    return r->getPowerState(j);
  }
  
  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::getPowerState(bool* b){
    for (size_t j=0;j<m.size();j++){
      b[j]=getPowerState(j);
    }
  }
  
  
  template<typename RobType,typename Sign>
  HwState SimulMotorAdapter<RobType,Sign>::isWorking(int m){
    return WORKINGHWSTATE;
  }
  
  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::isWorking(Array<HwState>& a,const HwIdList& l){
      a.reset(l.ids.size());
      setAll(a,WORKINGHWSTATE);
    }
  
   template<typename RobType,typename Sign>
   void SimulMotorAdapter<RobType,Sign>::getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList & l) const{
     tk.resize(1);
      tk[0]=new Deletable();
   }
  
  template<typename RobType,typename Sign>
  void SimulMotorAdapter<RobType,Sign>::isWorking(Callback<const HwStateNotification&>* cb,const HwInterface::WorkingToken&){
      for (size_t i=0;i<m.size();i++){
	(*cb)(std::make_pair(i,WORKINGHWSTATE));
      }
    }

  template<typename RobType,typename Sign>
  motor::IfaceId SimulMotorAdapter<RobType,Sign>::getIfaceId(){
    return IFACEID;
  }
  
   template<typename RobType,typename Sign>
   SimulMotorAdapter<RobType,Sign>::SimulMotorAdapter(RobType* pr,double* pm,int pn,
						      typename SimulPropNormalizer<RobType,Sign>::ParamPtr propParam,
						      typename SimulPosNormalizer<RobType,Sign>::ParamPtr posParam							 
						      ):r(pr),m(pm,pn),changed(pn),propNorm(*this,propParam),posNorm(*this,posParam){    
    }

  
  template<typename RobType,typename Sign>
  const char* SimulMotorAdapter<RobType,Sign>::IFACEID="ODE";


}
