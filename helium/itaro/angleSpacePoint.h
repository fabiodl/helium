#ifndef HE_ANGLESPACEPOINT
#define HE_ANGLESPACEPOINT

namespace helium{

    class AngleSpacePoint{
    
    double start[4];
    double target[4];
    double curr[4];    
    double propCurr[4];    
    int p;

    Axis::Type ax[3];
    double reachTime,startTime;
  public:
    RotoTranslation roto;
    


    void newPos(double t){
      assignn<4>(start,curr);
      randun<3>(target,-M_PI/6,M_PI/6);      
      //zeron<3>(target);
      target[3]=randu(-0.01,0.01);
      startTime=t;
      reachTime=t+3;
    }


    void updatePos(double t){
      if (t>reachTime){
	newPos(t);
      }
      linearInterpolationn<4>(propCurr,start,target,startTime,reachTime,t);
    }
   
    void set(double* m,double t){
      updatePos(t);
      RotoTranslation r[1],p[3] ;
      for (int i=0;i<3;i++){
	setAsRotation(p[i],ax[i],propCurr[i]);
	//cout<<"curr["<<i<<"]="<<curr[i]<<endl;
	//print(p[i]);
      }
      mul(r[0],p[0],p[1]);
      mul(roto,r[0],p[2]);
      roto[RT_23]=propCurr[3];
      double nm[4];
      rototraToMotors(nm,roto);
      if (angleDist(nm[BASE],m[BASE])<M_PI/40){
	assignn<4>(curr,propCurr);
	assignn<4>(m,nm);
      }else{
	newPos(t);
      }

      //printn<4>(curr);
      //printn<4>(m)<<endl;

    }
    
    AngleSpacePoint():p(0){
      zeron<4>(start);
      randun<3>(target,-M_PI/6,M_PI/6);
      zeron<4>(curr);
      ax[0]=Axis::Z;
      ax[1]=Axis::X;
      ax[2]=Axis::Y;
      eye(roto);
    }
  };



}


#endif
