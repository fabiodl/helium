#ifndef HE_CVUTIL
#define HE_CVUTIL
#include <helium/cv/cvMain.h>
namespace helium{
  inline void drawCross(IplImage* img,int x,int y,int l,const double* color){
    cvLine(img, cvPoint(x-l,y), cvPoint(x+l,y), cvScalar(color[0],color[1],color[2]));
    cvLine(img, cvPoint(x,y-l), cvPoint(x,y+l), cvScalar(color[0],color[1],color[2]));
  }
  extern double rgb2yuv[9];
 

}


#endif
