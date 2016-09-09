/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_DYNAMIXELSIGN
#define HE_DYNAMIXELSIGN

namespace helium{
  

  namespace dynamixel{
  
    class Mx28Sign{
    public:
      typedef int Pos;
      typedef int Prop;
      static const bool NEEDSZERO=true;
      static const Pos DEFAULTMIN=0x0;
      static const Pos DEFAULTMAX=0xFFF;
      static const Pos DEFAULTZERO=0x800;
      
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
