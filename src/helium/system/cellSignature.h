/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_CELLSIGNATURE
#define HE_CELLSIGNATURE
#include <helium/core/isSame.h>
#include <helium/core/void.h>
#include <helium/system/cell.h>

namespace helium{



 class CellSignature{      
  };
  

  
  template<typename PGT,
	   typename PGP=Void,
	   typename PST=PGT,
	   typename PSP=StdReply,
	   typename PUT=PGT,
	   typename PUP=Void,
	   bool GRP=false,
	   bool SRP=false,
	   bool CRP=false
	   >
  struct DefaultSignature:public CellSignature{
    typedef PGT GT;
    typedef PGP GP;
    typedef PST ST;
    typedef PSP SP;
    typedef PUT UT;
    typedef PUP UP;
    static const bool getRequiresParam=GRP;
    static const bool setRequiresParam=SRP;
    static const bool connectRequiresParam=CRP;
  };


template<typename PGT,
	   typename PGP,
	   typename PST,
	   typename PSP,
	   typename PUT,
	   typename PUP,
	   bool GRP,
	   bool SRP,
	   bool CRP
	 >
struct DefaultSignature<DefaultSignature<PGT,PGP,PST,PSP,PUT,PUP,GRP,SRP,CRP> >;
  //for preventing signatures of signatures;



  template<typename Signature>
  void deserializeUpdate(typename Signature::UT&,std::istream&);//used by the remotecell, override if necessary
 
  template<typename Signature>
  void deserializeGet(typename Signature::GT&,std::istream&);//used by the remotecell, override if necessary
 
    template<typename Signature>
  void deserializeSetParam(typename Signature::SP&,std::istream&);//used by the remotecell, override if necessary


  template<typename PGT>
  struct DefaultRoSignature:public CellSignature{
    typedef PGT GT;
    typedef Void GP;
    typedef Void ST;
    typedef Void SP;
    typedef PGT UT;
    typedef Void UP;
    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;
    //typedef DefaultAccessPolicy<GT,GP,ST,SP,UT,UP,
    //getRequiresParam,setRequiresParam,connectRequiresParam,options> Access;
    
  };
  
  template<typename PGT>
  struct DefaultWoSignature:public CellSignature{
    typedef Void GT;
    typedef Void GP;
    typedef PGT ST;
    typedef Void SP;
    typedef Void UT;
    typedef Void UP;
    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;
    //typedef DefaultAccessPolicy<GT,GP,ST,SP,UT,UP,
    //getRequiresParam,setRequiresParam,connectRequiresParam,options> Access;
    
  };

 

  template<typename PGT,bool CS=is_base_or_same_of<CellSignature,PGT>::value>
  struct cellTypes;

  template<typename PGT>
  struct cellTypes<PGT,true>{
    typedef PGT value;
  };

  template<typename PGT>
  struct cellTypes<PGT,false>{
    typedef DefaultSignature<PGT> value;
  };


    enum CacheAttachment{
    NOCELLTYPE,GETCELLTYPE,SETCELLTYPE,UPDATECELLTYPE
  };


  template <typename V,CacheAttachment c>
  struct cacheType{
  };


  template<typename V>
  struct cacheType<V,NOCELLTYPE>{
    typedef typename V::GT Return;
    typedef Void CacheType;
    typedef Void CacheParam;
  };

  template <typename V>
  struct cacheType<V,GETCELLTYPE>{
    typedef const typename V::GT& Return;
    typedef typename V::GT CacheType;
    typedef typename V::GP CacheParam;
 };

  template <typename V>
  struct cacheType<V,SETCELLTYPE>{
    typedef const typename V::ST& Return;
    typedef typename V::ST CacheType;
    typedef typename V::SP CacheParam;
  };


  template <typename V>
  struct cacheType<V,UPDATECELLTYPE>{
    typedef const typename V::UT& Return;
    typedef typename V::UT CacheType;
    typedef typename V::UP CacheParam;
  };




  enum CellEvent{
    ONGET=1,
    ONPARGET=2,

    ONSET=4,
    ONPARSET=8,//this par is different,is the return value
    
    ONUPDATE=16,
    ONPARUPDATE=32,
    
    ONREMOTE=64,
    ONREMOTEANDHASLISTENER=128 //to be used for connectFlags

 };

  enum EventAction{
    SKIPWRITECACHE=1,
    SKIPPARWRITECACHE=2,

    SKIPREMOTESET=4,
    SKIPPARREMOTESET=8,

    SKIPSIGNAL=16,
    SKIPPARSIGNAL=32,

    SKIPCONNECT=64,
    SKIPPARCONNECT=128
    
  };
 
