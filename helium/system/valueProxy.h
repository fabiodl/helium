/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_VALUEPROXY
#define HE_VALUEPROXY
#include <set>
#include <memory>
#include <helium/thread/sharedAccess.h>
#include <helium/core/isConvertible.h>
#include <helium/core/addClass.h>
#include <helium/core/selectByValue.h>
#include <helium/signal/signal.h>
#include <helium/signal/wrapInfo.h>

#include <helium/system/cell.h>
#include <helium/system/cellClient.h>
#include <helium/system/cellSignature.h>




namespace helium{


  namespace exc{
    class OperationOnDisconnectedProxy:public exc::InvalidOperation{
    public:

      OperationOnDisconnectedProxy(const std::string& s);      
      virtual void rethrow();      
      Exception* clone();
      
    };
  };

  

  class GenericValueProxy:public Cell,public ActionOnceNode<Cell*>{
  protected: 
    
    Cell* remote;
    SharedAccess acc;  

    void action(Cell* c);
    GenericValueProxy();

    virtual void remoteAvailable()=0;//for valueproxy,typed operations
    virtual void remoteUnset()=0;

    

  public:
    virtual ~GenericValueProxy(){}
    virtual void onRemoteAvailable()=0; //for subclasses of valueproxy, custom
    virtual void onRemoteUnset()=0;



    virtual Cell* getTypedRemote(CellClient* c,const std::string& cellname)=0;

    void setRemote(Cell* c);    
    void unsetRemote();
    Cell* getRemote();
    

    const CellProperties* getProperties() const;


    virtual void getData(DataP out);//these methods call the remote corresponding
    virtual void getData(DataP out,CellSignalData in);
    virtual void setData(CellSignalData in);
    void safeSetData(CellSignalData in);//safe= call the remote only if !null
    virtual void setData(DataP out,CellSignalData in);   
    virtual void safeSetData(DataP out,CellSignalData in);
    void streamGet(std::ostream& o);
    void print(std::ostream& o);
    

    void streamGet(std::ostream& o,std::istream& i);
    void streamSet(std::istream& i);
    void streamSet(std::ostream& o,std::istream& i);
    void connect(CellConnection& ref);
    void connect(CellConnection& ref,CellSignalData p);
    void connect(CellConnection& ref,std::istream& i);
    void disconnect(CellConnection &d);
    void formatSignal(std::ostream& o,CellSignalData data);
    void formatParam(std::ostream& o,CellSignalData data);    
    bool isConnected();
    int getRemoteFlagSet(CellFlags f);
    int getRemoteCapabilities();
    
  };


 

  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^



  enum WorkType{NOWORK,WWORK,PARWORK};

  template<typename V,typename VP,WorkType type>
  struct workType;

  template<typename V,typename VP>
  struct workType<V,VP,NOWORK>{
    typedef Void value;    
  };

  template<typename V,typename VP>
  struct workType<V,VP,WWORK>{
    struct value{
      VP* owner;
      void operator()(const typename V::UT& u){
	owner->notify(u);
      }
      void handle(exc::Exception& e){
	owner->handleRemote(e);
      }
    
    };
  };

  template<typename V,typename VP>
  struct workType<V,VP,PARWORK>{
    struct value{
      VP* owner;
      const typename V::UP* p;
      void operator()(const typename V::UT& u){
	owner->parNotify(u,*p);
      }
      void handle(exc::Exception& e){
	owner->parHandleRemote(e,p);
      }
    };
  };

  

  
  template <typename V,typename Work>
  struct SigData:public Cell::CellCallback,public Cell::CellConnection{
    typename V::SignalType sig; 
    Work work;
    
    void handle(exc::Exception& e){
      work.handle(e);
    }

    void operator()(Cell::CellSignalData p){
      const typename V::UT& u=*trust_cast<const typename V::UT*>(p);


      work.owner->valueProcessor.callSignalRemF(sig,u);
      
      //onRemoteF is simple invoking sig.*f, with f the function specified by the sign of the valueproxy
      //V::onRemoteF(sig,u); //we call onRemoteF for the remote signals, and the normal operator for other signals
      work(u);
    }

  public:

    SigData():Cell::CellConnection(this){}   
     
  };


  template <typename V>
  struct SigData<V,Void>{   
  };






  

