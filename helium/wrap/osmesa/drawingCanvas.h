#ifndef HE_DRAWINGCANVAS
#define HE_DRAWINGCANVAS

#include <helium/graphics/glHeader.h>
#include <GL/osmesa.h>

namespace helium{

class DrawingCanvas{

private: 
    unsigned char *buffer; 
    OSMesaContext ctx;
    int width,height;
public:

  DrawingCanvas(int width, int height);

  void resize(int width,int height); 
  
  void select();

  void readPixels(unsigned char* pixels);
  virtual ~DrawingCanvas();
};

class ExternalDrawingCanvas{

private: 
    unsigned char *buffer; 
    OSMesaContext ctx;
    int width,height;
public:

  ExternalDrawingCanvas(int width, int height,unsigned char *buff);//with alpha

  void resize(int width,int height,unsigned char *buff);   
  void select();
  
  virtual ~ExternalDrawingCanvas();
};



}

#endif
