/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_KNOWNPOSNORMALIZER
#define HE_KNOWNPOSNORMALIZER

#include <helium/robot/genericMotorSign.h>
#include <helium/robot/motorInterface.h>
#include <helium/signal/callback.h>

namespace helium{
  
  class JointPosConverter{    
  public:
    virtual RelMotorSign::Pos getRelPos(AbsMotorSign::Pos p,AbsMotorSign::Pos zero)=0;    
    virtual double getAbsVelocity(double relVelocity)=0;    
  };

  
  class KnownPosNormalizer{       
  public:
    enum MotorType{VSSV410,DYNAMIXELMX28,ALABANDROID};
    JointPosConverter* getNewJointPosConverter();   
    KnownPosNormalizer(Memory& mem);
  private:
    MotorType type;  
  };
  
  
  template<> class DefDescr<KnownPosNormalizer>{
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
    void onInitializationFinished(helium::Memory& mem);
  };
  
  
}


#endif
