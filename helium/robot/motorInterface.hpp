/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

namespace helium{

  namespace motor{
    namespace spt{
      struct noHwData{
	static const char* NOHARDWAREERRORMSG;
      };
    }

    template<typename MotorSign>
    typename Raw<MotorSign>::PropNormalizer& Raw<MotorSign>::NoHardware::convProp(){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    typename Raw<MotorSign>::PosNormalizer& Raw<MotorSign>::NoHardware::convPos(){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }
    
    template<typename MotorSign>
    typename Raw<MotorSign>::Position Raw<MotorSign>::NoHardware::getPotentiometer(int joint,int checks){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }       

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rawGetProperty(typename Raw::Property& o, const PropertyRequest & r){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rawSetProperty(const typename Raw::Property& i, const PropertyRequest & r){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }
    
    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rawGetGetterToken(Token& tk,const PropertyList &info){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rawGetOptimizedSplit(Array <Token>& tk,const PropertyList &){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rawGetProperty(const Token& tk){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }


    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::setPowerState(int j,bool state){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    bool Raw<MotorSign>::NoHardware::getPowerState(int j){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::getPowerState(bool* ){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rotate(int, Interface::Position){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }
    
    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rotate(const Interface::Position*){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }
    
    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rotate(const bool*,const Interface::Position*){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }


    template<typename MotorSign>
    Interface::Position Raw<MotorSign>::NoHardware::getPosition(int j){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }
    
   

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::getPosition(Interface::Position* p){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }


    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rawGetPosition(typename Raw::Position*){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::connect(ConnReference<Interface::Position>&, int){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }
    
    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::disconnect(ConnReference<Interface::Position>&,int j){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rawConnect(ConnReference<typename Raw::Position>& conn,int j){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rawDisconnect(ConnReference<typename Raw::Position>& conn,int j){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rawRotate(int, typename Raw<MotorSign>::Position){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rawRotate(const typename Raw<MotorSign>::Position*){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::rawRotate(const bool*,const typename Raw<MotorSign>::Position*){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    
    
    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::hwRotate(int, typename Raw<MotorSign>::Position){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::hwRotate(const typename Raw<MotorSign>::Position*){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }
    
    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::hwRotate(const bool*,const typename Raw<MotorSign>::Position*){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    typename Raw<MotorSign>::Position Raw<MotorSign>::NoHardware::rawGetPosition(int){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    IfaceId Raw<MotorSign>::NoHardware::getIfaceId(){
      return "noharware";
    }
    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList &) const{
        throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::isWorking(Array<HwState>&,const HwIdList&){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    HwState Raw<MotorSign>::NoHardware::isWorking(int id){
      throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    void Raw<MotorSign>::NoHardware::isWorking(Callback<const HwStateNotification&>*,const HwInterface::WorkingToken&){
        throw exc::InvalidOperation(spt::noHwData::NOHARDWAREERRORMSG);
    }

    template<typename MotorSign>
    int Raw<MotorSign>::NoHardware::getMotorNum(){
      return 0;
    }
    
    
  }//motor

};//helium
