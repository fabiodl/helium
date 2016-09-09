/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_PPM
#define HE_PPM

#include <helium/graphics/rawImage.h>
#include <helium/graphics/glTexture.h>
namespace helium{
  void loadppm(RawImage& r,const char* filename); //automatically resizes the image
  void writeppm(const char* filename,int w,int h,const unsigned char* buff);

  
  class PpmImage:public RawImage{
  public:
    PpmImage(const char* filename){
      loadppm(*this,filename);
    }
    
    void write(const char* filename){
      writeppm(filename,width(),height(),data());
    }
    virtual ~PpmImage(){}
    
  };

  class PpmTexture:public PpmImage,public GlTexture{
  public:
    PpmTexture(const char* filename,double xscale=1,double yscale=1):PpmImage(filename),GlTexture(this,xscale,yscale){
    }
  };

};


#endif
