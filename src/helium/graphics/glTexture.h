/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_GLTEXTURE
#define HE_GLTEXTURE


#include <helium/graphics/rawImage.h>

namespace helium{


  class GlTexture{
  public:    

  private:
    RawImage* image;   
    void load();
    double xScale,yScale;
  public:
    double getXscale(){return xScale;}
    double getYscale(){return yScale;}
    GlTexture(RawImage* img,double xScale=1,double yScale=1);
    void bind (bool modulate=true); //modulate means mix with the current color
    virtual ~GlTexture(){}
  };


};



#endif
