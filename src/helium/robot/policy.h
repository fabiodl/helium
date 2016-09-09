/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags ge
 */

#ifndef HE_POLICY
#define HE_POLICY

#include <helium/core/time.h>

namespace helium{
  class Policy {
  protected:
    Policy();
  public:
    virtual double getPosture(int id, Time t)=0;
    virtual void getPosture(double* p,Time t)=0;
    virtual Policy* clone()=0;
    virtual Time getEndTime()=0;
  };
}

#endif
