/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_PROTECTEDVALUEPROXY
#define HE_PROTECTEDVALUEPROXY

#include <helium/system/valueProxy.h>
#include <helium/system/protectedSetter.h>
#include <helium/system/cellMapping.h>

namespace helium{
  
  /*template<typename C,int flags>
    class RecursiveMap<C,flags,CELL>;*/
 

  
 
  template<typename PGT,
	   typename Reason,
	   Reason remoteReason,
	   typename Acc=DefaultAccessPolicy<PGT>,
	   typename SigType=Signal<typename signalArg<PGT,Acc>::value>,
	   typename signalFuncType<PGT,Acc,SigType>::value remF=&SigType::operator()>
  class ProtectedValueProxy:
    private ValueProxy<PGT,Acc,SigType,remF>,
    public ProtectedSetter<typename ValueProxySign<PGT,Acc,SigType,remF>::ST,
				  typename ValueProxySign<PGT,Acc,SigType,remF>::RUT,				  
				  Reason,remoteReason,
				  ValueProxy<PGT,Acc,SigType,remF>
				  >
  {
    typedef ValueProxy<PGT,Acc,SigType,remF> Super;
    typedef ProtectedSetter<typename ValueProxySign<PGT,Acc,SigType,remF>::ST,
			   typename ValueProxySign<PGT,Acc,SigType,remF>::RUT,				  
			   Reason,remoteReason,
			   ValueProxy<PGT,Acc,SigType,remF>
				   > Setter;


    typedef ValueProxySign<PGT,Acc,SigType,remF> V;
    typedef typename cacheType<V, V::Access::cacheAttachment>::Return CacheReturn;
    typedef typename cacheType<V,V::Access::cacheAttachment>::CacheParam CacheParam;





    template<typename C,int flags,NodeType n>
    friend class RecursiveMap;



  public:


    


    ProtectedValueProxy():Setter((Super&)*this){}
    template<typename X> ProtectedValueProxy(const X& x):Super(x),Setter(*this){}

    typename V::GT forceGet(){
      return Super::forceGet(); 
    }
    
    typename V::GT forceGet(const typename V::GP& p){
      return Super::forceGet(p);       
    }
    
    CacheReturn get(){
      return Super::get();
    }
    
    CacheReturn parGet(const CacheParam& p){
      return Super::parGet(p);
    }

    void get(typename V::GT& g){ //non cached,typed cell style signature
      return Super::get(g);
    }

    void get(typename V::GT& g,const typename V::GP &p){
      return Super::get(g,p);
    }
    
    template<typename X> void connect(X& x){
      Super::connect(x);
    }

    template<typename X> void connect(X& x,const typename V::UP& p){
      Super::connect(x,p);
    }


    void disconnect(ConnReference<typename V::RUT>& c){
      Super::disconnect(c);
    }

    using Setter::connect;
    using Setter::disconnect;
    using Setter::set;
    using Setter::setExcept;
    using Setter::assure;
    using Setter::assureExcept;
  };
  

  /*
    NO need,even a private is recognized 

    definition for correct mapping  by cellMapping

  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,SigType>::value remF>

  struct is_base_or_same_of<helium::GenericValueProxy,
			    ProtectedValueProxy<PGT,Acc,SigType,remF>
			    >{
    static const bool value=true;
  };
  */

}


#endif
