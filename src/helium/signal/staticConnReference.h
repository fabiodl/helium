/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags ge
 */


#ifndef HE_STATICCONNREFERENCE
#define HE_STATICCONNREFERENCE

#include <helium/signal/signal.h>

namespace helium{

  template<typename D,typename C,typename P,typename scb::MemberFunctionType<P,C>::value>
  struct CallbackType{   
  };
  
  
  template<typename D,typename C,void (C::*f)(void)>
  struct CallbackType<D,C,void,f>{   
    typedef scb::IgnoreParamClassCallback<D,C,f> value;
  };


  template<typename D,typename C,void (C::*f)(D&)>
  struct CallbackType<D,C,D&,f>{   
    typedef scb::ImplicitConvertClassCallback<D,C,D&,f> value;
  };


  template<typename D,typename C,void (C::*f)(const D&)>
  struct CallbackType<D,C,const D&,f>{   
    typedef scb::ImplicitConvertClassCallback<D,C,const D&,f> value;
  };
  

  template<typename D,typename C,void (C::*f)(D)>
  struct CallbackType<D,C,D,f>{   
    typedef scb::ClassCallback<D,C,f> value;
  };
  
  template <typename C,void (C::*f)()>
  struct CallbackType<void,C,void,f>{
    typedef scb::ClassCallback<void,C,f> value;
  };


  template <typename F,typename S,typename C,void (C::*f)(F)>
  struct CallbackType<std::pair<F,S>,C,F,f>{
    typedef scb::PairClassCallback<std::pair<F,S>,C,F,f,1> value;
  };

  template <typename F,typename S,typename C,void (C::*f)(S)>
  struct CallbackType<std::pair<F,S>,C,S,f>{
    typedef scb::PairClassCallback<std::pair<F,S>,C,S,f,2> value;
  };


  template <typename F,typename S,typename C,void (C::*f)(const F&)>
  struct CallbackType<std::pair<F,S>,C,const F&,f>{
    typedef scb::PairClassCallback<std::pair<F,S>,C,const F&,f,1> value;
  };

  template <typename F,typename S,typename C,void (C::*f)(const S&)>
  struct CallbackType<std::pair<F,S>,C,const S&,f>{
    typedef scb::PairClassCallback<std::pair<F,S>,C,const S&,f,2> value;
  };
  

  //D is the type of the signal
  //C is the listener's class
  //P is the parameter taken by the listener's function
  //f is the listener's function
  template<typename D,typename C,typename P,typename scb::MemberFunctionType<P,C>::value f> 
  class StaticConnReference:public CallbackType<D,C,P,f>::value,public ConnReference<D>{        
  public:
    StaticConnReference(C* c):CallbackType<D,C,P,f>::value(c),ConnReference<D>(this){}
    
  };


  template<typename D,typename C,typename P,typename scb::ParamMemberFunctionType<D,C,P>::value f,typename PS=P,typename PP=typename natReference<P>::value> 
  class ParamStaticConnReference:public scb::ParamClassCallback<D,C,P,f,PS,PP>,public ConnReference<D>{        
  public:
    ParamStaticConnReference(C* c,PP p):scb::ParamClassCallback<D,C,P,f,PS,PP>(c,p),ConnReference<D>(this){}
    
  };



  template<typename D,typename C,typename P,typename scb::ParamFirstMemberFunctionType<D,C,P>::value f,typename PS=P,typename PP=typename natReference<P>::value> 
  class ParamFirstStaticConnReference:public scb::ParamFirstClassCallback<D,C,P,f,PS,PP>,public ConnReference<D>{        
  public:
    ParamFirstStaticConnReference(C* c,PP p):scb::ParamFirstClassCallback<D,C,P,f,PS,PP>(c,p),ConnReference<D>(this){}
    
  };



  template<typename D,typename C,typename P,typename scb::MemberFunctionType<P,C>::value m,typename PS=P,typename PP=typename natReference<P>::value>
  class ReplaceParamStaticConnReference:public scb::ReplaceParamClassCallback<D,C,P,m,PS,PP>,public ConnReference<D>{
  public:
    ReplaceParamStaticConnReference(C* c,PP p):scb::ReplaceParamClassCallback<D,C,P,m,PS,PP>(c,p),ConnReference<D>(this){}        
  };

  template<typename D,typename C,typename P,typename scb::MemberFunctionType<P,C>::value m,typename PC,PC pval>
  class ReplaceTConstStaticConnReference:public scb::ReplaceTConstClassCallback<D,C,P,m,PC,pval>,public ConnReference<D>{
  public:
    ReplaceTConstStaticConnReference(C* c):scb::ReplaceTConstClassCallback<D,C,P,m,PC,pval>(c),ConnReference<D>(this){}        
  };
  



   template<typename C,typename P,typename scb::MemberFunctionType<P,C>::value f> struct FuncPlaceHolder{
  };


  template<typename D> class InternalConnReference:public ConnReference<D>{
  public:
    template<typename C,typename P, typename scb::MemberFunctionType<P,C>::value f> 
    InternalConnReference(C* c,const FuncPlaceHolder<C,P,f>&):ConnReference<D>(new typename CallbackType<D,C,P,f>::value(c)){
    }
    
    virtual ~InternalConnReference(){
      delete this->cb; 
    }
    
  };

  
  



}

#endif
