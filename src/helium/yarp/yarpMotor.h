/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */


#ifndef HE_YARPMOTOR
#define HE_YARPMOTOR

#include <helium/yarp/yarpRpc.h>
#include <helium/yarp/yarpPaths.h>


namespace helium{




  class YarpMotor:private YarpRpc{

  public:
    YarpMotor(YarpClient& c,const YarpPaths& yl);
    void rotate(int j,double d);
    void rotate(double*);//in radians
    void setVelocity(double*);
    void setVelocity(int j,double v);
    double getEncoder(int j);

    void connect(ConnReference<double>&, int j);
    
    
    void getAxesNum(std::vector<int>& axes );
    int  getAxesNum();

  private:


    typedef std::vector<std::pair<int,int> > JointMap;
    





    int getAxes(const std::string& limb);
    void sendAllAxesScaled(double* v,const std::string& what);
    
    YarpClient& client;
    const std::string prefix;
    YarpPaths config;
    JointMap jointmap;
    std::vector<int> axesNum; 
  };


  
  

  


}
#endif
