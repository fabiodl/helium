#include <helium/itaro/rototraToMotors.h>
#include <helium/vector/operations.h>
#include <helium/math/angle.h>
#include <helium/math/algebra.h>

namespace helium{

const double Dim::R=0.020;
const double Dim::C=0.021;
const double Dim:: L2=0.021*0.021+0.075*0.075;
const double Dim::H=0.075;
const double Dim::MINALPHA=-M_PI/6;
const double Dim::MAXALPHA=M_PI/6;



void leverToMotors(double* m,double h,double theta){
  Vector2d rc(Dim::C,0);
  Vector2d rp(Dim::R*cos(theta),Dim::H+h+Dim::R*sin(theta));
  Vector2d rvd;
  sub(rvd,rp,rc);    
  double rd2=squaredNorm(rvd);
  //  double rgammaCos;
  double rgamma=acos(limit(-1.0,1.0, (rd2+Dim::R*Dim::R-Dim::L2)/(2*sqrt(rd2)*Dim::R) ) );  
  m[RLEVER]=M_PI-(atan2(rvd[1],-rvd[0])+rgamma);
  Vector2d lc(-Dim::C,0);
  Vector2d lp(-Dim::R*cos(theta),Dim::H+h-Dim::R*sin(theta));
  Vector2d lvd;
  sub(lvd,lp,lc);  
  double ld2=squaredNorm(lvd);  
  double lgamma=acos(limit(-1.0,1.0, (ld2+Dim::R*Dim::R-Dim::L2)/(2*sqrt(ld2)*Dim::R) ) );  
  m[LLEVER]=M_PI-(atan2(lvd[1],lvd[0])+lgamma);
}


  void rototraToMotors(double* m,double* r){
  static const double RANGE=3*M_PI/180;
  static const double EPS=1*M_PI/180;
  double theta=acos(r[RT_22]);  

  theta=limit(-M_PI/6,M_PI/6,theta);

  double h=r[RT_23];


  double btarget=atan2(-r[RT_12],-r[RT_02]);



  if (angleDist(btarget+M_PI,m[BASE])<angleDist(btarget,m[BASE])){
    btarget=angleNorm_piPi(btarget+M_PI);
    theta=-theta;
    }
  
  //btarget=limit(-M_PI/2,M_PI/2,btarget);


  if (btarget>M_PI/2+EPS){
    btarget-=M_PI;
    theta=-theta;       
  }
  
  if (btarget<-M_PI/2-EPS){    
    btarget+=M_PI;
    theta=-theta;    
  }

  //cout<<btarget<<"  ";
  //cout<<r[RT_02]<<" "<<r[RT_12]<<" ";

  //std::cout<<"theta="<<theta<<std::endl;
  //cout<<"alpha "<<limit(0.0,1.0,1-fabs(theta)/RANGE)<<" "<<limit(0.0,1.0,fabs(theta)/RANGE)<<endl;



  /*  double base= 
    m[BASE]*(limit(0.0,1.0,1-fabs(theta)/RANGE))+   
    btarget*(limit(0.0,1.0,  fabs(theta)/RANGE))
    ;*/

  double base=btarget;


  //cout<<"x="<<-r[RT_12]<<" y = "<<-r[RT_02]<<"atan="<<atan2(-r[RT_12],-r[RT_02])<<endl;

  //cout<<"theta="<<theta<<" base="<<base<<" m[BASE]"<<m[BASE]<<endl;
  
  m[BASE]=base;
  //std::cout<<"base afte"<<m[BASE]<<endl; 
 //cout<<"mbase"<<m[BASE]<<endl;

  Vector3d newX(r[RT_00],r[RT_10],r[RT_20]);  
  //cout<<"newx"<<newX<<endl;
  Vector3d rbaseDir(cos(m[BASE])*cos(theta),sin(m[BASE])*cos(theta),sin(theta));  
  //cout<<"rbaseDir"<<rbaseDir<<endl;  
  Vector3d angleCross;
  angleCross.cross(rbaseDir,newX);  
  //cout<<"angleCross"<<angleCross<<"dot"<<dot(newX,rbaseDir)<< endl;  
  //sign(angleCross[2])* is as taking the dot product with 0 0 1
  //this may have instability in -PI +PI, should be avoided by the base rotation
  m[TOP]=-atan2(-sgn(angleCross[2])*angleCross.norm(),dot(newX,rbaseDir));  
  //cout<<"top"<<m[TOP]<<endl;

  leverToMotors(m,h,theta);
};





  //limits

  double theta(double h,double alpha){
  Vector2d d(Dim::C+ Dim::R*cos(alpha),  
	     Dim::H+h-Dim::R*sin(alpha));
  double d2=squaredNorm(d);
  return acos(limit(-1.0,1.0, (d2+Dim::R*Dim::R-Dim::L2)/(2*sqrt(d2)*Dim::R)))+atan2(d[0],d[1])-M_PI/2;   //the atan2 ordering is correct
}


double h(double theta,double alpha){
  double ldx=(Dim::C+Dim::R*cos(alpha)-Dim::R*cos(theta));
  return Dim::R*sin(alpha)-Dim::R*sin(theta)  +sqrt(Dim::L2-ldx*ldx)-Dim::H;
}


double thetaLimit(double h){
  return (std::max)(0.0, 
		    std::min(theta(h,Dim::MAXALPHA),
			     -theta(h,Dim::MINALPHA)
			     )		    
		    );
}


void hlimits(double& minh,double& maxh,double theta){

  minh=(std::max)(
		  h(theta,Dim::MINALPHA),		  
		  h(-theta,Dim::MINALPHA)
		  );
	       
  maxh=(std::min)(
		  h(theta,Dim::MAXALPHA),
		  h(-theta,Dim::MAXALPHA)
		  );

    
}
}
