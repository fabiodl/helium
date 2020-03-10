/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */


#ifndef HE_CALLBACK_
#define HE_CALLBACK_
#include <list>

#include <helium/core/referenceType.h>
#include <helium/debug/trust_cast.h>

namespace helium{

template <typename D> class Callback{
 public:
  virtual void operator()(D d)=0;
  virtual ~Callback(){}
};





template <> class Callback <void>{
  public:
  virtual void operator()()=0;
  virtual ~Callback(){}
};


  template <typename D> class ClonableCallback:public Callback<D>{
 public:

  virtual Callback<D>* clone() const=0;
};




  namespace dcb{

  

   

 template <typename D,typename P> class FunctionParamCallback:public Callback<D>{
  P p;
  void (*f)(D,P);
 public:
 FunctionParamCallback(void (*pf)(D,P),const P& pp):p(pp),f(pf){
  }

  void operator()(D d){
    (*f)(d,p);
  }
};


 template <typename P> class FunctionParamCallback<void,P>:public Callback<void>{
  P p;
  void (*f)(P);
 public:
 FunctionParamCallback(void (*pf)(P),const P& pp):p(pp),f(pf){
  }
  void operator()(){
    (*f)(p);
  }
};


    template <typename D> class FunctionParamCallback<D,void>:public Callback<D>{
      typedef void (*Func)(D);
      Func f;
    public:
      FunctionParamCallback(Func pf):f(pf){
      }
      void operator()(D d){
	(*f)(d);
      }
};



//a parameter passed to ()
  template <typename D,typename C> class ClassCallback:public Callback<D>{
  C* c;
  void (C::*m)(D);
 public:
  void operator()(D d){
    (c->*m)(d);
  }
  Callback<D>* clone() const{
    return new ClassCallback<D,C>(c,m);    
  }
  
    ClassCallback(C* pc,void (C::*pm)(D)):c(pc),m(pm){
    }
    
   
    
    

 };//class ClassCallback




 
 template <typename D,typename C>
   class IgnoreParamClassCallback:public ClonableCallback<D>{  
    C* c;
    void (C::*m)();
 public:
  void operator()(D d){
    (c->*m)();
  }
  Callback<D>* clone() const {
    return new IgnoreParamClassCallback<D,C>(c,m);    
  }  
 IgnoreParamClassCallback(C* pc,void (C::*pm)()):c(pc),m(pm){
  }  
 };//class IgnoreParamClassCallback


//no parameter passed to () 
template <typename C> class ClassCallback<void,C>:public Callback<void>{
  C* c;
  void (C::*m)();
 public:
  void operator()(){
    (c->*m)();
  }
  Callback<void>* clone() const{
    return new ClassCallback<void,C>(c,m);    
  }
 ClassCallback(C* pc,void (C::*pm)()):c(pc),m(pm){
  }
};

//adds an additional parameter stored with the callback
 template <typename D,typename C,typename P> class ParamClassCallback:public Callback<D>{
  C* c;
  void (C::*m)(D,P);
  P p;
 public:
  void operator()(D d){
    (c->*m)(d,p);
  }
  Callback<D>* clone() const{
    return new ParamClassCallback<D,C,P>(c,m,p);    
  }
 ParamClassCallback(C* pc,void (C::*pm)(D,P),const P &pp):c(pc),m(pm),p(pp){
  }
};//class ParamClassCallback

 template <typename C,typename P> class ParamClassCallback<void,C,P>:public Callback<void>{
  C* c;
  void (C::*m)(P);
  P p;
 public:
  void operator()(){
    (c->*m)(p);
  }
  Callback<void>* clone() const{
    return new ParamClassCallback<void,C,P>(c,m,p);    
  }
 ParamClassCallback(C* pc,void (C::*pm)(P),const P &pp):c(pc),m(pm),p(pp){
  }
};//class ParamClassCallback


//adds an additional parameter stored with the callback
 template <typename D,typename C,typename P> class ParamFirstClassCallback:public Callback<D>{
  C* c;
   void (C::*m)(P,D);
  P p;
 public:
  void operator()(D d){
    (c->*m)(p,d);
  }
  Callback<D>* clone() const{
    return new ParamFirstClassCallback<D,C,P>(c,m,p);    
  }
   ParamFirstClassCallback(C* pc,void (C::*pm)(P,D),const P &pp):c(pc),m(pm),p(pp){
  }
};//class ParamClassCallback



 template <typename D,typename C,typename D2> class CastClassCallback :public Callback<D>{
  C* c;
  void (C::*m)(D2);
  
  void operator()(D d){
    (c->*m)( (D2)d);
  }
 CastClassCallback(C* pc,void (C::*pm)(D2)):c(pc),m(pm){
  } 
};//class CastClassCallback


