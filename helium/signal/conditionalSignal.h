#ifndef HE_CONDITIONALSIGNAL
#define HE_CONDITIONALSIGNAL

#include <helium/callback/signal.h>
#include <list>
#include <helium/thread/mutex.h>
#include <helium/system/cell.h>

namespace helium{

  template <typename D,typename P,bool (*cond)(const D&,const P&)>
class AddrConditionalSignal{
    
    Mutex m;
    std::list<std::pair<Callback<const DataP>*,P> > cs;
    
    typedef typename std::list<std::pair<Callback<const DataP>*,P> >::iterator IT;    
   
    class CSDinfo:public DisconnectInfo{
      friend class AddrConditionalSignal;
      IT it;
      CSDinfo(const IT& pit):it(pit){
      }
    };

  public:
    AddrConditionalSignal():m("cs"){
    }
    void connect(ConnReference<const DataP>& r,const P& p){
      m.lock();
      r.disc=new CSDinfo(cs.insert(cs.end(),std::make_pair(r.cb,p) ));
      m.unlock();
    }
    
    void disconnect(ConnReference<D> &r){
       m.lock();
       delete r.disc;
       r.disc=NULL;
       m.unlock();
    }
    

    void operator()(const D& d){
      for (IT it=cs.begin();it!=cs.end();it++){
	if (cond(d,it->second)) (*it->first)((const DataP) &d);
      }
      
    }//
  
  
};



}


#endif
