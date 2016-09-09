/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_AUTORESIZINGBUFFER
#define HE_AUTORESIZINGBUFFER
#include <helium/graphics/glPrimitiveCanvas.h>

namespace helium{
  class AutoresizingBuffer:public Callback<const std::pair<int,int>&>{
    GlPrimitiveCanvas& p;
    ConnReference<const std::pair<int,int>&> resizedCr;
    
    void operator()(const std::pair<int,int>& p){
      delete[] buffer;
      buffer=new unsigned char[p.first*p.second*3];
    }

  public:
    unsigned char* buffer;
    
    AutoresizingBuffer(GlPrimitiveCanvas& pc):p(pc),resizedCr(this){
      buffer=new unsigned char[p.getWidth()*p.getHeight()*3];
      pc.resized.connect(resizedCr);
    }
    
    ~AutoresizingBuffer(){
      p.resized.disconnect(resizedCr);
      delete[] buffer;
    }
    
    int getWidth(){
      return p.getWidth();
    }

    int getHeight(){
      return p.getHeight();
    }
    
    operator unsigned char*(){
      return buffer;
    }

  };
  

}


#endif
