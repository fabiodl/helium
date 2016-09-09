/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_DYNAMIXELMOTOR
#define HE_DYNAMIXELMOTOR

#include <helium/robot/motorInterface.h>
#include <helium/dynamixel/dynamixelSign.h>
#include <helium/robot/motorStructures.h>


namespace helium{

  template<typename Dsrl,typename Sign=dynamixel::Mx28Sign>
  class DynamixelMotor:public motor::Raw<Sign>::Hardware{





     
	
    public:
    DynamixelMotor(Dsrl& dsrl,IdMapper& pidMap,motor::Calib<Sign>& calib);
      


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
   
   

    void getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList &) const;
    
    void isWorking(Array<HwState>&,const HwIdList&);
    HwState isWorking(int id);
    void isWorking(Callback<const HwStateNotification&>*,const HwInterface::WorkingToken&);

    int getMotorNum();
  private:

    class PropNormalize:public motor::Raw<Sign>::PropNormalizer{
      typename motor::Raw<Sign>::PosNormalizer& posNorm;
      
      motor::Interface::Property normProp(int value,const motor::PropertyRequest& r);
      typename motor::Raw<Sign>::Property unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r);
      
    public:
      PropNormalize(typename motor::Raw<Sign>::PosNormalizer& pposNorm):posNorm(pposNorm){}
      
    };//PropNormalize
    



    Dsrl& dsrl;
    IdMapper& idMap;
    LinearPosNormalizer<Sign> psNorm;
    PropNormalize prNorm;


    PosLimiter<Sign> limiter;
    Array<int> lastSent;

    exc::MultipleExceptionHolder meh;
    Array<bool> toRotate;//temporary 
    Array<typename Sign::Pos> limitedPos;


    void manageError(int j);
    void throwError(int j,const exc::Exception& e);//used to make an error understood by the motorerrorhandler
    void assureCalib(int j);


  };//class DynamixelMotor


}//ns helium







#include <helium/dynamixel/dynamixelMotor.hpp>

#endif