  template<typename V>
  struct WCache{
    typename cacheType<V,V::Access::cacheAttachment>::CacheType wCache;
  };
    

  template<typename V>
  struct ParCache{
    std::map<
      typename cacheType<V,V::Access::parCacheAttachment>::CacheParam,
      typename cacheType<V,V::Access::parCacheAttachment>::CacheType
      > parCache;
  };
    
  template<typename V,typename VP,bool hasWork>
  struct WSignal{
    SigData<V,typename workType<V,VP,hasWork?WWORK:NOWORK>::value> wSignal;
  };

  template<typename V,typename VP,bool hasWork>
  struct ParSignal{
    std::map<typename V::UP,
	     SigData<V,typename workType<V,VP,hasWork?PARWORK:NOWORK>::value>*
	     > parSignal;
  };




  template<typename V,typename VP> 
  struct vpDataSelector{

    static const bool hasWork=(ONUPDATE&V::Access::writeCacheFlags)!=0;
    static const bool hasParWork=(ONPARUPDATE&
      (V::Access::writeCacheFlags|
       V::Access::signalFlags|
       V::Access::parWriteCacheFlags|
       V::Access::parSignalFlags ))!=0;
    static const bool hasSignal=(V::Access::signalFlags|V::Access::connectFlags)!=0;
    static const bool hasParSignal=(V::Access::parSignalFlags|V::Access::parConnectFlags)!=0;


    typedef typename addClass<GenericValueProxy,WCache<V>,V::Access::writeCacheFlags!=0>::value Layer1;
    typedef typename addClass<Layer1,WSignal<V,VP,hasSignal>,hasSignal >::value Layer2;
    typedef typename addClass<Layer2,ParCache<V>,V::Access::parWriteCacheFlags!=0>::value Layer3;
    typedef typename addClass<Layer3,ParSignal<V,VP,hasParWork>,hasParSignal>::value value;//Layer4;

   
  };

  //when you connect, get the parametric values, for the cache 
  template<typename V,typename VP,
	   bool C=(V::Access::parWriteCacheFlags&ONREMOTE)!=0,
	   bool S=(V::Access::parSignalFlags&ONREMOTE)!=0> class RetrieveIdx;

  template<typename V,typename VP> class RetrieveIdx<V,VP,false,false>{
  };
  
  template<typename V,typename VP> class RetrieveIdx<V,VP,true,false>{
  public:
    typedef typename std::map<
      typename cacheType<V,V::Access::parCacheAttachment>::CacheParam,
      typename cacheType<V,V::Access::parCacheAttachment>::CacheType
      >::iterator IT;        

    typedef typename cacheType<V,V::Access::parCacheAttachment>::CacheParam R;
  private:
    VP* v;
    IT it;

  public:

    RetrieveIdx(VP* pv):v(pv),it(v->parCache.begin()){      
    }

    const R* getNext(){
      if (it==v->parCache.end()) return NULL;
      return  &(it++)->first;
    }
    
  };

  
  template<typename V,typename VP> class RetrieveIdx<V,VP,false,true>{
  public:
    typedef typename std::map<
      typename V::UP,
    SigData<V,typename workType<V,VP,vpDataSelector<V,VP>::hasParWork?PARWORK:NOWORK>::value>*
      >::iterator IT;

    
    typedef typename V::UP R;
  private:
    VP* v;
    IT it;

  public:

    RetrieveIdx(VP* pv):v(pv),it(v->parSignal.begin()){      
    }

    const R* getNext(){
      if (it==v->parSignal.end()) return NULL;
      return  &(it++)->first;
    }

  };

