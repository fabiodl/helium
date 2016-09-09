/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_WRAPINFO
#define HE_WRAPINFO
#include <helium/vector/array.h>
#include <helium/signal/signal.h>
#include <helium/destroy/autoDestroy.h>
#include <helium/core/void.h>
namespace helium{

  template<typename Add>
  struct PayloadDinfo:public DisconnectInfo{

    DisconnectInfo* dinfo;
    Add add;
    
    PayloadDinfo(){}
    template<typename T>
    PayloadDinfo(const T& padd):add(padd){}

    ~PayloadDinfo(){
      delete dinfo;
    }
    
  };

 

  template<typename Add>
  struct MultiConnDinfo:public DisconnectInfo{

    Array<DisconnectInfo*> dinfo;
    Add add;
    
    MultiConnDinfo(int n):dinfo(n){}
    template<typename T>
    MultiConnDinfo(int n,const T& padd):dinfo(n),add(padd){}
    
    ~MultiConnDinfo(){
      destroyNS(dinfo);
    }
    
  };

  template<>
  struct MultiConnDinfo<Void>:public DisconnectInfo{

    Array<DisconnectInfo*> dinfo;

    
    MultiConnDinfo(int n):dinfo(n){}
    template<typename T>
    MultiConnDinfo(int n):dinfo(n){}
    
    ~MultiConnDinfo(){
      destroyNS(dinfo);
    }
    
  };



  template<typename Add,typename D> Add unwrapInfo(ConnReference<D> &conn); //forward declaration
  template<typename Add,typename D> const Add& peekInfo(ConnReference<D> &conn);
  template<typename Add,typename D> void discardInfo(ConnReference<D> &conn);

  template <typename Add> 
  class EnvelopeDiscInfo:public DisconnectInfo{
  
    DisconnectInfo *real;
    Add add;

    template<typename A,typename C> friend void unwrapInfo(ConnReference<C> &conn,A& info);
    template<typename A,typename C> friend const A& peekInfo(ConnReference<C> &conn);
    template<typename A,typename C> friend void discardInfo(ConnReference<C> &conn);
  public:
    
    EnvelopeDiscInfo(DisconnectInfo *preal,const Add& padd):real(preal),add(padd){	
    }
    
    ~EnvelopeDiscInfo(){
      if (real!=NULL) delete real;
    }
      
  };
  
  
  template<typename Add,typename D> void wrapInfo(ConnReference<D> &conn,const Add& info){
    conn.disc=new EnvelopeDiscInfo<Add>(conn.disc,info);
  }

  template<typename Add,typename D> const Add& peekInfo(ConnReference<D> &conn){
    if (conn.disc==NULL){
      throw exc::Bug("Peek info on disconnected stuff!");
    }
    return trust_cast<EnvelopeDiscInfo<Add>*>(conn.disc)->add;	
  }


  template<typename Add,typename D> void discardInfo(ConnReference<D> &conn){
    EnvelopeDiscInfo<Add>* c=trust_cast<EnvelopeDiscInfo<Add>*>(conn.disc);	
    conn.disc=c->real;
    c->real=NULL;
    delete c;
  }


  
  template<typename Add,typename D> void unwrapInfo(ConnReference<D> &conn,Add& info){
    EnvelopeDiscInfo<Add>* c=trust_cast<EnvelopeDiscInfo<Add>*>(conn.disc);	
    conn.disc=c->real;
    info=c->add;
    c->real=NULL;
    delete c;
  }

}//helium
#endif
