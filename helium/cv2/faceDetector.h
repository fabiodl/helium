/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags e
 */

#ifndef _HE_FACEDETECTOR_
#define _HE_FACEDETECTOR_

#include <opencv2/objdetect/objdetect.hpp>


namespace helium{
class FaceDetector{
  cv::CascadeClassifier cascade;

  double scale;


public:
  FaceDetector(const std::string& filename="/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml",
	       double scaleDown=1);
  //void loop();
  void detect(std::vector<cv::Rect>& faces, cv::Mat& img);
  void draw(cv::Mat& img,std::vector<cv::Rect> faces,int palettesize=1);
  void detectAndDraw(cv::Mat& img);

};

}

#endif
