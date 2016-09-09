#ifndef _GTKVOLATILE_
#define _GTKVOLATILE_

namespace helium{
#include <system/volatile.h>

template <typename T>
  class GtkVolatile:public Volatile<T>{
  Glib::Dispatcher disp;
    Signal<DataP> dispSignal;
    DataP d;
    void onSignal(DataP p){
      d=p;
      disp();   
    }
    void onDisp(){
      dispSignal();
    }
  public:
    
  Volatile():gtkConnections(0){            
      link->updateSignal.connect(ClassCallback<DataP,GtkVolative<T> >(this,onSignal));
      disp.connect(sigc::mem_fun(*this,&onDisp));
    }
    
    template<typename C> inline Signal<DataP>::Reference gtkConnect(C* c,
								 void (C::*m)(T),
								 Time t){      
      }      
      return dispSignal.connect(CastDerefClassCallback<DataP,C,T>(c,m),t);
    }
    

  inline void gtkDisconnect(const ConnectAwareSignal<DataP,Time>::Reference &r){
    dispSignal.disconnect(r);
  }

  const T& operator=(const T &t){
    link->set((T&)t);
    return t;
  }

};



};


#endif
