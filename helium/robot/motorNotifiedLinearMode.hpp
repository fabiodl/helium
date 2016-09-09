/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

namespace helium{

  

  template <typename MotorSign,motor::MotorLevel L>
  void MotorNotifiedLinearMode<MotorSign,L>::MNLMPositionCell::do_getData(typename Base::P& pos){    
    pos=owner.info.currPos[id()];
  }
  
  template <typename MotorSign,motor::MotorLevel L>
  void MotorNotifiedLinearMode<MotorSign,L>::MNLMPositionCell::getData(typename Base::P& pos){
    StackJob<typename Base::Motors,
	     MNLMPositionCell,
	     typename Base::P&,
	     onlyParam<typename Base::P&>,
	     &MNLMPositionCell::do_getData,
	     false
	     > job(this,pos);
      
      
  owner.spool.waitJob(&job,owner.jointTargetSetPriority);
  }



}
