#ifndef HE_CVMAIN_
#define HE_CVMAIN_
#include <opencv/cv.h>
#include <opencv/highgui.h>
namespace helium{
  namespace CV{
    enum Channel{B,G,R,First=0};
  };
  
  inline unsigned char &getPixelChannel(IplImage* img,int i,int j,CV::Channel c){
    return ((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + c];
  }
  
  inline const unsigned char &getPixelChannel(const IplImage* img,int i,int j,CV::Channel c){
    return ((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + c];
  }

  inline void getPixel(unsigned char &r,unsigned char &g,unsigned char &b,const IplImage* img,int i,int j){
    uchar * p=&(((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels]);
    r=p[CV::R];  
    g=p[CV::G];
    b=p[CV::B];
  }
  
  
  inline void setPixel(IplImage* img,int i,int j,unsigned char r,unsigned char g,unsigned char b){
    uchar * p=&(((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels]);
    p[CV::R]=r;  
    p[CV::G]=g;
  p[CV::B]=b;
  }
  
  
  inline void setPixelChannel(IplImage* img,int i,int j,CV::Channel c,int v){
    ((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + c]=v;
  }
}//helium

#endif
