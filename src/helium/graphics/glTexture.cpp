/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/graphics/glTexture.h>
#include <helium/graphics/glDraw_private.h>

namespace helium{
  

  GlTexture::GlTexture(RawImage* img,double pxScale,double pyScale):image(img),
								  xScale(pxScale),yScale(pyScale){    
  }


  void GlTexture:: load(){
    TextInfo& t=TextInfo::getCurrent(image);
    glGenTextures (1,&t.name);
    glBindTexture (GL_TEXTURE_2D,t.name);
    
    // set pixel unpacking mode
    glPixelStorei (GL_UNPACK_SWAP_BYTES, 0);
    glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
    
    if (t.name==0) return; //the texture should be loaded later
    gluBuild2DMipmaps (GL_TEXTURE_2D, 3, image->width(), image->height(),
		       GL_RGB, GL_UNSIGNED_BYTE, image->data());
    
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		     GL_LINEAR_MIPMAP_LINEAR);
    
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    t.state=TextInfo::LOADED;    
  }

  void GlTexture::bind (bool modulate){
    TextInfo& t=TextInfo::getCurrent(image);
    if (t.state==TextInfo::UNLOADED) load();
    if (t.state==TextInfo::UNLOADED) return;
    glBindTexture (GL_TEXTURE_2D,t.name);
    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
	       modulate ? GL_MODULATE : GL_DECAL);
  }

};
