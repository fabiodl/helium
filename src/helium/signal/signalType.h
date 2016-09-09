/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_SIGNALTYPE
#define HE_SIGNALTYPE

#include <helium/signal/tsSignal.h>
#include <helium/signal/ehSignal.h>

namespace helium{


  template <typename T,bool TS,bool EH=false>
  struct SignalType{
  };

  template <typename T>
  struct SignalType<T,false,false>{
    typedef Signal<T> value ;
  };

  template <typename T>
  struct SignalType<T,true,false>{
    typedef TsSignal<T> value ;
  };

  template <typename T>
  struct SignalType<T,false,true>{
    typedef EhSignal<T,false> value ;
  };


  template <typename T>
  struct SignalType<T,true,true>{
    typedef EhSignal<T,true> value ;
  };

  
}

#endif
