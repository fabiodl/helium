/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags s
 */


#ifndef HE_ANDROIDMOTORSIGN
#define HE_ANDROIDMOTORSIGN

#include <stdint.h>

namespace helium{
  

  namespace android{
  
    class MotorSign{
    public:
      typedef int Pos;
      typedef int Prop;
      static const bool NEEDSZERO=true;
      static const Pos DEFAULTMIN=0x0;
      static const Pos DEFAULTMAX=0xFF;
      static const Pos DEFAULTZERO=0x80;
      
      static const Pos INVALIDPOS;
      static const Prop INVALIDPROP;
      
      static const Pos MOVINGPOS;
      static const Pos FREEPOS;
      
      static const double TORAW;
      static const double TONORM;      
    };

  }


};

#endif
