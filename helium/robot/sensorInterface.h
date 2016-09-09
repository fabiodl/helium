/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_SENSORINTERFACE
#define HE_SENSORINTERFACE

#include <helium/smartpointer/reference.h>
#include <helium/core/deletable.h>
#include <helium/core/enum.h>
#include <helium/vector/array.h>
#include <helium/vector/operations.h>
#include <helium/util/ioUtil.h>
#include <helium/system/cellSignature.h>
#include <helium/signal/ehSignal.h>
#include <helium/robot/hwInterface.h>

#include <vector>
#include <set>
namespace helium{

  namespace sensor{
     

    enum ReadType{IDLE,READ,READTYPES};    

  }//sensor

  template<>  struct EnumStrings<sensor::ReadType>{
    static const int NUM=3;
    static const char* strings[NUM];
  };







  template<typename T>
  struct SensorValList{    
    Array<T> vals;
    SensorValList(){}
    SensorValList(int n):vals(n){
    }

    bool operator==(SensorValList& b){
      return equals(vals,b.vals);
    }

  };

  template<typename T>
  struct SensorIdvalList{
    Array<std::pair<int,T> > idvals;
    SensorIdvalList(){
      printTrace();
    }
  };

    
  template<typename T>
  inline std::ostream& operator<<(std::ostream& o,const SensorValList<T>& l){
    sizeWrite(o,l.vals);
    return o;
    }
  
  template<typename T>
  inline std::istream& operator>>(std::istream& i,SensorValList<T>& l){
    sizeRead(l.vals,i);
    return i;
    }


  namespace exc{
    template<typename T>
    class SensorGroupException:public exc::Exception{
    public:
      exc::Exception& problem;
      const T* data;
      const HwIdList& bad;

     
      SensorGroupException(exc::Exception& pproblem,
		       const T* pdata,
		       const HwIdList& pbad
			   ):Exception("Sensor processing error"+addDetail(pproblem.what()) ),problem(pproblem),data(pdata),bad(pbad){
      }
      
      virtual void rethrow(){
	throw *this;
      }
      
      virtual Exception* clone() const{
	return new SensorGroupException(*this);
      }
      
    };//SensorGroupError

  }//exc



  template<typename T>
  struct  SensorCellSignature:public CellSignature{
    typedef T GT;
    typedef Void GP;
    typedef Void ST;
    typedef Void SP;
    typedef T UT;
    typedef sensor::ReadType UP; //it makes no sense making a "connect fast" because the disconnection information needs to be generated
    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;

  };

  template<typename T>
  struct SensorGroupCellSignature:public CellSignature{
    typedef SensorValList<T> GT;
    typedef HwIdList GP;
    typedef Void ST;
    typedef Void SP;
    typedef SensorValList<T> UT;
    typedef std::pair<HwIdList,sensor::ReadType> UP;
    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;
   
  };


  template<typename T> 
  struct DefaultAccessPolicy<SensorGroupCellSignature< T>,
			     typename SensorGroupCellSignature< T>::GP,
			     typename SensorGroupCellSignature< T>::ST,
			     typename SensorGroupCellSignature< T>::SP,
			     typename SensorGroupCellSignature< T>::UT,
			     typename SensorGroupCellSignature< T>::UP,
			     false,
			     false,
			     false,
			     0
			     >{
    static const CacheAttachment cacheAttachment=UPDATECELLTYPE;
      static const CacheAttachment parCacheAttachment=UPDATECELLTYPE;
      static const int writeCacheFlags=ONUPDATE;
      static const int parWriteCacheFlags=ONPARUPDATE;
      static const int remoteSetFlags=0;
      static const int parRemoteSetFlags=0;
      static const int signalFlags=ONUPDATE;
      static const int parSignalFlags=ONPARUPDATE;
      static const int connectFlags=ONREMOTEANDHASLISTENER;
      static const int parConnectFlags=ONREMOTEANDHASLISTENER;
      static const bool assureUsesGet=false;
      static const bool utReference=true;
     static const int allowedRemoteless=0;
  };


  namespace sensor{

    enum HardwareState{SENSOROK,SENSORUNREACHABLE,SENSORINVALID};

    template<typename T>
    struct Set:public HwInterface{      

      typedef SingleReference<Deletable> Token;
      virtual T rawRead(int idx)=0;
      virtual void rawRead(T*)=0; 
      virtual void rawRead(T*, const HwIdList&)=0; 
      virtual void rawRead(SensorValList<T>& , const HwIdList&)=0; 

      virtual void getToken(Token& t, const HwIdList& toRead)=0;

      virtual void rawRead(T*,const Token& t)=0;//T* is for ALL sensors, not just the one in the token

      virtual size_t size() const=0;

      
    };


  




      template<typename T>
      class FilterInterface{  
      public:
	virtual T read(int idx)=0;
	virtual void read(T* )=0;
	virtual void read(T*,const HwIdList& what)=0;
	virtual void read(SensorValList<T>& v,const HwIdList& what)=0;//assumes the sensorgroup size is ok
	virtual void connect(EhConnReference<const T*>& ref,const HwIdList& what, sensor::ReadType type)=0;    
	virtual void disconnect(EhConnReference<const T*>& ref)=0;
      };
  }//ns sensor
  

}//ns helium




#endif
