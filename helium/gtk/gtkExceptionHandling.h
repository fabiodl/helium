/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */


#ifndef GTKEXCEPTIONHANDLING
#define GTKEXCEPTIONHANDLING


#include <helium/core/exception.h>
#include <gtkmm.h>

namespace helium{

  class GtkExceptionHandling:public exc::ExceptionHandler{
  static exc::ExceptionHandler* defaultHandler;
  exc::ExceptionHandler* eh;
 public:


  static exc::ExceptionHandler*  getDefaultHandler(){
    return defaultHandler;
  }
  static void setDefaultExceptionHandler(exc::ExceptionHandler* e){
    defaultHandler=e;
  }

  GtkExceptionHandling():eh(getDefaultHandler()){
  }

  void setExceptionHandler(exc::ExceptionHandler* e){
    eh=e;
  }

  /*class ErrorNotification{
    TsSlList<std::string> l;
    GtkSignal<TsSlList<std::string>& > s;
    
    bool defer;


  public:

    void deferMode(bool state){
      defer=state;
    }


    void push(const std::string& s){
      l.push_back(s);
      if (!defer){
	s.signal(l);
      }
    }
        
    };*/

public:
  //Helium::GtkSignal<std::string,std::string,true> error;
  

  /*void handle(helium::exc::Exception &e){
    std::cerr<<"GuideExceptionHandler::"<<e<<std::endl;              
    error(e.what());    
    }*/

  /*void handle(){
    try{
      throw;
    }catch(helium::exc::Exception &e){
      handle(e);
    }
    }*/

  void handle(exc::Exception& e){
    eh->handle(e);
  }

  void handleUnknown(){
    try{
      throw;
    }catch(Glib::Exception& e){
      std::cerr<<"Glib exception "<<e.what()<<std::endl;
    }catch(std::exception& e){
      std::cerr<<"std::exception "<<e.what()<<std::endl;    
    }catch(...){
      std::cerr<<"Unknown exception"<<std::endl;
    }
  }

  
  template <typename C,typename R> R handle(C* c,R (C::*m)() ){
    try{
      return (c->*m)();
    }catch(helium::exc::Exception &e){
      eh->handle(e);      
    }catch(...){
      handleUnknown();
    }
  }
  
  template <typename C> void handle(C* c,void (C::*m)() ){
    try{
      (c->*m)();
    }catch(helium::exc::Exception &e){
      eh->handle(e);
    }catch(...){
      handleUnknown();
    }
  }


  template <typename C,typename P1,typename R> R handle1(P1 p1, C* c,R (C::*m)(P1) ){
    try{
      return (c->*m)(p1);
    }catch(helium::exc::Exception &e){
      eh->handle(e);
      
    }catch(...){
      handleUnknown();
    }
    return R();
  }
  
  template <typename C,typename P1> void handle1(P1 p1,C* c,void (C::*m)(P1) ){
    try{
      (c->*m)(p1);
    }catch(helium::exc::Exception & e){
      eh->handle(e);
    }catch(...){
      handleUnknown();
    }
  }

  template <typename C,typename P1,typename P2,typename R> R handle2(P1 p1, P2 p2,C* c,R (C::*m)(P1,P2) ){
    try{
      return (c->*m)(p1,p2);
    }catch(helium::exc::Exception &e){
      eh->handle(e);
    }catch(...){
      handleUnknown();
    }
    return R();
  }
  
  template <typename C,typename P1,typename P2> void handle2(P1 p1,P2 p2,C* c,void (C::*m)(P1,P2) ){
    try{
      (c->*m)(p1,p2);
    }catch(helium::exc::Exception &e){
      eh->handle(e);
    }catch(...){
      handleUnknown();
    }
  }

  template <typename C,typename P1,typename P2,typename P3,typename R> R handle3(P1 p1,P2 p2,P3 p3, C* c,R (C::*m)(P1,P2,P3) ){
    try{
      return (c->*m)(p1,p2,p3);
    }catch(helium::exc::Exception &e){

      eh->handle(e);;
    }catch(...){
      handleUnknown();
    }
    return R();
  }
  
