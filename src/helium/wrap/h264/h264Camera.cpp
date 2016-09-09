#include <helium/wrap/h264/h264Camera.h>
#include <helium/core/exception.h>
#include <helium/os/time.h>
namespace helium{




  H264Camera::H264Camera(int id,cv::Size size,bool autostart):
    uvc(id),
    width(-1),
    height(-1),
    mutex("h254cam")
  {
    uvc.parent=this;   
    if (size.width!=-1){
      uvc.set_image_size(size.width,size.height);
    }
    start();
  }
  
  void H264Camera::run(){
    uvc.loop();  
  }


  void H264Camera::getFrame(cv::Mat& m){
    mutex.lock();
    cv::Mat mr(height,width,CV_8UC3,cv:: Scalar(0,0,255));
    m.data=data;
    mutex.unlock();
  }

  cv::Size H264Camera::getSize(){
    int c=0;
    while (width==-1){
      if (c++==10) throw exc::InvalidOperation("getsize failed, is the camera running?");
      milliSleep(1000);
    }
    return cv::Size(width,height);
  }
  
  
  void H264Camera::process_media(void* start, int length, int& something){
    dec.decode((char*)start,length);

    mutex.lock();
    int got=dec.get_image_RGB(data,lineSize,width,height);   
    mutex.unlock();
    if (!got) return;

    //    static 
      cv::Mat m(height,width,CV_8UC3,cv:: Scalar(0,0,255));
      //static 
      cv::Mat mr(height,width,CV_8UC3,cv:: Scalar(0,0,255));
    m.data=data;
    cvtColor(m,mr, CV_BGR2RGB);
    onFrame(mr);
  }




}
