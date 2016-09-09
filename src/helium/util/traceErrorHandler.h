#ifndef HE_TRACEERRORHANDLER
#define HE_TRACEERRORHANDLER

#include <helium/util/debug.h>
#include <helium/core/exception.h>

#ifdef TRACEDEBUG
namespace helium{
  class TraceExceptionHandler:public exc::ExceptionHandler{
    void handle(exc::Exception& e){
      printTrace();
    }
    
  };
}
#endif

#endif