  template <typename C,typename P1,typename P2,typename P3> void handle3(P1 p1,P2 p2,P3 p3,C* c,void (C::*m)(P1,P2,P3) ){
    try{
      (c->*m)(p1,p2,p3);
    }catch(helium::exc::Exception &e){
      eh->handle(e);;
    }catch(...){
      handleUnknown();
    }
  }

  template <typename C,typename P1,typename P2,typename P3,typename P4,typename R> R handle4(P1 p1,P2 p2,P3 p3, P4 p4, C* c,R (C::*m)(P1,P2,P3,P4) ){
    try{
      return (c->*m)(p1,p2,p3,p4);
    }catch(helium::exc::Exception &e){
      eh->handle(e);;
    }catch(...){
      handleUnknown();
    }
    return R();
  }
  
  template <typename C,typename P1,typename P2,typename P3,typename P4> void handle4(P1 p1,P2 p2,P3 p3,P4 p4,C* c,void (C::*m)(P1,P2,P3,P4) ){
    try{
      (c->*m)(p1,p2,p3,p4);
    }catch(helium::exc::Exception &e){
      eh->handle(e);;
    }catch(...){
      handleUnknown();
    }
  }

  template <typename C,typename P1,typename P2,typename P3,typename P4,typename P5,typename R> R handle5(P1 p1,P2 p2,P3 p3, P4 p4,P5 p5, C* c,R (C::*m)(P1,P2,P3,P4,P5) ){
    try{
      return (c->*m)(p1,p2,p3,p4,p5);
    }catch(helium::exc::Exception &e){
      eh->handle(e);;
    }catch(...){
      handleUnknown();
    }
    return R();
  }
  
  template <typename C,typename P1,typename P2,typename P3,typename P4,typename P5> void handle5(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,C* c,void (C::*m)(P1,P2,P3,P4,P5) ){
    try{
      (c->*m)(p1,p2,p3,p4,p5);
    }catch(helium::exc::Exception &e){
      eh->handle(e);;
    }catch(...){
      handleUnknown();
    }
  }




  /*template <typename C> void operator()(C& c,void (C::*m)() ){
    try{
      c.*m();
    }catch(helium::exc::Exception &e){
      handle();
    }
    }*/


  
  template<typename C,typename R>
    sigc::bind_functor<-1, sigc::bound_mem_functor2<R,GtkExceptionHandling,C*,R (C::*)()>,C*, R (C::*)() >	
    wrap(C& c,R (C::*m)()){
    return sigc::bind<C*,R (C::*)()>( sigc::mem_fun(*this,&GtkExceptionHandling::handle<C,R>),&c,m);
  }


  template<typename C,typename R,typename P1>
    sigc::bind_functor<-1, sigc::bound_mem_functor3<R,GtkExceptionHandling,P1,C*,R (C::*)(P1)>,P1,C*, R (C::*)(P1) >	
    wrap(C& c,R (C::*m)(P1),P1 p1){
    return sigc::bind<P1,C*,R (C::*)(P1)>( sigc::mem_fun(*this,&GtkExceptionHandling::handle1<C,P1,R>),p1,&c,m);
  }

  template<typename C,typename R,typename P1,typename P2>
    sigc::bind_functor<-1, sigc::bound_mem_functor4<R,GtkExceptionHandling,P1,P2,C*,R (C::*)(P1,P2)>,P1,P2,C*, R (C::*)(P1,P2) >	
    wrap(C& c,R (C::*m)(P1,P2),P1 p1,P2 p2){
    return sigc::bind<P1,P2,C*,R (C::*)(P1,P2)>( sigc::mem_fun(*this,&GtkExceptionHandling::handle2<C,P1,P2,R>),p1,p2,&c,m);
  }

  
  template<typename C,typename R,typename P1>
    sigc::bind_functor<-1, sigc::bound_mem_functor3<R,GtkExceptionHandling,P1,C*,R (C::*)(P1)>,C*, R (C::*)(P1) >	
    wrap(C& c,R (C::*m)(P1)){
    return sigc::bind<C*,R (C::*)(P1)>( sigc::mem_fun(*this,&GtkExceptionHandling::handle1<C,P1,R>),&c,m);
  }

