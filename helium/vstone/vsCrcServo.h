/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_VSCRCSERVO
#define HE_VSCRCSERVO

#include <helium/robot/motorInterface.h>
#include <helium/robot/motorStructures.h>
#include <helium/robot/motorException.h>
#include <helium/vstone/vServoSign.h>
#include <helium/vstone/vServoTable.h>
#include <helium/vstone/vsByte.h>

namespace helium{


  namespace vservo{

    typedef MotorGetterToken<VServoSign> GetterToken;
      
  }//ns vservo


  template<typename VsSrl>
  class VsCrcServo:public motor::Raw<VServoSign>::Hardware{
    static const int RETRIES=2;
    
    
    void manageError(int j);

    void throwError(int j,const std::string& );//manages and then throws
    void throwError(int j,const exc::Exception& );

    
  public:
        
    typedef VServoSign::Prop RawProperty;
    typedef VServoSign::Pos RawPosition;    
    typedef  motor::Raw<VServoSign>  RawMotor;

    typedef RawMotor::Token Token;
    typedef RawMotor::PropertyList PropertyList;
    typedef motor::Interface::Position NormPosition;
    
    RawMotor::PropNormalizer& convProp();
    RawMotor::PosNormalizer& convPos();
    
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

    void rawRotate(int, RawPosition);
    void rawRotate(const RawPosition*);
    void rawRotate(const bool*,const RawPosition*);

      
    void connect(ConnReference<NormPosition>&, int);
    void disconnect(ConnReference<NormPosition>&,int j);
    void rawConnect(ConnReference<RawPosition>& conn,int j);
    void rawDisconnect(ConnReference<RawPosition>& conn,int j);

    RawPosition rawGetPosition(int);
    void rawGetPosition(RawPosition* p);


    
    void rotate(int, NormPosition);	
    void rotate(const NormPosition*);
    void rotate(const bool*,const NormPosition*);

    NormPosition getPosition(int j);
    void getPosition(NormPosition* p);

    motor::IfaceId getIfaceId();
   
    void unlock(int id);
    VsCrcServo(VsSrl&,IdMapper& pidMap, motor::Calib<VServoSign>& calib);
    virtual ~VsCrcServo(){}

    void getWorkingOptimizedSplit(Array <WorkingToken>& tk,const HwIdList &) const;
    
    void isWorking(Array<HwState>&,const HwIdList&);
    HwState isWorking(int id);
    void isWorking(Callback<const HwStateNotification&>*,const WorkingToken&);

    int getMotorNum();
  private:   
    VsSrl& vserial;
    IdMapper &idMap;
    LinearPosNormalizer<VServoSign> psNorm;
    vservo::PropNormalize prNorm;

    PosLimiter<VServoSign> limiter;

    enum PowerState{ON,OFF,UNKNOWNPOWER};
    
    Array<int> lastSent;
    Array<unsigned char> expectedCrc,receivedCrc,receivedData;
    Array<bool> toRotate;
    Array<int> limitedPos;
    exc::MultipleExceptionHolder meh;
  };//Vservo
  




  
}//ns helium  


#include <helium/vstone/vsCrcServo.hpp>

#endif
