/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef _DRAWCONVEX_
#define _DRAWCONVEX_

#include <helium/graphics/glPrimitiveCanvas.h>
#include <helium/graphics/basicKeybInter.h>
#include "convexModel.h"
namespace helium{
namespace convex{

  enum Faces {FRONT,RIGHT,BACK,LEFT,TOP,BOTTOM,ISO};

  class DrawConvex:public GlPrimitiveCanvas{
    const ConvexCollisionModel* cm;
  protected:
    double viewMatrix[16];
    double worldMatrix[16];
    bool wireframe;
    BasicKeybInter bki;

    double bg[3];
    double aspect;
    double zoom;
  public: 
    void setAspect(double d){aspect=d;}
    void setZoom(double z){zoom = z;}
    void setBG(const double* b){memcpy(bg,b,sizeof(double)*3);}
    void setWorldMatrix(const double* d){transpose(worldMatrix,d);}
    void setViewMatrix(const double* m){memcpy(viewMatrix,m,sizeof(double)*16);}
    double* getViewMatrix(){return viewMatrix;}
    DrawConvex(const ConvexCollisionModel* cm);
    void fastReplaceModel(const ConvexCollisionModel* cm);
    helium::PrimitiveKeyboardInteraction::KeyParsingState onKeyPushed(unsigned char key,int x,int y);
    void drawConvex();
    virtual void onResize(int,int){}

    void getKeyViewMatrix(double &x,double &y,double &z) { x=viewMatrix[3]; y=viewMatrix[7]; z=viewMatrix[11];}
    void setKeyViewMatrix(double x,double y,double z) { viewMatrix[3]=x; viewMatrix[7]=y; viewMatrix[11]=z; }
    void face(helium::convex::Faces f); ///< change robot's facing direction
    const ConvexCollisionModel* getCCM() { return cm; }
  protected:
    virtual void onDraw();
  };
}
}
#endif