  template<typename C,typename R,typename P1,typename P2>
    sigc::bind_functor<-1, sigc::bound_mem_functor4<R,GtkExceptionHandling,P1,P2,C*,R (C::*)(P1,P2)>,C*, R (C::*)(P1,P2) >	
    wrap(C& c,R (C::*m)(P1,P2)){
    return sigc::bind<C*,R (C::*)(P1,P2)>( sigc::mem_fun(*this,&GtkExceptionHandling::handle2<C,P1,P2,R>),&c,m);
  }
  
  template<typename C,typename R,typename P1,typename P2>
  sigc::bind_functor<-1, sigc::bound_mem_functor4<R,GtkExceptionHandling,P1,P2,C*,R (C::*)(P1,P2)>,P2,C*,R (C::*)(P1,P2) >	
  wrap(C& c,R (C::*m)(P1,P2),P2 p2){
    return sigc::bind<P2,C*,R (C::*)(P1,P2)>( sigc::mem_fun(*this,&GtkExceptionHandling::handle2<C,P1,P2,R>),p2,&c,m);
  }

  
  template<typename C,typename R,typename P1,typename P2,typename P3>
    sigc::bind_functor<-1, sigc::bound_mem_functor5<R,GtkExceptionHandling,P1,P2,P3,C*,R (C::*)(P1,P2,P3)>,C*, R (C::*)(P1,P2,P3) >	
    wrap(C& c,R (C::*m)(P1,P2,P3)){
    return sigc::bind<C*,R (C::*)(P1,P2,P3)>( sigc::mem_fun(*this,&GtkExceptionHandling::handle3<C,P1,P2,P3,R>),&c,m);
  }

  template<typename C,typename R,typename P1,typename P2,typename P3,typename P4>
    sigc::bind_functor<-1, sigc::bound_mem_functor6<R,GtkExceptionHandling,P1,P2,P3,P4,C*,R (C::*)(P1,P2,P3,P4)>,C*, R (C::*)(P1,P2,P3,P4) >	
    wrap(C& c,R (C::*m)(P1,P2,P3,P4)){
    return sigc::bind<C*,R (C::*)(P1,P2,P3,P4)>( sigc::mem_fun(*this,&GtkExceptionHandling::handle4<C,P1,P2,P3,P4,R>),&c,m);
  }

  template<typename C,typename R,typename P1,typename P2,typename P3,typename P4,typename P5>
    sigc::bind_functor<-1, sigc::bound_mem_functor7<R,GtkExceptionHandling,P1,P2,P3,P4,P5,C*,R (C::*)(P1,P2,P3,P4,P5)>,C*, R (C::*)(P1,P2,P3,P4,P5) >	
    wrap(C& c,R (C::*m)(P1,P2,P3,P4,P5)){
    return sigc::bind<C*,R (C::*)(P1,P2,P3,P4,P5)>( sigc::mem_fun(*this,&GtkExceptionHandling::handle5<C,P1,P2,P3,P4,P5,R>),&c,m);
  }

  /*template<typename C,typename R,typename P1,typename P2,typename P3,typename P4>
    //sigc::mem_functor2<C*,R (C::*)() ,R,GtkExceptionHandling> 
    sigc::bind_functor<-1, sigc::bound_mem_functor2<R,GtkExceptionHandling,C*,R (C::*)(P1,P2,P3,P4)>,C*, R (C::*)(P1,P2,P3,P4) >	
    wrap(C& c,R (C::*m)(P1,P2,P3,P4)){
    return sigc::bind<C*,R (C::*)(P1,P2,P3,P4)>( sigc::mem_fun(*this,&GtkExceptionHandling::handle<C,R>),&c,m);
    }*/




};
}

#endif
