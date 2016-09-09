#ifndef HE_MULTIMOTORMAP
#define HE_MULTIMOTORMAP

#include <helium/robot/motorInterface.h>
#include <helium/core/exception.h>

namespace helium{

 
 namespace supp{
    enum MotorOperationLevel {HWMOTOP,RAWMOTOP,IFACEMOTOP,HWBOOLMOTOP,IFACEBOOLMOTOP};
    template<typename Sign,MotorOperationLevel l> struct motOpT;
 }


  template<typename Sign>
  class MultiMotorMap:public motor::Raw<Sign>::Hardware,
		      public motor::Raw<Sign>::PropNormalizer,
		      public motor::Raw<Sign>::PosNormalizer    
  {
  public:
    
    typedef motor::Raw<Sign> RawMotor;
    typedef typename motor::Raw<Sign>::Hardware Hw;



    typedef typename motor::Interface::Position NormPosition;
    typedef typename motor::Interface::Property NormProperty;

    static const int INVALIDID;

    motor::Interface::Property normProp(typename Sign::Prop value,const motor::PropertyRequest& r);


    typename Sign::Prop unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r);
    
    motor::Interface::Position normPos(typename Sign::Pos value,int joint);
    
    typename Sign::Pos unnormPos(motor::Interface::Position value,int joint);
    
    typename RawMotor::PropNormalizer& convProp();

    typename RawMotor::PosNormalizer& convPos();

    
    void addMapping(int virtId,typename motor::Raw<Sign>::Hardware* hw,int hwId);
   

    typename Sign::Pos getPotentiometer(int j,int checks);

    void rawGetProperty(typename Sign::Prop& o, const motor::PropertyRequest & r);


    void rawSetProperty(const typename Sign::Prop& i, const motor::PropertyRequest & r);


    typedef std::map<Hw*, typename RawMotor::PropertyList> PropertyListMap;



    
    void rawGetGetterToken(typename RawMotor::Token& tk,const typename RawMotor::PropertyList &info);

    void rawGetOptimizedSplit(Array<typename RawMotor::Token>& tk,const typename RawMotor::PropertyList & info);

    void rawGetProperty(const typename RawMotor::Token& tk);


    void setPowerState(int j,bool state=true);

    bool getPowerState(int j);

    void getPowerState(bool* b);

    void hwRotate(int j, typename Sign::Pos p);

    void hwRotate(const typename Sign::Pos* p);

    void hwRotate(const bool* b,const typename Sign::Pos* p);
    void rawRotate(int j, typename Sign::Pos p);

    
    void rawRotate(const typename Sign::Pos* p);
    void rawRotate(const bool* b,const typename Sign::Pos* p);

      
    void connect(ConnReference<NormPosition>& conn, int j) ;
    
    void disconnect(ConnReference<NormPosition>& conn,int j);
    
    void rawConnect(ConnReference<typename Sign::Pos>& conn,int j);
    
    void rawDisconnect(ConnReference<typename Sign::Pos>& conn,int j);

    typename Sign::Pos rawGetPosition(int j);

    void rawGetPosition(typename Sign::Pos* p);

    void rotate(int j, NormPosition p);

    void rotate(const NormPosition* p);
    
    void rotate(const bool* b,const NormPosition* p);

    NormPosition getPosition(int j);

    void getPosition(NormPosition* p);

    motor::IfaceId getIfaceId();


    HwState isWorking(int j);
    

