/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * Release_flags s
 * \version 0.1
 */


namespace helium{
  
  namespace mlin{

    
    template<typename P>
    BasicTarget<P>::BasicTarget(){}
    

    template<typename P>
    BasicTarget<P>::BasicTarget(P pposition,Time preachTime, int pj):
      position(pposition),reachTime(preachTime),j(pj){}
    
    template<typename P>
    BasicJointTarget<P>::BasicJointTarget(P pposition,Time preachTime):
      position(pposition),reachTime(preachTime){}

    
    template<typename P>
    BasicJointTarget<P>::BasicJointTarget(const BasicTarget<P>& t):position(t.position),reachTime(t.reachTime){}


    template<typename MotorSign,motor::MotorLevel L>
    inline std::ostream& operator<<(std::ostream& o,const Target<MotorSign,L>& t){
      write(o,t.getType());
      o<<" ";
      t.serialize(o);
      return o;
    }
    
    template<typename MotorSign,motor::MotorLevel L>
      inline std::ostream& operator<<(std::ostream& o,const JointTarget<MotorSign,L>& t){
      write(o,t.getType());
      o<<" ";
      t.serialize(o);
      return o;
    }


    template<typename MotorSign,motor::MotorLevel L>
    JointPVTarget<MotorSign,L>::JointPVTarget(){}

    template<typename MotorSign,motor::MotorLevel L>
    JointPVTarget<MotorSign,L>::JointPVTarget(P ppos,double pv):pos(ppos),v(pv){}

    template<typename MotorSign,motor::MotorLevel L>
    void JointPVTarget<MotorSign,L>::serialize(std::ostream& o) const{
      o<<pos<<" "<<v;
    }
    
    template<typename MotorSign,motor::MotorLevel L>
    void JointPVTarget<MotorSign,L>::deserialize(std::istream& i){
      i>>pos>>v;
    }
    
    
    template<typename MotorSign,motor::MotorLevel L>
    void JointPVTarget<MotorSign,L>::compute(std::list<BasicJointTarget<P> >& target,P currPos) const{
      target.push_back(BasicJointTarget<P>(pos,tAbs(pos-currPos)/v)); //the time is adjusted after the compute in push
      }
    
    template<typename MotorSign,motor::MotorLevel L>
    TargetType  JointPVTarget<MotorSign,L>::getType() const{
      return PVTARGET;
    }
    
    
    template<typename MotorSign,motor::MotorLevel L>
    void JointEPVTarget<MotorSign,L>::serialize(std::ostream& o) const{
      rawHexWrite(o,this->pos);
      o<<" ";
      rawHexWrite(o,this->v);
    }
    
    template<typename MotorSign,motor::MotorLevel L>
    void JointEPVTarget<MotorSign,L>::deserialize(std::istream& i){
      rawHexRead(this->pos,i);
      rawHexRead(this->v,i);
    }
    
    template<typename MotorSign,motor::MotorLevel L>
    TargetType JointEPVTarget<MotorSign,L>::getType() const{
      return EPVTARGET;
    }
    

    template<typename MotorSign,motor::MotorLevel L>    
    void JointHaltTarget<MotorSign,L>::compute(std::list<BasicJointTarget<P> >& target,P currPos) const{
      target.push_back(BasicJointTarget<P>(currPos,0));
      
    }


     template<typename MotorSign,motor::MotorLevel L>
     TargetType JointHaltTarget<MotorSign,L>::getType() const{
       return HALTTARGET;
     }

    


    template<typename MotorSign,motor::MotorLevel L>
    void PVTarget<MotorSign,L>::serialize(std::ostream& o) const{
      write(o,pv);
    }

    template<typename MotorSign,motor::MotorLevel L>
    void PVTarget<MotorSign,L>::deserialize(std::istream& i){
      read(pv,i);
    }
    
    template<typename MotorSign,motor::MotorLevel L>
    void PVTarget<MotorSign,L>::compute(std::list<BasicTarget<P> >& target,P* currPos) const{
      //std::cout<<"compute the whole"<<std::endl;
	for (size_t j=0;j<pv.size();j++){
	  
	  //std::cout<<" ["<<j<<"]="<<180/M_PI*pv[j].first<<"["<<pv[j].first<<"]"<<" in "<<pv[j].second<<" currently "<<180/M_PI*currPos[j]<<std::endl;
	  //std::cout<<"setting at "<<(tAbs(pv[j].first-currPos[j])/pv[j].second)<<std::endl;
	  
	  target.push_back(BasicTarget<P>(pv[j].first,
					  tAbs(pv[j].first-currPos[j])/pv[j].second,
					  j)
			   );
	  
	}
	
    }
    
