#ifndef HE_CONNECTABLESIGNAL
#define HE_CONNECTABLESIGNAL

#include <helium/signal/ehSignal.h>

namespace helium{

template<typename D,bool TS> class ConnectableEhSignal:public EhSignal<D,TS>,public EhConnReference<D>{
  
  ConnectableEhSignal():EhConnReference<D>(this){
  }

  void handle(exc::Exception& e){
    EhSignal<D,TS>::handle(e);
  }

  void operator()(D d){
    EhSignal<D>::operator()(d);
  }

};
}

#endif
