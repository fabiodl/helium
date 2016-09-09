#ifndef HE_COLOR
#define HE_COLOR

namespace helium{

  void hsl2rgb(int* rgb,double h,double s=1,double l=0.5);
  void yuv2rgb(int* rgb,double y,double u,double v);
  void yCbCr2rgb(int* rgb,double y,double cb,double cr);
}

#endif
