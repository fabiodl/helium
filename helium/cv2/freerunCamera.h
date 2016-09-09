/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.2
 * Release_flags e
 */

#ifndef HE_FREERUNCAMERA
#define HE_FREERUNCAMERA

#include <helium/thread/thread.h>
#include <opencv2/opencv.hpp>
#include <helium/signal/signal.h>
#include <helium/cv2/imageStream.h>

namespace helium{



  class FreerunCamera:public helium::Thread,public ImageStreamSource{

    struct MTframe{
      cv::Mat frame;
      helium::Mutex lock;
      MTframe():lock("mtframe"){}
      
    };
    
    int buffIdx;
    MTframe buff[2];
    volatile bool running;      
    CvCapture* capture ;       
    void fillOne();
    void run();
public:
    FreerunCamera(int cameraID,bool autostart=true);
    void getFrame(cv::Mat& m);
    ~FreerunCamera();    
    TsSignal<cv::Mat&> onFrame;
    cv::Size getSize();
  };


  

}



\
#endif
