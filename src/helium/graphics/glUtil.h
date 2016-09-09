/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_GLUTIL_
#define HE_GLUTIL_

#include <helium/graphics/glPrimitiveCanvas.h>
#include <helium/math/algebra.h>
namespace helium{

  typedef double Rectangle[8];
  
  void clearWindow(float r,float g,float b);
  void getLookAt(double* viewMatrix,const Vector3d &target,const Vector3d& pos,const Vector3d& up=Vector3d(0,0,1));
  void setCamera(const double* viewMatrix);
  void setPerspective(double width,double height,bool skipinit=false,float aspect=0.8,float zoom=1.0f,float near=0.01f,float far=100.0f);
  void initGL(int width,int height);
  void getPixels(unsigned char* dst,int w,int h);
  void flipUD(unsigned char* dst,int w,int h);


  void drawBox(double *vertex);
  void drawBox(double sx,double sy,double sz,double* rotoTra);
  void setLight();
  double getPointZ(int x,int y);
  void getSpacePos(double * pos,int x, int y,double z);
  
 
  void setOrthogonal (int w,int h);
  void drawString(const char *str,int x, int y, const double *color=NULL, double alpha=1,void *font=NULL);
  void drawRectangle(double l,double t,double w,double h,const double* color);
  void drawBorder(double l,double t,double w,double h,double bw,double bh,double* color);
  
  void getRectangle(double* v,const GlPrimitiveCanvas* pc,double x,double y,double w,double h,bool centered);//returns 8 doubles in v
  void enlargeSquare(double* v,double dw,double dh);
  
  void drawFilledRectangle(const double* v,const double* fillcolor);
  
  extern Rectangle fullRectangle;
  void drawTexturedRectangle(const double* v,const double* texCoor=fullRectangle);
  void drawCenterGradientRectangle(const double* v,const double* c1,const double* c2,const double* cb);
  void drawDiagonalGradientRectangle(const double* v,const double* c1,const double* c2,const double* cb);
  void drawArrow(double x,double y,double scale,int direction);
  void drawCircle(double x,double y,double r,const double* centerColor,const double* outColor);
  void drawPie(double x,double y,double r,double start,double stop,const double* centerColor,const double* outColor);


}
#endif
