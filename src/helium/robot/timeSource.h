/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_TIMESOURCE
#define HE_TIMESOURCE

#include <helium/os/time.h>

namespace helium{
  class TimeSource{
  public:
    virtual Time getTime()=0;
  };
  

  class RealTimeSource:public TimeSource{
  public:
    Time getTime(){
      return getSystemMillis();
    }
  };

  class SecondRefTimeSource:public TimeSource{
    volatile double& t;

  public:
    Time getTime(){
      //std::cout<<"t="<<t<<std::endl;
      return (Time)(t*1000);
    }
    SecondRefTimeSource(double& pt):t(pt){
    }
  };


}


#endif
