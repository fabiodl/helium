#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "drawingCanvas.h"
#include <iostream>
#include <helium/vector/array.h>
#include <helium/vector/operations.h>
namespace helium{




  


  DrawingCanvas::DrawingCanvas(int w, int h):
    buffer(NULL),width(0),height(0)
    {
    
#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
    /* specify Z, stencil, accum sizes */
      ctx = OSMesaCreateContextExt( OSMESA_RGBA,16, 0, 0, NULL );//
#else
    ctx = OSMesaCreateContext( OSMESA_RGBA, NULL );
#endif
    if (!ctx) {
      printf("OSMesaCreateContext failed!\n");
    }

    /* Allocate the image buffer */
    resize(w,h);
    select();
  }
  
  void DrawingCanvas::resize(int w,int h){
    if (w==width&&h==height) return;
    width=w;
    height=h;
    if (buffer){
      delete buffer;
    }
    buffer=new unsigned char[width*height*4];
  }

  
  
  void DrawingCanvas::select(void)
  {
    /* Bind the buffer to the context and make it current */
    //std::cout << "dc::select " << width << " " << height << "buffer "<<(void*)buffer<<std::endl;
    if (!OSMesaMakeCurrent( ctx, buffer, GL_UNSIGNED_BYTE, width, height )){
      printf("OSMesaMakeCurrent failed!\n");
    }
}
  
  void DrawingCanvas::readPixels(unsigned char* pixels)
  {
    //std::cout << "read pixels " << width << " " << height << "buffer "<<(void*)buffer<<std::endl;
    //helium::Array<unsigned char*> pix;
    //pix.resize(width * height * 4);
    //helium::zero(pix);
    //memcpy(pixels, pix.data, width * height * 4 * sizeof(GLubyte));
    /*for(int i=0;i<width * height * 4;i++) {
      if(buffer[i] != 0) std::cout << "non0 at " << i<< std::endl;
      }*/
    memcpy(pixels, buffer, width * height * 4 * sizeof(GLubyte));
  }

  DrawingCanvas::~DrawingCanvas()
  {
    if (buffer!=NULL){
      delete[] buffer;
    }
   /* destroy the context */
    OSMesaDestroyContext( ctx );       
  }




  ExternalDrawingCanvas::ExternalDrawingCanvas(int w, int h,unsigned char *buff):
    buffer(buff),width(w),height(h)
    {
    
#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
    /* specify Z, stencil, accum sizes */
    ctx = OSMesaCreateContextExt( OSMESA_RGBA, 16, 0, 0, NULL );
#else
    ctx = OSMesaCreateContext( OSMESA_RGBA, NULL );
#endif
    
    /* Allocate the image buffer */
    select();
  }
  
  void ExternalDrawingCanvas::resize(int w,int h,unsigned char *buff){
    width=w;
    height=h;
    buffer=buff;
  }

  
  
  void ExternalDrawingCanvas::select(void)
  {
    /* Bind the buffer to the context and make it current */
    OSMesaMakeCurrent( ctx, buffer, GL_UNSIGNED_BYTE, width, height ); 
}
  
 
  ExternalDrawingCanvas::~ExternalDrawingCanvas()
  {

   /* destroy the context */
    OSMesaDestroyContext( ctx );       
  }



}//helium