    template<typename MotorSign,motor::MotorLevel L>
    TargetType PVTarget<MotorSign,L>::getType() const{
	return PVTARGET;
    }
    
    template<typename MotorSign,motor::MotorLevel L>
    PVTarget<MotorSign,L>::PVTarget(int n):pv(n){
    }
    

    template<typename MotorSign,motor::MotorLevel L>
    EPVTarget<MotorSign,L>::EPVTarget(int n):PVTarget<MotorSign,L>(n){
    }

    template<typename MotorSign,motor::MotorLevel L>
    void EPVTarget<MotorSign,L>::serialize(std::ostream& o) const{
      for (size_t i=0;i<this->pv.size();i++){
	rawHexWrite(o,this->pv[i].first);
	o<<" ";
	rawHexWrite(o,this->pv[i].second);
	o<<" ";
      }
    }

    template<typename MotorSign,motor::MotorLevel L>
    void EPVTarget<MotorSign,L>::deserialize(std::istream& in){
      for (size_t i=0;i<this->pv.size();i++){
	rawHexRead(this->pv[i].first,in);
	rawHexRead(this->pv[i].second,in);
      }
    }


    template<typename MotorSign,motor::MotorLevel L>
    TargetType EPVTarget<MotorSign,L>::getType() const{
      return EPVTARGET;
    }
    
    template<typename MotorSign,motor::MotorLevel L>
    void HaltTarget<MotorSign,L>::compute(std::list<BasicTarget<P> >& target,P* currPos) const{
      for (int j=0;j<n;j++){
	target.push_back(BasicTarget<P>(currPos[j],0,j));
      }
    }

    
    template<typename MotorSign,motor::MotorLevel L>
    TargetType HaltTarget<MotorSign,L>::getType() const{
      return HALTTARGET;
    }


    template<typename MotorSign,motor::MotorLevel L>
    void HaltTarget<MotorSign,L>::serialize(std::ostream& ) const{
    }

    template<typename MotorSign,motor::MotorLevel L>
    void HaltTarget<MotorSign,L>::deserialize(std::istream& ){
    }
    


    template<typename MotorSign,motor::MotorLevel L>
    HaltTarget<MotorSign,L>::HaltTarget(int pn):
      PVTarget<MotorSign,L>(pn),
      n(pn){
    }
    

  }//ns mlin

  template <typename MotorSign,motor::MotorLevel L>
  void MotorLinearMode<MotorSign,L>::push(int j,P pos,Time t){
      typename std::list<BasicJointTarget >::reverse_iterator rit;
      for (rit=targets[j].rbegin();
	   rit!=targets[j].rend()&&t<rit->reachTime ;++rit){
	
      }
      targets[j].insert(rit.base(),BasicJointTarget(pos,t));
    }
  

  template <typename MotorSign,motor::MotorLevel L>
  void MotorLinearMode<MotorSign,L>::clearTargets(){
    for (int i=0;i<(int)targets.size();i++){
      targets[i].clear();
    }
  }

  template <typename MotorSign,motor::MotorLevel L>
  void MotorLinearMode<MotorSign,L>::clearTarget(int j){
    targets[j].clear();
  }
  
  template <typename MotorSign,motor::MotorLevel L>
  void MotorLinearMode<MotorSign,L>::pushTargets(std::list<BasicTarget >& target){
      Time t=time.getTime();
      startPos=this->info.currPos;   
      setAll(zeroTime,t);
      for (typename std::list<BasicTarget >::iterator it=target.begin();it!=target.end();++it){
	push(it->j,it->position,it->reachTime+t);
      }
      this->assureSpool();
    }

