/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_OFFMODE
#define HE_OFFMODE

#include <helium/robot/motorSpooler.h>

namespace helium{


  namespace motor{
    extern const char* OFFMODEID;
  }
  
  template<typename MotorSign>
  class MotorOffMode:public MotorSpooler<MotorSign>::MotorMode{
    typedef typename MotorSpooler<MotorSign>::Motors Motors;   

    ExceptionSignal onError;
    
    void addMotor(Motors& m,int j,motor::ModeId prevMode);
    void removeMotor(Motors& m,int j,motor::ModeId newMode){}
    typename MotorSpooler<MotorSign>::NormPosition getPosition(Motors& m,int j){return motor::Interface::FREEPOS;}
    void connect(Cell::CellConnection& ref,int j);
    void disconnect(Cell::CellConnection& ref,int j);

  public:
    MotorOffMode();

  };//OffMode
  


}

#include  <helium/robot/motorOffMode.hpp>

#endif