 template <typename D,typename C,typename D2> class ImplicitConvertClassCallback :public Callback<D>{
  C* c;
  void (C::*m)(D2);
  
  void operator()(D d){
    (c->*m)( d);
  }
    Callback<D>* clone() const{
    return new ImplicitConvertClassCallback(c,m);
  }

 public:
 ImplicitConvertClassCallback(C* pc,void (C::*pm)(D2)):c(pc),m(pm){
  } 

  
};//class CastClassCallback




 template <typename D, typename C,typename D2> class CastDerefClassCallback:public Callback<D>{
  C* c;
  void (C::*m)(D2);
  void operator()(D d){
    (c->*m)( *(trust_cast<D2*>(d)));
  }
 public:
  CastDerefClassCallback(C* pc,void (C::*pm)(D2)):c(pc),m(pm){
  } 

  Callback<D>* clone() const{
    return new CastDerefClassCallback(c,m);    
  }
};//class CastDerefClassCallback

  }//dcb
 
  namespace scb{
    
    template <typename D,typename P> struct ParamFunctionType{
      typedef void (*value)(D,P);
    };

    template <typename P> struct ParamFunctionType<void,P>{
      typedef void (*value)(P);
    };

    

    template <typename D,typename C> struct MemberFunctionType{
      typedef void (C::*value)(D);
    };
    
    template <typename C> struct MemberFunctionType<void,C>{
      typedef void (C::*value)();
    };

    template <typename D,typename C,typename P> struct ParamMemberFunctionType{
      typedef void (C::*value)(D,P);
    };

    template <typename D,typename C,typename P> struct ParamFirstMemberFunctionType{
      typedef void (C::*value)(P,D);
    };

    
    template <typename C,typename P> struct ParamMemberFunctionType<void,C,P>{
      typedef void (C::*value)(P);
    };


    



    template <typename D,typename P,typename ParamFunctionType<D,P>::value f> class FunctionParamCallback:public Callback<D>{
       P p;
       
       
       FunctionParamCallback(const P& pp):p(pp){
       }
       
       void operator()(D d){
	 (*f)(d,p);
       }
     };


    template <typename P,void (*f)(P)> class VoidFunctionParamCallback:public Callback<void>{
      P p;
      
      
      VoidFunctionParamCallback(const P& pp):p(pp){
      }
      void operator()(){
	(*f)(p);
      }
};


//a parameter passed to ()
    template <typename D,typename C,typename MemberFunctionType<D,C>::value m> class ClassCallback:public Callback<D>{
      C* c;
      
    public:
      void operator()(D d){
	(c->*m)(d);
      }
      Callback<D>* clone() const{
	return new ClassCallback(c);    
      }
      
      ClassCallback(C* pc):c(pc){
    }
          

 };//class ClassCallback




 
    template <typename D,typename C,void (C::*m)()>
   class IgnoreParamClassCallback:public Callback<D>{  
      C* c;
    public:
      void operator()(D d){
	(c->*m)();
      }
      Callback<D>* clone() const {
	return new IgnoreParamClassCallback(c);    
      }  
      IgnoreParamClassCallback(C* pc):c(pc){
  }  
 };//class IgnoreParamClassCallback

    template<typename D,typename C,typename P,typename MemberFunctionType<P,C>::value m,typename PS=P,typename PP=typename natReference<P>::value> 
    class ReplaceParamClassCallback:public Callback<D>{
      C* c;
      PS p;
    public:
      void operator()(D d){
	(c->*m)(p);
      }
      ReplaceParamClassCallback(C* pc,PP pp):c(pc),p(pp){
      }

    };


    template<typename D,typename C,typename P,typename MemberFunctionType<P,C>::value m,typename PC,PC pval> 
    class ReplaceTConstClassCallback:public Callback<D>{
      C* c;
    public:
      void operator()(D d){
	(c->*m)(pval);
      }
      ReplaceTConstClassCallback(C* pc):c(pc){
      }

    };