  template<typename V,typename VP> class RetrieveIdx<V,VP,true,true>{

    typedef typename std::map<
      typename cacheType<V,V::Access::parCacheAttachment>::CacheParam,
      typename cacheType<V,V::Access::parCacheAttachment>::CacheType
      >::const_iterator CIT;        


    /*typedef typename std::map<
      typename V::UP,
      SigData<V,typename workType<V,VP,vpDataSelector<V,VP>::hasParWork?PARWORK:NOWORK>::value>
      >::const_iterator SIT;*/

    typedef typename std::map<typename V::UP,
	     SigData<V,typename workType<V,VP,vpDataSelector<V,VP>::hasParWork?PARWORK:NOWORK>::value>*
			      >::const_iterator SIT;

  public:  
    typedef typename V::UP R;  
    typedef typename std::set<R>::const_iterator IT;

  private:
    std::set<R> s;
    VP* v;
    IT it;


  public:

    RetrieveIdx(VP* pv):v(pv){            
      for (CIT cit=v->parCache.begin();cit!=v->parCache.end();++cit){
	s.insert(cit->first);
      }

      for (SIT sit=v->parSignal.begin();sit!=v->parSignal.end();++sit){
	s.insert(sit->first);
      }
      it=s.begin();

    }
    const R* getNext(){
      if (it==s.end()) return NULL;
      return &*it++;
    }

  };



  template<bool b> struct vpOper{

    template<typename VP> static void FAILCOMPILE(VP* v);//for debug
    template<typename VP> static void assureConnection(VP* v,const std::string& oper);
    template<typename Return,typename VP> static Return getCache(VP* v);
    template<typename Return, typename VP,typename P> static Return getParCache(VP* v,const P& p);
    template<typename VP,typename X> static void writeCache(VP* v,const X& x);
    template<typename VP,typename X,typename P> static void parWriteCache(VP* v,const X& x,const P& p);    
    template<typename VP,typename X> static void  sigW(VP* v,X& x);
    
    template<typename VP,typename X> static void sigW(VP* v,X& x,const Callback<typename VP::V::RUT>* noEx);

    template<typename VP,typename X,typename P> static void sigPar(VP* v,X& x,const P& p);

    template<typename VP,typename X,typename P> static void sigPar(VP* v,X& x,const P& p,const Callback<typename VP::V::RUT>* noEx);
    template<typename VP> static inline void signalCache(VP* v);
    template<typename VP,typename P> static void signalParCache(VP* v,const P& p);
    template<typename VP> static bool  isToSet(VP* v,const typename VP::V::ST& st);
    template<typename VP> static void wlinkOwner(VP* v);    
    template<typename VP> static bool isConnected(VP* v);    
    template<typename VP> static void connect(VP* v,ConnReference<typename VP::V::RUT>& c);
    template<typename VP> static void connect(VP* v,Callback<typename VP::V::RUT>* c);
    
    template<typename VP>
    struct SignalObjPtr{
      typedef typename selectFirstOnTrue<typename VP::ParSigData* ,
					 void,
					 b>::value value;   
    };

    template<typename VP> static typename SignalObjPtr<VP>::value
    createSignal(VP* v,const typename VP::V::UP& p);//call inside a shareduse



    template<typename VP> static typename  SignalObjPtr<VP>::value
    assureSignal(VP* v,const typename VP::V::UP& p);//call inside a shareduse: calls createSignal if necessary
    
    template<typename VP,typename P>  static void connectSingleParCache(VP* v, const P& p);
    template<typename VP,typename P>  static void retrieveSingleParCache(VP* v, const P& p);

    template<typename VP> static void parConnect(VP* v,ConnReference<typename VP::V::RUT>& c,const typename VP::V::UP& p);//connection requested by the user

    template<typename VP> static void parConnect(VP* v,Callback<typename VP::V::RUT>* c,const typename VP::V::UP& p);
    
    //returns if this was without parameters (otherwise we need to use parDisconnect)
    template<typename VP> static bool disconnect(VP* v,ConnReference<typename VP::V::RUT>& c);
    template<typename VP> static void parDisconnect(VP* v,ConnReference<typename VP::V::RUT>& c);

    template<typename VP> static void remoteConnect(VP* v); //connection when the remote becomes available
    template<typename VP> static void remoteConnectOnListeners(VP* v);//connection when the remote becomes available

    template<typename VP> static void parRemoteConnect(VP* v);//connection when the remote becomes available
    template<typename VP> static void parRemoteConnectOnListeners(VP* v);//connection when the remote becomes available

    template<typename VP> static void clearConn(VP* v);
    template<typename VP> static void parClearConn(VP* v);

        
    template<typename VP,typename X> static void safeSet(VP* v,const X& st);
    template<typename VP,typename X> static void discardRetValSafeSet(VP* v,const X& st);

    template<typename VP> static void retrieveValue(VP* v);    
    template<typename VP> static void parRetrieveValue(VP* v);
 
    template<typename VP> static void onRemoteAvailableSet(VP* v);
    template<typename VP> static void parOnRemoteAvailableSet(VP* v);
    
  };


