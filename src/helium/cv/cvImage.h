#ifndef HE_CVIMAGE_
#define HE_CVIMAGE_
#include <string>
#include <helium/thread/thread.h>
#include "cvMain.h"

namespace helium{


class CvImage{
  public:
    IplImage* data;
    CvImage(int width,int height,int channels=3){
      data=cvCreateImage( cvSize(width,height),
		     IPL_DEPTH_8U, channels );
      //std::cout<<"created "<<data<<" a "<<width<<"x"<<height<<std::endl;
    }
    
    ~CvImage(){
      cvReleaseImage(&data);
      //std::cout<<"destroyed "<<data<<std::endl;
    }

   operator IplImage*(){
     //std::cout<<"returning "<<data<<std::endl;
     return data;
    
  }

  };




}//helium


#endif
