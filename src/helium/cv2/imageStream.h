#ifndef _IMGSTREAM_
#define _IMGSTREAM_

#include <opencv2/opencv.hpp>

namespace helium{
  
  class ImageStreamSource{
  public:
    TsSignal<cv::Mat&> onFrame;
    virtual void getFrame(cv::Mat& m)=0;
  };


}


#endif
