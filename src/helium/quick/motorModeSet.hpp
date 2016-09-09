/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */
namespace helium{
  
  template<typename MotorSign> 
  MotorModeSet<MotorSign>::MotorModeSet(MotorSpooler<MotorSign>& ms,TimeSource& ts):
    mlm(ms,ts),
    mfp(ms,ts),
    rmlm(ms,ts){
    ms.registerMode(&off);    
    ms.registerMode(&mlm);    
    ms.registerMode(&mfp);    
    ms.registerMode(&rmlm);    
  }
  
   template<typename MotorSign> 
   typename MotorSpooler<MotorSign>::MotorMode& MotorModeSet<MotorSign>::getMotorMode(motorModeSet::ModeSetId mode){
     switch(mode){
    case motorModeSet::OFFMOTORMODE:
      return off;
    case motorModeSet::LINEARMOTORMODE:
      return mlm;
    case motorModeSet::FRAMEPLAYMOTORMODE:
      return mfp;
    case motorModeSet::RAWLINEARMOTORMODE:
      return rmlm;
     }
     throw exc::InvalidDataValue(mode,"Unknown motor mode");
   }
  
  template<typename MotorSign> 
  void DefDescr< MotorModeSet<MotorSign> >::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(this->dMapping(this->obj.mlm));
    members.push_back(this->dMapping(this->obj.mfp));
    members.push_back(this->dMapping(this->obj.rmlm));
  }
  
  

}
