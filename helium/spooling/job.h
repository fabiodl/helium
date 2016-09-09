/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.02.18
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_JOB
#define HE_JOB

#include <helium/core/exception.h>
#include <helium/core/referenceType.h>
#include <helium/thread/semaphore.h>
namespace helium{




  template<typename T>
  class Job{   
  public:
    virtual bool operator()(T& t)=0;    
    virtual bool isToCancel()=0;    //called after an exception is thrown
    virtual ~Job(){}
  };

  template<>
  class Job<void>{
  public:
    virtual bool operator()()=0;    
    virtual bool isToCancel()=0;    //called after an exception is thrown
    virtual ~Job(){}
  };



  template<typename T>
  class WaitableJob:public Job<T>{   
  public:
    virtual void wait()=0;    
  };


  template<typename FP> struct onlyParam{
  };



  template<typename F,typename S> class PairPasser{
  public:
    typename natReference<F>::value first;
    typename natReference<S>::value second;
    PairPasser(typename natReference<F>::value pf,typename natReference<S>::value ps):first(pf),second(ps){}
  };


  template<typename F,typename S> class PairStorage{
  public:
    F first;
    S second;

    template<typename PF,typename PS>
    PairStorage(const PairPasser<PF,PS>& p):first(p.first),second(p.second){
    }
  };




  /*template<typename F,typename S> class PairPasser<const F&,S>{
  public:
    const F& first;
    S& second;
    PairPasser(const F& pf,S& ps):f(pf),s(ps){}    
  };

  template<typename F,typename S> class PairPasser<F,const S&>{
  public:
    F& first;
    const S& second;
    PairPasser(F& pf,const S& ps):f(pf),s(ps){}    
  };


  template<typename F,typename S> class PairPasser<const F&,const S&>{
  public:
    const F& first;
    const S& second;
    PairPasser(const F& pf,const S& ps):f(pf),s(ps){}    
  };
  */

  template<typename T,typename C,typename FP> struct jobFunction{
    typedef void (C::*value)(typename simpleReference<T>::value ,FP); 

    template<typename P,value f>
    inline static void call(C* c,typename simpleReference<T>::value  t,typename simpleReference<P>::value  p){
      (c->*f)(t,p);      
    }
  };

  template<typename T,typename C> struct jobFunction<T,C,void>{
    typedef void (C::*value)(typename simpleReference<T>::value );

    template<typename P,value f>
    inline static void call(C* c,typename simpleReference<T>::value  t){
      (c->*f)(t);      
    }
  };


  template<typename T,typename C,typename FP> struct jobFunction<T,C,onlyParam<FP> >{
    typedef void (C::*value)(FP);

    template<typename P,const value f>
    inline static void call(C* c,typename simpleReference<T>::value  t,typename simpleReference<P>::value  p){
      (c->*f)(p);      
    }
  };

  
  
  
  
  

  template<typename T,typename C> struct jobFunction<T,C,onlyParam<void> >{
    typedef void (C::*value)();

    template<typename P,value f>
    inline static void call(C* c,typename simpleReference<T>::value  t){
      (c->*f)();      
    }
  };



template<typename T,typename C,typename F,typename S> struct jobFunction<T,C,PairPasser<F,S> >{
    typedef void (C::*value)(typename simpleReference<T>::value ,F,S);

    template<typename P,const value f>
    inline static void call(C* c,typename simpleReference<T>::value  t,typename simpleReference<P>::value pp){
      (c->*f)(t,pp.first,pp.second);      
    }
  };

template<typename T,typename C,typename F,typename S> struct jobFunction<T,C,onlyParam<PairPasser<F,S> > >{
    typedef void (C::*value)(F,S);

    template<typename P1,const value f>
    inline static void call(C* c,typename simpleReference<T>::value  t,typename simpleReference<P1>::value pp){
      (c->*f)(pp.first,pp.second);      
    }
  };


  



  template<typename C,typename FP> struct jobFunction<void,C,FP>{
    typedef void (C::*value)(FP); 

    template<typename P,value f>
    inline static void call(C* c,typename simpleReference<P>::value  p){
      (c->*f)(p);      
    }
  };

  template<typename C> struct jobFunction<void,C,void>{
    typedef void (C::*value)();

    template<typename P,value f>
    inline static void call(C* c){
      (c->*f)();      
    }
  };


   
  
  template<typename T,typename C,typename P,typename FP,
	   typename jobFunction<T,C,FP>::value f,
	   bool toCancel,
	   typename PP=typename natReference<P>::value>
  class OnceJob:public Job<T>{   
    C* c;
    P p;

  public:
    OnceJob(C* pc,PP pp):c(pc),p(pp){      
    }
    bool operator()(T& t){
      jobFunction<T,C,FP>:: template call<P,f>(c,t,p);
      return true;
    }    
    virtual bool isToCancel(){
      return toCancel;
    }
  };



  template<typename T,typename C,typename FP,
	   typename jobFunction<T,C,FP>::value f,bool toCancel>
  class OnceJob<T,C,void,FP,f,toCancel,void>:public Job<T>{   
    C* c;

  public:
    OnceJob(C* pc):c(pc){      
    }
    bool operator()(T& t){
      jobFunction<T,C,FP>:: template call<void,f>(c,t);
      return true;
    }

    virtual bool isToCancel(){
      return toCancel;
    }
    
  };


