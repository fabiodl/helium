/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SELECTBYVALUE
#define HE_SELECTBYVALUE


namespace helium{

  
  template <typename T1,typename T2,bool b> struct selectFirstOnTrue{
  };

  template <typename T1,typename T2> struct selectFirstOnTrue<T1,T2,true>{
    typedef T1 value;
  };
  template <typename T1,typename T2> struct selectFirstOnTrue<T1,T2,false>{
    typedef T2 value;
  };


  template<typename T,T v1,typename T1,T v2,typename T2,T v> struct selectTypeOnValue{
  };

  template<typename T,T v1,typename T1,T v2,typename T2> struct selectTypeOnValue<T,v1,T1,v2,T2,v1>{
    typedef T1 value;
    inline static void assign(T1& d,T1& s,T2&){
      d=s;
    }
  };

  template<typename T,T v1,typename T1,T v2,typename T2> struct selectTypeOnValue<T,v1,T1,v2,T2,v2>{
    typedef T2 value;
    inline static void assign(T2& d,T1& ,T2&s){
      d=s;
    }
  };
  
  template<bool b> struct conditionalOper{
    template <typename D,typename S> static  void assign(D& d,const S& s){}
    template<typename C,typename P,void (C::*m)(P& p)> static void invokeSelf(C* c){}	  
    template <typename Sig,typename D> static void callSig(Sig& s,D& d){}
    template <typename Sig,typename D,typename C> static void callSig(Sig& s,D& d,C* noExec){}
    template <typename D,typename S> static void get(D& d,S& s){}
    template <typename C,typename P,void (C::*m)(P)> static void invoke(C*,P){}
  };


  template<> struct conditionalOper<true>{
    template <typename D,typename S> static void assign(D& d,const S& s){d=s;}
    template <typename Sig,typename D> static void callSig(Sig& s,D& d){s(d);}
    template<typename C,typename P,void (C::*m)(P& p)> static void invokeSelf(C* c){(c->*m)(*c);}	      
    template <typename Sig,typename D,typename C> static void callSig(Sig& s,D& d,C* noExec){s(d,noExec);}
    template <typename D,typename S> static void get(D& d,S& s){s.get(d);}
    template <typename C,typename P,void (C::*m)(P)> static void invoke(C* c,P p){
      (c->*m)(p);
    }
  };

}


#endif
