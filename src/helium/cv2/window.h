#ifndef HE_CV2WINDOW
#define HE_CV2WINDOW


#include <string>
#include <opencv2/core/core.hpp>

namespace helium{
  
  namespace cv2{
    
    class Window{
      std::string name;      
    public:  
      Window(const std::string& pname);      
      ~Window();
      void draw(cv::Mat& m);
      
    };//window
    
  }

}

#endif
