#ifndef _HE_SCHEDULEDMOTOR_
#define _HE_SCHEDULEDMOTOR_


#include <helium/robot/motorInterface.h>

namespace helium{
  

  namespace motor{

    class ScheduledMotorInterface{
      virtual void rotate(int j,Interface::Position val)=0;      
      virtual void rotate(const Interface::Position* d)=0;
      virtual void rotate(const bool*, const Interface::Position* d)=0;
    };

  }

}

#endif
