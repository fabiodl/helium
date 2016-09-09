/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.2
 * Release_flags e
 */

#include <helium/thread/thread.h>
#include <helium/cv2/freerunCamera.h>
#include <helium/util/ioUtil.h>
namespace helium{


  void FreerunCamera::fillOne(){
    IplImage* iplImg = cvQueryFrame( capture );
    if (!iplImg){
      throw helium::exc::FileOpenRead("camera stream");
    }
    cv::Mat f=cv::cvarrToMat(iplImg);

    //std::cout<<"on frame "<<onFrame.listenerNum()<<std::endl;
    onFrame(f);

    buff[buffIdx].lock.lock();
    if (iplImg->origin == IPL_ORIGIN_TL ){
      //std::cout<<"filling "<<buffIdx<<std::endl;
      buff[buffIdx].frame=cv::cvarrToMat(iplImg);
    }else{
      cv::Mat tmp=cv::cvarrToMat(iplImg);
      flip(tmp, buff[buffIdx].frame, 0 );
    }
    buff[buffIdx].lock.unlock(); 
    buffIdx=1-buffIdx;
  }



  void FreerunCamera::run(){
    running=true;
    while(running){
      fillOne();
    }
  }



  FreerunCamera::FreerunCamera(int cameraID,bool autostart):buffIdx(0){
      capture = cvCaptureFromCAM(cameraID);
      
      if (!capture){
	throw helium::exc::InvalidOperation(helium::stringBegin()<<"Error opening camera "<<cameraID<<helium::stringEnd());
      }  
      fillOne();
      fillOne();
      if (autostart){
	start();
      }
    }
  

    void  FreerunCamera::getFrame(cv::Mat& m){
      int idx=1-buffIdx;
      buff[idx].lock.lock();
      //std::cout<<"getting"<<idx<<std::endl;
      buff[idx].frame.copyTo(m);
      buff[idx].lock.unlock();
    }
    
    FreerunCamera::~FreerunCamera(){
      running=false;
      join();
      cvReleaseCapture( &capture );

    }
  
  cv::Size FreerunCamera::getSize(){
    return  buff[1-buffIdx].frame.size();
  }

}