  template <typename MotorSign,motor::MotorLevel L>
  void MotorLinearMode<MotorSign,L>::pushJointTargets(int j,std::list<BasicJointTarget >& target){
    Time t=time.getTime();
    startPos[j]=this->info.currPos[j];   
    zeroTime[j]=t;
    for (typename std::list<BasicJointTarget >::iterator it=target.begin();it!=target.end();++it){
	push(j,it->position,it->reachTime+t);
	//std::cout<<j<<" new target "<<it->position<<"(="<<std::hex<<it->position<<std::dec<<") from"<<t<<" to end at "<<(it->reachTime+t)<<std::endl;
	//std::cout<<j<<" new target "<<it->position<<" from"<<t<<" to end at "<<(it->reachTime+t)<<", now "<<this->info.currPos[j]<< std::endl;
    }
    
    this->assureSpool();
  }
  
  
  template <typename MotorSign,motor::MotorLevel L>
  void MotorLinearMode<MotorSign,L>::getPosture(bool* moving,P* newPos,Motors& m){
      Time now=time.getTime();
      //std::cout<<"now "<<now<<std::endl;
      for (size_t j=0;j<targets.size();j++){
	typename std::list<BasicJointTarget >::iterator it=targets[j].begin();
	moving[j]=false;
	//we make moving true if we're late, but erase the targets
	//next time the list will be empty, and we will return moving=false
	while(it!=targets[j].end()&&it->reachTime<=now){
	  startPos[j]=newPos[j]=it->position;
	  zeroTime[j]=it->reachTime;
	  moving[j]=true;
	  targets[j].erase(it++);
	}	

	//it=targets[j].begin(); can I really comment this?
	//if (targets[j].size()>0){
	if (it!=targets[j].end()){
	  double ratio=(double)(now-zeroTime[j])/(double)(it->reachTime-zeroTime[j]);


	  //std::cout<<"now="<<now<<"zerotime="<<zeroTime[j]<<" reachTime="<<it->reachTime<<std::endl;
	  newPos[j]=(P)((1-ratio)*startPos[j]+ratio*it->position);
	  moving[j]=true;
	  //std::cout<<"%="<<ratio<<" start="<<startPos[j]<<" targ="<<it->position<<" newPos= "<<newPos[j]<<std::endl;
	}
	
      }//for j       

    }


  template <typename MotorSign,motor::MotorLevel L>
  void MotorLinearMode<MotorSign,L>::setWholeTarget(Motors& m,const Target& st){
    std::list<BasicTarget > target;
    st.compute(target,this->info.currPos);
    clearTargets();
    pushTargets(target);
  }

  template <typename MotorSign,motor::MotorLevel L>
  void MotorLinearMode<MotorSign,L>::setJointTarget(Motors& m,const JointTarget& st,int j){
    std::list<BasicJointTarget > target;
    st.compute(target,this->info.currPos[j]);
    clearTarget(j);
    pushJointTargets(j,target);
  }

  template <typename MotorSign,motor::MotorLevel L>
  typename MotorLinearMode<MotorSign,L>::Target* MotorLinearMode<MotorSign,L>::getWholeTarget(std::istream& i){
    mlin::TargetType type;
    read(type,i);
    //std::cout<<"received type ";
    //print(type)<<std::endl;
    Target* r;
    switch(type){
    case mlin::EPVTARGET:
	r=new mlin::EPVTarget<MotorSign,L>(this->info.n);
	break;
    case mlin::PVTARGET:
      r=new mlin::PVTarget<MotorSign,L>(this->info.n);
      break;
    case mlin::HALTTARGET:
      r=new mlin::HaltTarget<MotorSign,L>(this->info.n);
      break;
    default:
      throw exc::InvalidDataValue(type,"whole target type");
    }
    r->deserialize(i);
    return r;      
  }
  
  template <typename MotorSign,motor::MotorLevel L>
  typename MotorLinearMode<MotorSign,L>::JointTarget* MotorLinearMode<MotorSign,L>::getJointTarget(std::istream& i,int j){
      mlin::TargetType type;
      i>>en::eio>>type;      
      JointTarget* r;
      switch(type){
      case mlin::EPVTARGET:
	r=new mlin::JointEPVTarget<MotorSign,L>();
	break;
      case mlin::PVTARGET:
	r=new mlin::JointPVTarget<MotorSign,L>();
	break;
      case mlin::HALTTARGET:
	r=new mlin::JointHaltTarget<MotorSign,L>();
      default:
	throw exc::InvalidDataValue(type,"whole target type");
      }
      r->deserialize(i);
      return r;      
    }
  

  template <typename MotorSign,motor::MotorLevel L>
  motor::ModeId MotorLinearMode<MotorSign,L>::modeID(){
    return adapter::modeId+"Lin";
  }

  template <typename MotorSign,motor::MotorLevel L>
  MotorLinearMode<MotorSign,L>::MotorLinearMode(Spool& pspool,TimeSource& timeSource):      
    Base(pspool,modeID()),
    time(timeSource),targets(pspool.getMotorNum()),startPos(pspool.getMotorNum()),zeroTime(pspool.getMotorNum()){
  }

  template <typename MotorSign,motor::MotorLevel L>
  void DefDescr<MotorLinearMode<MotorSign,L> >::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      hopname="motors";
      IDPath path("target.linear",".");
      path.downPath(mpAdapter<MotorSign,L>::modeId);
      members.push_back(vectMapping(this->obj.cells.jointTarget,"motor",path));
      members.push_back(scalarMapping(this->obj.cells.wholeTarget,path));
    }
  
  
  
  
}//ns helium
