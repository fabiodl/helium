/*!
 * \brief   guide
 * \author  Fransiska Basoeki
 * \date    2015.05.22
 * \version 0.1
 * Release_flags g
 */

#ifndef HE_TRACEPRINTER
#define HE_TRACEPRINTER

#include <helium/signal/callback.h>
#include <helium/debug/trace.h>
namespace helium{

  
  template<typename T> class TracePrinter:public Callback<T>,public ConnReference<T>{
  public:
    TracePrinter():ConnReference<T>(this){
    }

    void operator()(T t){
      printTrace();
    }
  };

}

#endif
