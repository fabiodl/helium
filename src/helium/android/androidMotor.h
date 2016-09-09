/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_ANDROIDMOTOR
#define HE_ANDROIDMOTOR

#include <helium/robot/motorInterface.h>
#include <helium/robot/motorStructures.h>
#include <helium/android/androidMotorSign.h>
#include <helium/core/exception.h>
#include <helium/os/serial.h>
#include <stdint.h>

namespace helium{

  
  class AndroidMotor:public motor::Raw<android::MotorSign>::Hardware{
  public:
    typedef android::MotorSign Sign;
    
    AndroidMotor(Serial& serial,IdMapper& pidMap,motor::Calib<Sign>& calib);
    
    
    typedef typename Sign::Prop RawProperty;
    typedef typename Sign::Pos RawPosition;    
    typedef typename motor::Raw<Sign>::Token Token;
    typedef typename motor::Raw<Sign>::PropertyList PropertyList;
    typedef motor::Interface::Position NormPosition;
    typedef MotorGetterToken<Sign> GetterToken;

    typename motor::Raw<Sign>::PropNormalizer& convProp();
    typename motor::Raw<Sign>::PosNormalizer& convPos();
    
    RawPosition getPotentiometer(int joint,int checks);



    void rawGetProperty(RawProperty& o, const motor::PropertyRequest & r);
    void rawSetProperty(const RawProperty& i, const motor::PropertyRequest & r);
    void rawGetGetterToken(Token& tk,const PropertyList &info);
    void rawGetOptimizedSplit(Array<Token>& tk,const PropertyList &);
    void rawGetProperty(const Token& tk);
    void setPowerState(int j,bool state=true);
    bool getPowerState(int j);
    void getPowerState(bool* b);

    void hwRotate(int, RawPosition);
    void hwRotate(const RawPosition*);
    void hwRotate(const bool*,const RawPosition*);

    void rawRotate(int, RawPosition);//adds limits and ignore 
    void rawRotate(const RawPosition*);
    void rawRotate(const bool*,const RawPosition*);

      
    void connect(ConnReference<NormPosition>&, int);
    void disconnect(ConnReference<NormPosition>&,int j);
    void rawConnect(ConnReference<RawPosition>& conn,int j);
    void rawDisconnect(ConnReference<RawPosition>& conn,int j);

    RawPosition rawGetPosition(int);//returns the target, use potentio for the real value
    void rawGetPosition(RawPosition* p);


    
    void rotate(int, NormPosition);	
    void rotate(const NormPosition*);
    void rotate(const bool*,const NormPosition*);

    NormPosition getPosition(int j);
    void getPosition(NormPosition* p);

    motor::IfaceId getIfaceId();
   
    HwState isWorking(int id);
    void isWorking(Array<HwState>&,const HwIdList&);

    void getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList &) const;
    


    void isWorking(Callback<const HwStateNotification&>*,const HwInterface::WorkingToken&);

    int getMotorNum();


  private:

    
    //this class is used to normalize both pos and props
    class PropertyNormalizer:
      public LinearPosNormalizer<Sign>,
      public motor::Raw<Sign>::PropNormalizer
    {      
      motor::Interface::Property normProp(motor::Raw<Sign>::Property value,const motor::PropertyRequest& r);
      typename motor::Raw<Sign>::Property unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r);
    public:
      PropertyNormalizer(motor::Calib<Sign>&);
    };




    Serial& srl;
    IdMapper& idMap;
    PropertyNormalizer pnorm;
    PosLimiter<Sign> limiter;
    Array<RawPosition> lastSent;
    Array<uint8_t> rotatePacket;

    exc::MultipleExceptionHolder meh;
    //temp variables
    Array<bool> onlyOneSelector;//used by hwRotate j,p
    Array<RawPosition> onlyOneValue;//used by hwRotate j,p
    Array<RawPosition> limitedPos;
    bool globalPowerState;
  };
  



}



#endif