  //use these enums in the specialization of  DefaultAccessOptions or in DerivedAccessPolicy
  enum AccessPolicyQuickOption{
    UPDATEUSESREFERENCE=1<<1,
    COMPLETELYDISABLEAUTOCONNECT=1<<2,
    REQUIRELISTENERSFORAUTOCONNECT=1<<3,
    DISABLERETRIEVEONREMOTE=1<<4,
    DISABLEREMOTELESSGET=1<<5,
    DISABLEREMOTELESSPARGET=1<<6,
    DISABLEREMOTELESSSET=1<<7,
    DISABLEREMOTELESSPARSET=1<<8,
    DISABLEREMOTELESSCONNECT=1<<9,
    DISABLEREMOTELESSPARCONNECT=1<<10,
    DISABLEREMOTELESSDISCONNECT=1<<11,
    ORDERRECURSIVEPROCESSVALUE=1<<12
  };


    
  /*template<typename T,int options=0> class RefSign:public DefaultSignature<T,true,options>{
    };*/
  //used to set the  allowedRemoteless field in DefaultAccessPolicy
  enum AllowedRemoteless{
    REMOTELESSGET=1<<1,
    REMOTELESSPARGET=1<<2,
    REMOTELESSSET=1<<3,
    REMOTELESSPARSET=1<<4,
    REMOTELESSCONNECT=1<<5,
    REMOTELESSPARCONNECT=1<<6,
    REMOTELESSDISCONNECT=1<<7,
  };


  template<typename Sign>
  struct DefaultAccessOptions{
    static const int value=0;
  };


  //options flags are defined in AccessPolicyQuickOption
  template <typename PGT,
	    typename GP=typename cellTypes<PGT>::value::GP,
	    typename ST=typename cellTypes<PGT>::value::ST,
	    typename SP=typename cellTypes<PGT>::value::SP,
	    typename UT=typename cellTypes<PGT>::value::UT,
	    typename UP=typename cellTypes<PGT>::value::UP,
	    bool RPG=cellTypes<PGT>::value::getRequiresParam,
	    bool RPS=cellTypes<PGT>::value::setRequiresParam,
	    bool RPU=cellTypes<PGT>::value::connectRequiresParam,
	    int options=DefaultAccessOptions<PGT>::value>


  struct DefaultAccessPolicy{

    typedef typename cellTypes<PGT>::value::GT GT;

    template<AccessPolicyQuickOption opt> struct isSet{
      static const bool value=((opt&options)!=0);
    };


    static const CacheAttachment cacheAttachment=(VoidToTrue<GT>::value||RPG) ?NOCELLTYPE:GETCELLTYPE;
    static const CacheAttachment parCacheAttachment=(VoidToTrue<GT>::value)?NOCELLTYPE:GETCELLTYPE;
    static const int writeCacheFlags=(VoidToTrue<GT>::value||RPG)?0:
      (
       ONGET|
       ((!RPG&&!isSet<DISABLERETRIEVEONREMOTE>::value)?ONREMOTE:0)|
       (isSame<GT,ST>::value?ONSET|ONPARSET:0)|
       (isSame<GT,UT>::value?ONUPDATE:0)       
       );
    static const int parWriteCacheFlags=VoidToTrue<GP>::value?0:
      (
       ONPARGET|
       (isSame<GT,UT>::value&&isSame<GP,UP>::value?ONPARUPDATE:0)|
       (!isSet<DISABLERETRIEVEONREMOTE>::value?ONREMOTE:0)
       );

    static const int remoteSetFlags=(VoidToTrue<ST>::value||RPS)?0:ONSET; //you may add ONREMOTE
    static const int parRemoteSetFlags=VoidToTrue<ST>::value?0:ONPARSET;//if you add ONSET here, you get that the parametric version is used, and the return value is ignored


    static const int signalFlags=(VoidToTrue<UT>::value||RPU)?0:
      (
       ONUPDATE|       
       (isSame<UT,GT>::value?ONGET:0)|
       (isSame<UT,GT>::value&&!RPG&&!isSet<DISABLERETRIEVEONREMOTE>::value?ONREMOTE:0)|
       (isSame<UT,ST>::value?ONSET|ONPARSET:0)
       );
    static const int parSignalFlags=VoidToTrue<UP>::value?0:
      (
       ONPARUPDATE|
       (isSame<UT,GT>::value?ONPARGET:0)|
       (isSame<UT,GT>::value&&!isSet<DISABLERETRIEVEONREMOTE>::value?ONREMOTE:0)
       );             
    static const int connectFlags=(VoidToTrue<UT>::value||RPU||isSet<COMPLETELYDISABLEAUTOCONNECT>::value)?0: //if we can't connect 0
      isSet<REQUIRELISTENERSFORAUTOCONNECT>::value?ONREMOTEANDHASLISTENER:                           //if we request to have listeners, do it      
      (
       cacheAttachment==UPDATECELLTYPE||
       (cacheAttachment==GETCELLTYPE&&isSame<GT,UT>::value)||
       (cacheAttachment==SETCELLTYPE&&isSame<ST,UT>::value)
       )?      
       ONREMOTE:ONREMOTEANDHASLISTENER; //if it makes sense to update the cache,connect always 
        //otherwise connect if there're listeners (and it is possible,because if not we return 0 soon)
    
    
    static const int parConnectFlags=VoidToTrue<UP>::value?0:
      (
       (isSame<GT,UT>::value?ONREMOTE:0)
       );
    
