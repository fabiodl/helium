/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_GLDRAWPRIVATE
#define HE_GLDRAWPRIVATE
#include <helium/graphics/glHeader.h>
namespace helium{

  struct TextInfo{
    enum TextureState {UNLOADED, LOADED };
    GLuint name;
    TextureState state;
    TextInfo():name(0),state(UNLOADED){}

    
    static TextInfo& getCurrent(RawImage*);
  };

  

  
};

#endif