    void isWorking(Array<HwState>& s,const HwIdList& idList);





   
    void getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList &idList) const;
    

    class IdSwapCallback:public Callback<const HwStateNotification&>{      
      std::vector<int>& idMap;
      Callback<const HwStateNotification&>* cb;

    public:
      IdSwapCallback( std::vector<int>& pidMap,Callback<const HwStateNotification&>* pcb);
      void operator()(const HwStateNotification& n);
      
    };


    void isWorking(Callback<const HwStateNotification&>* cb,const HwInterface::WorkingToken& tk);
    

    MultiMotorMap(int virtMotorNum,int maxHwMotorNum);

    int getMotorNum();


  private:
    exc::MultipleExceptionHolder meh;
    Array<std::pair<Hw*,int> > motMap; //ftom virtual id to hw id
    typedef std::map<Hw*,std::vector<int> > HwMap;

    HwMap hardwares;//from hw id to virtual id

    
    Array<bool> tmpPower;
    Array<NormPosition> tmpNorm;
    Array<typename Sign::Pos> tmpRaw;
    Array<HwState> tmpHwState;


    inline Hw* hw(int joint) const{
      return motMap[joint].first;
    }
  
    inline int id(int joint) const{
      return motMap[joint].second;
    }

    inline motor::PropertyRequest propReq(const motor::PropertyRequest& p){           
      return motor::PropertyRequest(id(p.joint),p.id,p.idx);
    }



    class PropCallback:public Callback<typename RawMotor::PropertyNotify*>{
      Callback<typename RawMotor::PropertyNotify*>* cb;
      typename motor::PropertyRequest req;//the virtual
      typename RawMotor::PropertyNotify notification;

      PropCallback& operator=(const PropCallback&);

    public:	
      typename motor::PropertyRequest hwReq;//the one to the hw

      PropCallback(const PropCallback& pc);      
      PropCallback(const typename RawMotor::PropertyNotifyRequest& pnr,int hardId,typename RawMotor::PropNormalizer* pnorm);

      void operator()(typename RawMotor::PropertyNotify* pn);
    };





    class GetterToken:public Deletable{
    public:      
      Array<std::pair<Hw*,typename RawMotor::Token> > tokens;         
      void setSingle(Hw* hw,typename RawMotor::Token& hwToken);      
      GetterToken(){}
      GetterToken(Hw* hw,typename RawMotor::Token& hwToken);
      
    };//GetterToken


    class CbHolderGetterToken:public GetterToken{
    public:      
      std::list<PropCallback> callbacks;            
    };//GetterToken
    
    

    
    void splitPropertyList(PropertyListMap& infos, const typename RawMotor::PropertyList &info,CbHolderGetterToken* gt);
      
      

    template<typename T> void hwToVirt(T* virtData,const T* hwData,Hw* hw);
    
    template<typename T> void virtToHw(T* hwData,const T* virtData,Hw* hw);



    

    
    template<supp::MotorOperationLevel l,	     
	     void (supp::motOpT<Sign,l>::Mot::*m)(typename supp::motOpT<Sign,l>::D*)> void getFromHw(typename supp::motOpT<Sign,l>::D* tmp,typename supp::motOpT<Sign,l>::D* data);



    template<supp::MotorOperationLevel l,	     
	     void (supp::motOpT<Sign,l>::Mot::*m)(const typename supp::motOpT<Sign,l>::D*)
	     > void setToHw(typename supp::motOpT<Sign,l>::D* tmp,const typename supp::motOpT<Sign,l>::D* data);


    template<supp::MotorOperationLevel l,	     
	     void (supp::motOpT<Sign,l>::Mot::*m)(const bool*,const typename supp::motOpT<Sign,l>::D*)      
	     > void setToHwConditional(typename supp::motOpT<Sign,l>::D* tmp,const typename supp::motOpT<Sign,l>::D* data,const bool* b);
    
    class GetWorkingToken:public Deletable{
    public:
      Hw* hw;
      HwInterface::WorkingToken tok;
      GetWorkingToken(Hw* phw,HwInterface::WorkingToken& ptok):hw(phw),tok(ptok){
      }
    };

  

  };//MultiMotorMap
  template<typename Sign>
  const int MultiMotorMap<Sign>::INVALIDID=-1;

}//ns helium



#include <helium/robot/multiMotorMap.hpp>

#endif