  //throws the exception in the thread that launched the job
  template<typename T,typename C,typename P,typename FP,
	   typename jobFunction<T,C,FP>::value f,
	   bool toCancel,
	   typename PP=typename natReference<P>::value>
  class EhOnceJob:public Job<T>{   
    C* c;
    P p;
    exc::ExceptionHandler& eh;
  public:
    EhOnceJob(C* pc,PP pp,exc::ExceptionHandler& peh):c(pc),p(pp),eh(peh){      
    }
    bool operator()(T& t){
      try{
	jobFunction<T,C,FP>::template call<P,f>(c,t,p);
      }catch(exc::Exception& e){
	eh.handle(e);
	throw;
      }
      return true;
    }

    virtual bool isToCancel(){
      return toCancel;
    }

  };

  


  template<typename T,typename C,typename P,typename FP,
	   typename jobFunction<T,C,FP>::value f ,
	   bool toCancel,
	   typename PP=typename natReference<P>::value >
  class AutoDeleteJob:public Job<T>{   
    C* c;
    P p;   
  public:
    AutoDeleteJob(C* pc,PP pp):c(pc),p(pp){      
    }
    bool operator()(T& t){      
      jobFunction<T,C,FP>::template call<P,f>(c,t,p);
      delete this;
      return true;      
    }
    
    virtual bool isToCancel(){
      if (toCancel){
	delete this;
      }
      return toCancel;
    }

  };

template<typename T,typename C,typename FP,typename jobFunction<T,C,FP>::value f,bool toCancel>
class AutoDeleteJob<T,C,void,FP,f,toCancel,void>:public Job<T>{   
    C* c;

  public:
    AutoDeleteJob(C* pc):c(pc){      
    }
    bool operator()(T& t){
      try{
	jobFunction<T,C,FP>::template call<void,f>(c,t);
	delete this;
      }catch(exc::Exception& ){
	if (toCancel) delete this;
	throw;
      }
      return true;
    }        

     virtual bool isToCancel(){
      return toCancel;
    }
    
  };

  
  //throws the exception in the thread that launched the job
  template<typename T,typename C,typename P,typename FP, //P is stored, FP is the function param
	   typename jobFunction<T,C,FP>::value f,
	   bool toCancel,
	   typename PP=typename natReference<P>::value >
  class StackJob:public WaitableJob<T>{   
    C* c;
    P p;
    EhSemaphore s;
  public:
    StackJob(C* pc,PP pp):c(pc),p(pp){      
    }
    bool operator()(T& t){
      try{
	jobFunction<T,C,FP>::template call<P,f>(c,t,p);
	s.signal();      
      }catch(exc::Exception& e){
	s.handle(e);
	throw;
      }

      return true;
    }    
    void wait(){
      s.wait();      
    }

    virtual bool isToCancel(){
      return toCancel;
    }

  };
  

  template<typename T,typename C,typename FP,typename jobFunction<T,C,FP>::value f,bool toCancel>
  class StackJob<T,C,void,FP,f,toCancel,void> :public WaitableJob<T>{   
    C* c;
    EhSemaphore s;
  public:
    StackJob(C* pc):c(pc){      
    }
    bool operator()(T& t){
      try{
	jobFunction<T,C,FP>::template call<void,f>(c,t);
	s.signal();      
      }catch(exc::Exception& e){
	s.handle(e);
	throw;
      }

      return true;
    }    
    void wait(){
      s.wait();      
    }

    virtual bool isToCancel(){
      return toCancel;
    }

  };

  


  //throws the exception in the thread that launched the job
  template<typename C,typename P,typename FP, //P is stored, FP is the function param
	   typename jobFunction<void,C,FP>::value f,
	   bool toCancel,
	   typename PP >
  class StackJob<void,C,P,FP,f,toCancel,PP>:public WaitableJob<void>{   
    C* c;
    P p;
    EhSemaphore s;
  public:
    StackJob(C* pc,PP pp):c(pc),p(pp){      
    }
    bool operator()(){
      try{
	jobFunction<void,C,FP>::template call<P,f>(c,p);
	s.signal();      
      }catch(exc::Exception& e){
	s.handle(e);
	throw;
      }

      return true;
    }    
    void wait(){
      s.wait();      
    }

    virtual bool isToCancel(){
      return toCancel;
    }

  };
  

  template<typename C,typename FP,typename jobFunction<void,C,FP>::value f,bool toCancel>
  class StackJob<void,C,void,FP,f,toCancel,void> :public WaitableJob<void>{   
    C* c;
    EhSemaphore s;
  public:
    StackJob(C* pc):c(pc){      
    }
    bool operator()(){
      try{
	jobFunction<void,C,FP>::template call<void,f>(c);
	s.signal();      
      }catch(exc::Exception& e){
	s.handle(e);
	throw;
      }

      return true;
    }    
    void wait(){
      s.wait();      
    }

    virtual bool isToCancel(){
      return toCancel;
    }

  };




  /*
  template<typename C,typename P,typename FP,bool (C::*f)(T&,FP) >
  class OnceNotifiedJob<void,C,{   
    P p;
    C* c;
    Semaphore& s;
  public:
    OnceJob(C* pc,Semaphore& ps):c(pc),s(ps){      
    }
    bool operator()(T&n t){
      (c->*f)(t,p);     
      s.signal();
      delete this;
      return true;
    }    
    };*/

}

#endif
