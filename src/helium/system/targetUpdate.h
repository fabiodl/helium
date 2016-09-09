#ifndef _TARGETUPDATE_
#define _TARGETUPDATE_

#include <opencv2/opencv.hpp>

class TargetUpdate{
public:
  int id;
  cv::Point2d abst;
  
  TargetUpdate():id(-1),abst(0,0){}
  TargetUpdate(int _id,const cv::Point2d& _abst):id(_id),abst(_abst){}
  
};



#endif