    template<typename C,typename P,typename MemberFunctionType<P,C>::value m,typename PC,PC pval> 
    class ReplaceTConstClassCallback<void,C,P,m,PC,pval>:public Callback<void>{
      C* c;
    public:
      void operator()(){
	(c->*m)(pval);
      }
      ReplaceTConstClassCallback(C* pc):c(pc){
      }

    };



//no parameter passed to () 
    template <typename C,void (C::*m)()> class VoidClassCallback:public Callback<void>{
    protected:
      C* c;
      
    public:
      void operator()(){
	(c->*m)();
      }
      Callback<void>* clone() const{
	return new VoidClassCallback(c);    
      }
      VoidClassCallback(C* pc):c(pc){
      }
};


//adds an additional parameter stored with the callback
    template <typename D,typename C,typename P,typename ParamMemberFunctionType<D,C,P>::value m,typename PS=P,typename PP=typename constReference<P>::value> class ParamClassCallback:public ClonableCallback<D>{
      C* c;
      
      PS p;
    public:
      void operator()(D d){
	(c->*m)(d,p);
      }
      Callback<D>* clone() const{
	return new ParamClassCallback(c,p);    
      }
      ParamClassCallback(C* pc,PP pp):c(pc),p(pp){
  }
};//class ParamClassCallback




//adds an additional parameter stored with the callback
    template <typename D,typename C,typename P,typename ParamFirstMemberFunctionType<D,C,P>::value m,typename PS=P,typename PP=typename constReference<P>::value> class ParamFirstClassCallback:public ClonableCallback<D>{
      C* c;
      
      PS p;
    public:
      void operator()(D d){
	(c->*m)(p,d);
      }
      Callback<D>* clone() const{
	return new ParamFirstClassCallback(c,p);    
      }
      ParamFirstClassCallback(C* pc,PP pp):c(pc),p(pp){
  }
};//class ParamClassCallback

    template <typename C,typename P,typename ParamMemberFunctionType<void,C,P>::value m> class ParamClassCallback<void,C,P,m>:public ClonableCallback<void>{
      C* c;
      P p;
    public:
      void operator()(){
	(c->*m)(p);
      }
      Callback<void>* clone() const{
	return new ParamClassCallback(c,p);    
      }
      ParamClassCallback(C* pc,const P &pp):c(pc),p(pp){
      }
};//class ParamClassCallback<void>




    
//adds an additional parameter stored with the callback
    template <typename D,typename C,typename P,typename P2,typename ParamMemberFunctionType<D,C,P2>::value m> class ParamConvertClassCallback:public ClonableCallback<D>{
      C* c;
      
      P p;
    public:
      void operator()(D d){
	(c->*m)(d,p);
      }
      Callback<D>* clone() const{
	return new ParamConvertClassCallback(c,p);    
      }
      ParamConvertClassCallback(C* pc,const P &pp):c(pc),p(pp){
  }
};//class ParamConvertClassCallback

    template <typename C,typename P,typename P2,typename ParamMemberFunctionType<void,C,P2>::value m> class ParamConvertClassCallback<void,C,P,P2,m>:public ClonableCallback<void>{
      C* c;
      P p;
    public:
      void operator()(){
	(c->*m)(p);
      }
      Callback<void>* clone() const{
	return new ParamConvertClassCallback(c,p);    
      }
      ParamConvertClassCallback(C* pc,const P &pp):c(pc),p(pp){
      }
};//class ParamConvertClassCallback




    template <typename D,typename C,typename D2,void (C::*m)(D2)> class CastClassCallback :public Callback<D>{
      C* c;      
      
      void operator()(D d){
	(c->*m)( (D2)d);
      }
      CastClassCallback(C* pc):c(pc){
      } 
    };//class CastClassCallback
    

    template <typename D,typename C,typename D2,void (C::*m)(D2)> class ImplicitConvertClassCallback :public Callback<D>{
      C* c;

      
      void operator()(D d){
	(c->*m)( d);
      }
      Callback<D>* clone() const{
	return new ImplicitConvertClassCallback(c);
      }
      
    public:
      ImplicitConvertClassCallback(C* pc):c(pc){
      } 
      
  
    };//class CastClassCallback



    template<typename D,typename C,typename D2, void (C::*m)(D2),int N> class PairClassCallback;


    template<typename D,typename C,typename D2, void (C::*m)(D2)>
    class  PairClassCallback<D,C,D2,m,1>:public Callback<D>
    {
      C* c;
      
      void operator()(D d){
	(c->*m)( d.first);
      }
      
    public:
     PairClassCallback(C* pc):c(pc){
     } 
      

    };


    template<typename D,typename C,typename D2, void (C::*m)(D2)> 
    class PairClassCallback<D,C,D2,m,2>:public Callback<D>{
      C* c;
      
      void operator()(D d){
	(c->*m)( d.second);
      }
      
    public:
     PairClassCallback(C* pc):c(pc){
     } 
      

    };




    template <typename D, typename C,typename D2,void (C::*m)(D2)> class CastDerefClassCallback:public Callback<D>{
  C* c;

  void operator()(D d){
    (c->*m)( *(trust_cast<D2*>(d)));
  }
 public:
  CastDerefClassCallback(C* pc):c(pc){
  } 

  Callback<D>* clone() const{
    return new CastDerefClassCallback(c);    
  }
};//class CastDerefClassCallback

  }//scb


};//helium


#endif
