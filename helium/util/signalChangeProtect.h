/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef _HE_SIGNALCHANGEPROTECT_
#define _HE_SIGNALCHANGEPROTECT__

#include <helium/signal/signal.h>

namespace helium{
  template<typename C> class SignalChangeProtect{

private:
  const C c;

public:
  helium::Signal<void> changed;

  
  template<typename T> SignalChangeProtect(T t):c(t){
  }


  const C& get(){
    return c;
  }



  template<void (C::*f)()> void set(){
   (const_cast<C&>(c).*f)();
   changed();
  }

  template<typename P,void (C::*f)(P)> void set(P p){
    (const_cast<C&>(c).*f)(p);
    changed();
  }

  template<typename R,R (C::*f)()> R set(){
    R r=(const_cast<C&>(c).*f)();
    changed();
    return r;
  }

  template<typename R,typename P,R (C::*f)(P)> R set(P p){
    R r = ( const_cast<C&>(c).*f)(p);
    changed();
    return r;
  }


};
};//namespace

#endif
