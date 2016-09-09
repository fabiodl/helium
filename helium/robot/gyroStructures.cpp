/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */


#include <helium/robot/gyroStructures.h>
#include <helium/vector/operations.h>
#include <helium/math/sign.h>
namespace helium{

  
  DefDescr<GyroCalib>::DefDescr(GyroCalib& g): Description<GyroCalib>(g){
  }


  void DefDescr<GyroCalib>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    //std::cout<<"Importing gyro"<<std::endl;
    members.push_back(scalarMapping(obj.xoffset,"xoffset"));
    members.push_back(scalarMapping(obj.yoffset,"yoffset"));
    members.push_back(scalarMapping(obj.zoffset,"zoffset"));
    members.push_back(scalarMapping(obj.xratio,"xratio"));
    members.push_back(scalarMapping(obj.yratio,"yratio"));
  }

 
  void GyroCalib::getRototranslation(double *m,const double * data) {
      /*cout<<"data";
	print(data,3)<<endl;
	cout<<"offset"<<endl;
	print(accOffset,3)<<endl;*/

    /*std::cout<<" with ";
      std::cout<<g.xratio<<" "<<g.xoffset<<std::endl;
      std::cout<<g.yratio<<" "<<g.yoffset<<std::endl;
      std::cout<<" "<<g.zoffset<<std::endl;*/
    
    /*helium::Vector3d z((-data[0]+xoffset)*xratio,
		       (-data[1]+yoffset)*yratio,
		       -data[2]+zoffset);  */

    helium::Vector3d z((data[0]-xoffset)*xratio,
		       (data[1]-yoffset)*yratio,
		       -data[2]+zoffset);  
    
    //std::cout<<"produced "<<z<<std::endl;
    
    helium::normalize(z);
    
    //std::cout<<"normalized as "<<z<<std::endl;
    
    helium::Vector3d x,y;
    //if (fabs(data[1])>fabs(data[0])){
    /*static double angle=0;
      angle=angle+0.1;
      double ro[16];
      helium::setAsRotation(ro,helium::Axis::Z,angle);*/
    
    helium::Vector3d xf(z[1],z[2],z[0]);
    
    //helium::Vector3d xf(1,0,0);
    y.cross(z,xf);
    x.cross(y,z);      
    /*}else{
      helium::Vector3d yf(0,1,0);
      x.cross(yf,z);
      y.cross(z,x);
      }*/
    helium::normalize(x);
    helium::normalize(y);
    
    helium::eye(m);
    m[0]=x.x();  m[1]=x.y();  m[2]=x.z(); 
    m[4]=y.x();  m[5]=y.y();  m[6]=y.z();
    m[8]=z.x();  m[9]=z.y();  m[10]=z.z(); m[11]=0;

    //the gyro is the projection of the (upward) gravity vector on the xyz of the robot reference frame
    // p=R^-1 [0 0 1] i.e. Rp=[0 0 1]


    /*std::cout<<"from ";
      helium::print(data,3)<<std::endl;
      std::cout<<"reconstructed ";
      helium::print(m,4,4)<<std::endl;;*/  
    
  }

  enum{RXX,RXY,RXZ,TX,
       RYX,RYY,RYZ,TY,
       RZX,RZY,RZZ,TZ,
       SX,SY,SZ,ST};
       


  void GyroCalib::getRightToRightRototranslation(double *m,const double * data) {
    
    /* 0  1   2  3
       4  5   6  7
       8  9  10 11
       12 13 14 15*/


    m[TX]=m[TY]=m[TZ]=
      m[SX]=m[SY]=m[SZ]=
      0;
    m[ST]=1;
    
    m[RZX]=(data[0]-xoffset)*xratio;
    m[RZY]=(data[1]-yoffset)*yratio;
    m[RZZ]=-data[2]+zoffset;
    helium::normalizen<3>(m+RZX);
    


    m[RYX]=0;
    double normZYZZ=normn<2>(m+RZY);

    
    m[RYY]=m[RZZ]/normZYZZ;
    m[RYZ]=-m[RZY]/normZYZZ;
    
    m[RXX]=normZYZZ;

    double den=m[RYY]*m[RZZ]-m[RYZ]*m[RZY];
    m[RXY]=m[RXX]*m[RYZ]*m[RZX]/den;
    m[RXZ]=-m[RXX]*m[RYY]*m[RZX]/den;


  }




  void GyroCalib::setCalib(const double* c){
    xoffset = c[0];
    yoffset = c[1];
    zoffset = c[2];
    xratio = c[3];
    yratio = c[4];
  }

}