    static const bool assureUsesGet=isSame<GT,ST>::value&&!RPG;
    static const bool utReference=isSet<UPDATEUSESREFERENCE>::value;
    static const int allowedRemoteless=
      ((options&DISABLEREMOTELESSGET)==0?REMOTELESSGET:0)|
      ((options&DISABLEREMOTELESSPARGET)==0?REMOTELESSPARGET:0)|
      ((options&DISABLEREMOTELESSSET)==0?REMOTELESSSET:0)|
      ((options&DISABLEREMOTELESSPARSET)==0?REMOTELESSPARSET:0)|
      ((options&DISABLEREMOTELESSCONNECT)==0?REMOTELESSCONNECT:0)|
      ((options&DISABLEREMOTELESSPARCONNECT)==0?REMOTELESSPARCONNECT:0)|
      ((options&DISABLEREMOTELESSDISCONNECT)==0?REMOTELESSDISCONNECT:0)
      ;
    static const bool orderRecursiveProcessValue=isSet<ORDERRECURSIVEPROCESSVALUE>::value;
  };


  template<typename Sign,int options>
  struct DerivedAccessPolicy:public DefaultAccessPolicy<typename Sign::GT,
							typename Sign::GP,
							typename Sign::ST,
							typename Sign::SP,
							typename Sign::UT,
							typename Sign::UP,
							Sign::getRequiresParam,
							Sign::setRequiresParam,
							Sign::connectRequiresParam,
							options
							>
  {
    
  };



  template<int e>
  inline std::string describeEvents(){
    std::string s;
    s+=((e&ONGET)!=0)?"get ":"";
    s+=((e&ONPARGET)!=0)?"parget ":"";
    s+=((e&ONSET)!=0)?"set ":"";
    s+=((e&ONSET)!=0)?"parset ":"";
    s+=((e&ONUPDATE)!=0)?"update ":"";
    s+=((e&ONPARUPDATE)!=0)?"parupdate ":"";
    s+=((e&ONREMOTE)!=0)?"remote ":"";
    s+=((e&ONREMOTEANDHASLISTENER)!=0)?"remotAndHasListener":"";
    return s;
  }

  template<int e>
  inline std::string describeAccessPolicyOptions(){
    std::string s;
    s+=((e&UPDATEUSESREFERENCE)!=0)?"updateUsesReference ":"";    
    s+=((e&COMPLETELYDISABLEAUTOCONNECT)!=0)?"completelyDisableAutoconnect ":"";
    s+=((e&REQUIRELISTENERSFORAUTOCONNECT)!=0)?"requireListenersForAutoconnect ":"";
    s+=((e&DISABLERETRIEVEONREMOTE)!=0)?"disableRetrieveOnRemote ":"";
    s+=((e&ORDERRECURSIVEPROCESSVALUE)!=0)?"orderRecursiveProcessValue ":"";    
    return s;
  }

  template<CacheAttachment c>
  inline std::string describeCacheAttachment(){    
    switch(c){
    case NOCELLTYPE:
      return "none";
    case GETCELLTYPE:
      return "get";
    case SETCELLTYPE:
      return "set";
    case UPDATECELLTYPE:
      return "update";
    }
  }

  template<typename P>
  inline std::string describeAccessPolicy(){
    std::stringstream ss;
    
    ss<<"cache is of type ";
    ss<<describeCacheAttachment<P::cacheAttachment>();
    ss<<std::endl;

    ss<<"writeCache on ";
    ss<<describeEvents<P::writeCacheFlags>();
    ss<<std::endl;

    ss<<"parWriteCache on ";
    ss<<describeEvents<P::parWriteCacheFlags>();
    ss<<std::endl;

    ss<<"remoteSet on ";
    ss<<describeEvents<P::remoteSetFlags>();
    ss<<std::endl;
    
    ss<<"parRemoteSet on ";
    ss<<describeEvents<P::parRemoteSetFlags>();
    ss<<std::endl;
    
    ss<<"signal on ";
    ss<<describeEvents<P::signalFlags>();
    ss<<std::endl;

    ss<<"parSignal on ";
    ss<<describeEvents<P::parSignalFlags>();
    ss<<std::endl;
    
    ss<<"connect on ";
    ss<<describeEvents<P::connectFlags>();
    ss<<std::endl;
    
    ss<<"parConnect on ";
    ss<<describeEvents<P::parConnectFlags>();
    ss<<std::endl;
    
    ss<<"assure uses get "<<std::boolalpha<<P::assureUsesGet<<std::endl;
    ss<<"update is reference "<<std::boolalpha<<P::utReference<<std::endl;
    return ss.str();
  }


}

#include <helium/system/cellSignature.hpp>

#endif
