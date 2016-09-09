#ifndef _HE_PIP_
#define _HE_PIP_


#include <helium/thread/mutex.h>
#include <helium/signal/tsSignal.h>
#include <opencv2/opencv.hpp>

namespace helium{
  

  class Pip{  

  public:
    cv::Rect roi;

    
    void setMain(const cv::Mat& m){
      mutex.lock();      
      whole=m.clone();
      update();
      mutex.unlock();
    }


    void setSub(const cv::Mat& m){
      mutex.lock();
      sub=m.clone();
      update();
      mutex.unlock();
    }

    TsSignal<cv::Mat&> onFrame;
  
    Pip():mutex("pipmutex"){}

  private:
    Mutex mutex;
    cv::Mat whole;
    cv::Mat sub,ressub;
    
    void update(){
      if (whole.size().width<=0||sub.size().width<=0) return;
      //std::cout<<whole.size()<<" including "<<sub.size()<<std::endl;

      cv::Mat dst_roi = whole(roi);
      resize(sub,ressub,cv::Size(roi.width,roi.height));
      ressub.copyTo(dst_roi);
      onFrame(whole);
    }

  };//Pip
  


}//ns helium



#endif
