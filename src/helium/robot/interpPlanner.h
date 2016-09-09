#ifndef HE_INTERPPLANNER
#define HE_INTERPPLANNER

#include <helium/robot/motorPlannerMode.h>
#include <helium/robot/timeSource.h>

namespace helium{


  namespace mplan{

  template<typename P>
  class InterpPlanner:public Planner<P> {
    TimeSource& time;

   

    Array<std::list<BasicJointTarget<P> > > targets;
    Array<P> startPos;
    Array<Time> zeroTime;

    void push(int j,P pos,Time t){
      typename std::list<BasicJointTarget<P> >::reverse_iterator rit;
      for (rit=targets[j].rbegin();
	   rit!=targets[j].rend()&&t<rit->reachTime ;++rit){
	
      }
      targets[j].insert(rit.base(),BasicJointTarget<P>(pos,t));
    }


    void clearTargets(){
      for (int i=0;i<(int)targets.size();i++){
	targets[i].clear();
      }
    }

    void clearTarget(int j){
      targets[j].clear();
    }
    


    void pushTargets(std::list<BasicTarget<P> >& target,const P* currPos){
      Time t=time.getTime();
      startPos=currPos;   
      setAll(zeroTime,t);
      for (typename std::list<BasicTarget<P> >::iterator it=target.begin();it!=target.end();++it){
	push(it->j,it->position,it->reachTime+t);
      }
      this->workRequested();
    }


    void pushJointTargets(int j,std::list<BasicJointTarget<P> >& target,P currPos){
      Time t=time.getTime();
      startPos[j]=currPos;   
      zeroTime[j]=t;
      for (typename std::list<BasicJointTarget<P> >::iterator it=target.begin();it!=target.end();++it){
	push(j,it->position,it->reachTime+t);
	std::cout<<j<<" new target from"<<t<<" to end at "<<(it->reachTime+t)<<std::endl;
      }

      this->workRequested();
    }
    
    void getPosture(bool* moving,P* newPos,const P* currPos){
      Time now=time.getTime();
      //std::cout<<"now "<<now<<std::endl;
      for (int j=0;j<(int)targets.size();j++){
	typename std::list<BasicJointTarget<P> >::iterator it=targets[j].begin();
	moving[j]=false;
	while(it!=targets[j].end()&&it->reachTime<=now){
	  startPos[j]=newPos[j]=it->position;
	  zeroTime[j]=it->reachTime;
	  moving[j]=true;
	  targets[j].erase(it++);
	}	

	it=targets[j].begin();
	if (targets[j].size()>0){
	  double ratio=(double)(now-zeroTime[j])/(double)(it->reachTime-zeroTime[j]);
	  newPos[j]=(P)((1-ratio)*startPos[j]+ratio*it->position);
	  moving[j]=true;
	  std::cout<<"%="<<ratio<<" start="<<startPos[j]<<" targ="<<it->position<<" newPos= "<<newPos[j]<<std::endl;
	}
	
      }      

    }
    void setMask(bool* mask){}

  public:
    InterpPlanner(TimeSource& timeSource,int n):
      time(timeSource),targets(n),startPos(n),zeroTime(n){
    }



  };

  }//mplan

}//helium



#endif