  template<typename PGT,typename Access> struct signalArg{
   typedef typename selectFirstOnTrue< 
     const typename cellTypes<PGT>::value::UT&,
     typename cellTypes<PGT>::value::UT,
     Access::utReference     
     >::value value;
  };

  template<typename PGT,typename Access,typename SigType> struct signalFuncType{
   
    typedef void (SigType::*value)(typename signalArg<PGT,Access>::value);
 };
  
  template<typename PGT,typename Acc,
	   typename SigType,
	   typename signalFuncType<PGT,Acc,SigType>::value remF=&SigType::operator()>
  
  struct ValueProxySign{
    typedef typename cellTypes<PGT>::value Signature;
    typedef Acc Access;
    typedef SigType SignalType;
    static void onRemoteF(SignalType& s,const typename cellTypes<PGT>::value::UT& u){//always reference
      (s.*remF)(u);
    }

    typedef typename Signature::GT GT;
    typedef typename Signature::GP GP;
    typedef typename Signature::ST ST;
    typedef typename Signature::SP SP;
    typedef typename Signature::UT UT;
    typedef typename Signature::UP UP;

    typedef typename signalArg<PGT,Acc>::value RUT;

    static const bool getRequiresParam=Signature::getRequiresParam; 
    static const bool setRequiresParam=Signature::setRequiresParam;
    static const bool connectRequiresParam=Signature::connectRequiresParam;
  };
  

  template<typename VP,bool orderRecursiveProcessValue>
  class ValueProcessor;
  
  template<typename VP>
  class ValueProcessor<VP,false>{   
  protected:
    VP* owner;
  public:
    ValueProcessor(VP*);
    template<typename Access,CellEvent e,int mask,typename X> void processValue(X& x);
    void callSignalRemF(typename VP::V::SignalType& s,const typename VP::V::UT&);
  };


  template<typename VP>
  class ValueProcessor<VP,true>:
    public ValueProcessor<VP,false>{
    Mutex m;
    std::vector<Callback<void>*> pendingUpdates;

    void pushUpdate(Callback<void>*);
  public:
    ValueProcessor(VP*);
    template<typename Access,CellEvent e,int mask,typename X> void processValue(X& x);
    void callSignalRemF(typename VP::V::SignalType& sig,const typename VP::V::UT& u);
  };


  template<typename PGT,typename Acc=DefaultAccessPolicy<PGT>,
	   typename SigType=Signal<typename signalArg<PGT,Acc>::value>,
	   typename signalFuncType<PGT,Acc,SigType>::value remF=&SigType::operator()>

