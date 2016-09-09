/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/graphics/pixelReader.h>
#include <helium/graphics/glHeader.h>
#include <helium/vector/array.h>
#include <helium/vector/operations.h>
#include <helium/graphics/glUtil.h>



namespace helium{

  PixelReader::PixelReader(GlPrimitiveCanvas& pc,bool disableAutoRedraw):
    c(pc),drawnCr(this),
    buff(c),
    waitingData(false),w(c.getWidth()),h(c.getHeight()){
    if (disableAutoRedraw){
      c.setAutomaticRedrawPeriod(0);
    }
    c.drawCompleted.connect(drawnCr);
  }
  
  PixelReader::~PixelReader(){
    c.drawCompleted.disconnect(drawnCr);
  }

  void PixelReader::operator()(){
    if (waitingData){
      getPixels(buff,w,h);
      waitingData=false;
      drawn.signal();   
    }
  }
  
  const unsigned char* PixelReader::read(){
    waitingData=true;
    c.requestDraw();
    drawn.wait();
    return buff;
  }

  
  const unsigned char* PixelReader::readAndFlip(){
    unsigned char* c=const_cast< unsigned char*>(read());
    flipUD(c,w,h);
    return c;
  }

}
