/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_GLDRAW
#define HE_GLDRAW

#include <helium/core/exception.h>
#include <helium/thread/semaphore.h>
#include <helium/signal/signal.h>
#include <helium/graphics/glPrimitiveCanvas.h>
namespace helium{

  class GlDraw{
  public:
    class Stopper:public Callback<void>{
      ConnReference<void> cr;
      bool alreadyDead;
      Semaphore s;
      Mutex state;
      //friend class GlDraw;
      void operator()();      
    public:
      Stopper();
      ~Stopper();
      void stop();//automatically joins
      void join();
    };
    static void join();
    static void push_back(GlPrimitiveCanvas*);
    static void start(int& argc,char** argv);//as another thread
    static void execute(int& argc,char** argv); //in the current thread
    static void setExceptionHandler(exc:: ExceptionHandler* eh);        

  };

};


#endif
