#ifndef HE_TARGETMOTION
#define HE_TARGETMOTION

#include <helium/robot/frame.h>
#include <helium/robot/motorPlannerMode.h>

namespace helium{



  void motionToTargets(std::list<BasicTarget<double> >& target,std::map<Time,NDRef<Posture> >& motion,int n){
    
  }
  
  class TargetMotion:public Motion,
		     public mplan::TargetInterface<double>{
    
    
    void compute(std::list<BasicTarget<double> >& target,P* currPos){
      for (std::map<Time,NDRef<Posture> >::iterator it=motion.begin();
	 it=!motion.end();it++){
      for (int j=0;j<n;j++){
	if ((*it->second)[j]!=NAN){
	    target.push_back(BasicTarget<double>((*it->second)[j],it->first,j));
	}
      }
      }
      
    }


    void serialize(std::ostream& o){
      std::map<Posture*,int> postures;
      for (std::map<Time,NDRef<Posture> >::iterator it=motion.begin();
	   it!=motion.end();it++){
	postures.insert(make_pair(&it->second,-1) );
      }
      
      int c=0;
      o<<postures.size();
      for (std::map<Time,NDRef<Posture> >::iterator it=postures.begin();
	   it!=postures.end();it++){
	it->second=c;
	o<<" "<<c;
	write(o,*it->first);
	c++;
      }
      
      o<<" "<<motion.size();
      
      for (std::map<Time,NDRef<Posture> >::iterator it=motion.begin();
	   it!=motion.end();it++){
	o<<" "<<it->first<<" "<<postures[it->second];
      }
      
    }


    void deserialize(std::istream& in){
      int pn;
      i>>pn;
      Array<Posture*> postures(pn);
      for (int i=0;i<pn;i++){
	postures[i]=new Posture(n);
	read(*postures[i],in);
      }      
      i>>pn;
      Time t,int idx;
      for (int i=0;i<n;i++){
	i>>t>>idx;
	motion.insert(make_pair(t,NDRef<Posture>(postures[idx])));
      }           
    }
    
    TargetType getType(){
      return PTLINKTARGET;
    }


  };//TargetMotion
    

  
}//helium



#endif