  class ValueProxy:public vpDataSelector<ValueProxySign<PGT,Acc,SigType,remF>,
					 ValueProxy<PGT,Acc,SigType,remF> >::value,
									    public CellProperties{    
    
  public:    
    typedef ValueProxySign<PGT,Acc,SigType,remF> V;
    typedef SigData<V,typename workType<V,ValueProxy,vpDataSelector<V,ValueProxy>::hasParWork?PARWORK:NOWORK>::value> ParSigData;


    friend class RetrieveIdx<V,ValueProxy,(V::Access::parWriteCacheFlags&ONREMOTE)!=0,(V::Access::parSignalFlags&ONREMOTE)!=0>;

    ValueProcessor<ValueProxy,Acc::orderRecursiveProcessValue> valueProcessor;
  private:
    
    friend struct vpOper<false>;
    friend struct vpOper<true>;
    



    template<CellEvent e,int mask,typename X> void processValue(X& x);   
    template<CellEvent e,int mask,typename X> void processValue(X& x,const Callback<typename ValueProxy::V::RUT>* noEx);
    template<CellEvent e,int mask,typename X,typename P> void parProcessValue(const X& x,const P& p);

    template<CellEvent e,int mask,typename X,typename P> void parProcessValue(const X& x,const P& p,const Callback<typename ValueProxy::V::RUT>* noEx);

   

  protected:
    
    void onRemoteAvailable(){} //to be ovverided when needed
    void onRemoteUnset(){}

    void remoteAvailable();//calls onRemoteAvailable() and does necessary stuff
    void remoteUnset();//calls onRemoteUnset() and does necessary stuff
    
    friend struct workType<V,ValueProxy,WWORK>;
    friend struct workType<V,ValueProxy,PARWORK>;

    //VP specified for VS2005
  public://VS2005 cannot use them even if friended
    void notify(const typename ValueProxy::V::UT& ut);
    void parNotify(const typename ValueProxy::V::UT& ut,
		   const typename ValueProxy::V::UP& p);
    


    

  public:


    ValueProxy(const Void&);

    template<typename T> ValueProxy(const T& t);       
    ~ValueProxy();
    //ValueProxy inserted for a visual studio bug
    typedef typename cacheType<typename ValueProxy::V, 
			       ValueProxy::V::Access::cacheAttachment>::Return CacheReturn;
    typedef typename cacheType<typename ValueProxy::V, 
			       ValueProxy::V::Access::parCacheAttachment>::Return ParCacheReturn;
  

    CacheReturn get();    

    
    
    ParCacheReturn parGet(const typename cacheType<typename ValueProxy::V,
						   ValueProxy::V::Access::parCacheAttachment>::CacheParam& p);//creates locally if it is not exisiting only in case REMOTELESSPARGET is set





    void get(typename ValueProxy<PGT,Acc,SigType,remF>::V::GT& g);//non cached,typed cell style signature
    
    void get(typename ValueProxy::V::GT& g,const typename ValueProxy::V::GP &p); //non cached,typed cell style signature     


    void setCache(const typename cacheType<typename ValueProxy::V,
					   ValueProxy::V::Access::cacheAttachment>::CacheType& s);


    void setCache(const typename cacheType<typename ValueProxy::V,
					   ValueProxy::V::Access::parCacheAttachment>::CacheType& s,
		  const typename cacheType<typename ValueProxy<PGT,Acc,SigType,remF>::V,
					   ValueProxy::V::Access::parCacheAttachment>::CacheParam& p);


    void retrieveAndSignal();
    void signalCache();
    void clearCache();
    void signalParCache(const typename ValueProxy::V::GP &p);

    void signalData(const typename ValueProxy::V::UT& ut);
    void signalData(const typename ValueProxy::V::UT& ut,const typename ValueProxy::V::UP& p);


    void set(const typename ValueProxy::V::ST& s);

    void set(typename ValueProxy::V::SP& sp,const typename ValueProxy::V::ST& st);

    const typename ValueProxy::V::ST& operator=(const typename ValueProxy::V::ST& s);//set and retrun the same      
    void setExcept(const typename ValueProxy::V::ST& s,const Callback<typename ValueProxy::V::RUT>* noEx);
    void setExcept(const typename ValueProxy::V::ST& s,const ConnReference<typename ValueProxy::V::RUT>& noEx);

    bool isToSet(const typename ValueProxy::V::ST& st);

    void assure(const typename ValueProxy::V::ST& st);

    void assureExcept(const typename ValueProxy::V::ST& st,const Callback<typename ValueProxy::V::RUT>* noEx);


    inline void assureExcept(const typename ValueProxy::V::ST& st,const ConnReference<typename ValueProxy::V::RUT>& noEx);


    void connect(ConnReference<typename ValueProxy::V::RUT>& c);
    void connect(Callback<typename ValueProxy::V::RUT>* c);

    void connect(ConnReference<typename ValueProxy::V::RUT>& c,const typename ValueProxy::V::UP& p);

    void connect(Callback<typename ValueProxy::V::RUT>* c,const typename ValueProxy::V::UP& p);

    void disconnect(ConnReference<typename ValueProxy::V::RUT>& c);

    std::string getName() const;


    virtual void handleRemote(exc::Exception& e);//override when necessary: default just throws
   
    virtual void parHandleRemote(exc::Exception& e,const typename ValueProxy::V::UP* p);//override when necessary: default just throws
     
    Cell* getTypedRemote(CellClient* c,const std::string& cellname);
       
    virtual int getCapabilities() const;
    virtual const CellProperties* getProperties() const;
    virtual int getCellFlags()const;
  
    //using GenericValueProxy::isConnected;

  };



}//helium

#include <helium/system/valueProxy.hpp>

#endif
