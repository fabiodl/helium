/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags 
 */


#include <opencv2/highgui/highgui.hpp>
#include <helium/cv2/window.h>

namespace helium{
  namespace cv2{

    Window::Window(const std::string& pname):name(pname){    
      cv::namedWindow(name.c_str(), CV_WINDOW_AUTOSIZE);
    }
    
    void Window::draw(cv::Mat& m){
      cv::imshow(name.c_str(),m);
    }


    Window::~Window(){
      cv::destroyWindow(name.c_str());
    }
    
  }
  



}

