/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_PIXELREADER
#define HE_PIXELREADER
#include <helium/graphics/glPrimitiveCanvas.h>
#include <helium/thread/semaphore.h>
#include <helium/signal/signal.h>
#include <helium/graphics/autoresizingBuffer.h>

namespace helium{

  class PixelReader:public Signal<void>{
    GlPrimitiveCanvas& c;
    ConnReference<void> drawnCr;
    AutoresizingBuffer buff;
    void operator()();//when drawn 
    Semaphore drawn;

    bool waitingData;
    int w,h;
  public:
    int getWidth(){return w;}
    int getHeight(){return h;}
    PixelReader(GlPrimitiveCanvas& pc,bool disableAutoRedraw);
    ~PixelReader();
    const unsigned char* read();
    const unsigned char* readAndFlip();

  };
  

}


#endif
