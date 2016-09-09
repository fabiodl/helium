/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags e
 */


#ifndef _HE_CV2UTIL_
#define _HE_CV2UTIL_
#include <opencv2/highgui/highgui.hpp>

namespace helium{
  inline cv::Size getFrameSize(cv::VideoCapture& c){
  
    return cv::Size
      (
       static_cast<int>(c.get(CV_CAP_PROP_FRAME_WIDTH)),
       static_cast<int>(c.get(CV_CAP_PROP_FRAME_HEIGHT))
       );
    
}
  
}

#endif
