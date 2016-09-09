#ifndef _HE_H26CAMERA_
#define _HE_H26CAMERA_


#include <h264/H264Decoder.hpp>
#include <h264/UVC_H264.hpp>
#include <helium/thread/thread.h>
#include <helium/thread/mutex.h>
#include <helium/signal/tsSignal.h>
#include <opencv2/opencv.hpp>
#include <helium/cv2/imageStream.h>

namespace helium{
class H264Camera:
    public MediaProcessor,
    public helium::Thread,
    public ImageStreamSource
{
  Geminoid::UVC_H264 uvc;
  Geminoid::H264Decoder dec;



  
public:
  H264Camera(int id,cv::Size size,bool autostart=true);
  
  cv::Size getSize();
  void getFrame(cv::Mat& m);

 private:
  void run();
  void process_media(void* start, int length, int& something);
  int width,height,lineSize;
  Mutex mutex;
  uint8_t* data;
};


}//ns helium

#endif
