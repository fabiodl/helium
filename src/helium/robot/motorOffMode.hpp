/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

namespace helium{
  
  
  
  template<typename MotorSign>
  void MotorOffMode<MotorSign>::addMotor(Motors& m,int j,motor::ModeId prevMode){
    try{
      m.hw->setPowerState(j,false);
    }catch(exc::Exception& e){
      onError.handle(e);
      throw;
    }
  }

  
  template<typename MotorSign>
  void MotorOffMode<MotorSign>::connect(Cell::CellConnection& ref,int j){
    onError.connect(ref);
  }

  template<typename MotorSign>
  void MotorOffMode<MotorSign>::disconnect(Cell::CellConnection& ref,int j){
    onError.disconnect(ref);
  }

  template<typename MotorSign>
  MotorOffMode<MotorSign>::MotorOffMode():MotorSpooler<MotorSign>::MotorMode(motor::OFFMODEID){
  }



}
