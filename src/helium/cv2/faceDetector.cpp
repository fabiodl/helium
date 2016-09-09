/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags e
 */

#include <helium/core/exception.h>
#include <helium/cv2/faceDetector.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


namespace helium{
  FaceDetector::FaceDetector(const std::string& filename,double scaleDown):
    scale(scaleDown)
  {
   
    if (!cascade.load(filename)){
      throw helium::exc::FileOpenRead(filename);
    }
    
  }
  
  void FaceDetector::detectAndDraw(cv::Mat& img){
    std::vector<cv::Rect> faces;
    detect(faces,img);
    draw(img,faces);
    cv::imshow( "result", img );
  }
  




void FaceDetector::draw(cv::Mat& img,std::vector<cv::Rect> faces,int palettesize){  
  const static cv::Scalar colors[] =  { CV_RGB(0,0,255),
				    CV_RGB(0,128,255),
				    CV_RGB(0,255,255),
				    CV_RGB(0,255,0),
				    CV_RGB(255,128,0),
				    CV_RGB(255,255,0),
				    CV_RGB(255,0,0),
				    CV_RGB(255,0,255)} ;
  int i=0;
  for( std::vector<cv::Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++ )
    {
      cv::Point center;
      cv::Scalar color = colors[i%palettesize];     
      rectangle( img, cvPoint(cvRound(r->x*scale), cvRound(r->y*scale)),
		 cvPoint(cvRound((r->x + r->width-1)*scale), cvRound((r->y + r->height-1)*scale)),
		 color, 3, 8, 0);

    }
  
}


void FaceDetector::detect( std::vector<cv::Rect>& faces, cv::Mat& img)
{

  bool tryflip=true;

    std::vector<cv::Rect>  faces2;
    
    cv::Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );

    cv::cvtColor( img, gray, CV_BGR2GRAY );
    cv::resize( gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR );
    cv::equalizeHist( smallImg, smallImg );

    cascade.detectMultiScale( smallImg, faces,
        1.1, 2, 0
			      //|CV_HAAR_FIND_BIGGEST_OBJECT
			      //|CV_HAAR_DO_ROUGH_SEARCH
			      |CV_HAAR_SCALE_IMAGE
        ,
			      cv::Size(30, 30) );
    if( tryflip )
    {
      flip(smallImg, smallImg, 1);
        cascade.detectMultiScale( smallImg, faces2,
				  1.1, 2, 0
				  //|CV_HAAR_FIND_BIGGEST_OBJECT
				  //|CV_HAAR_DO_ROUGH_SEARCH
				  |CV_HAAR_SCALE_IMAGE
				  ,
				  cv::Size(30, 30) );
        for( std::vector<cv::Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++ )
        {
	  faces.push_back(cv::Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
        }
    }



}


}//ns helium
