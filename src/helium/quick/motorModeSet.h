/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */
#ifndef HE_MOTORMODESET
#define  HE_MOTORMODESET

#include <helium/robot/timeSource.h>
#include <helium/robot/motorOffMode.h>
#include <helium/robot/motorLinearMode.h>
#include <helium/robot/motorFramePlayMode.h>
#include <helium/robot/motorNotifiedLinearMode.h>

namespace helium{


  namespace motorModeSet{
    enum ModeSetId{OFFMOTORMODE,LINEARMOTORMODE,FRAMEPLAYMOTORMODE,RAWLINEARMOTORMODE};
  };

  template<typename MotorSign> struct MotorModeSet{
  
    
    MotorOffMode<MotorSign> off;
    MotorLinearMode<MotorSign,helium::motor::IFACELEVEL> mlm;
    MotorFramePlayMode<MotorSign> mfp;
    MotorNotifiedLinearMode<MotorSign,helium::motor::RAWLEVEL>  rmlm;    
    MotorModeSet(MotorSpooler<MotorSign>& ms,TimeSource& ts);
    typename MotorSpooler<MotorSign>::MotorMode& getMotorMode(motorModeSet::ModeSetId);
  };//motorModeSet

    
  template<typename MotorSign> 
  class DefDescr<MotorModeSet<MotorSign> >: public Description<MotorModeSet<MotorSign> >{
  public:
    DefDescr(MotorModeSet<MotorSign>& obj):Description<MotorModeSet<MotorSign> >(obj){
    }
    void  getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);    
  };



}

#include "motorModeSet.hpp"
#endif
