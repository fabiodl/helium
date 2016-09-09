#include "rectFilter.h"



bool isPresent(const cv::Rect& r, const std::vector<cv::Rect>& s){
  for (std::vector<cv::Rect>::const_iterator it=s.begin();
       it!=s.end();
       ++it){
    cv::Rect inters=r & *it;
    if (inters.size().width || inters.size().height ){
      return true;
    }
  }
  return false;
}


  
  RectFilter::RectFilter(int n):m("rectSmooth"),rects(n),idx(0){};  
  void RectFilter::push(const std::vector<cv::Rect>& v){
    m.lock();
    idx=(idx+1)%rects.size();
    rects[idx]=v;
    m.unlock();
  }


  void RectFilter::getClosest(cv::Point& p,const cv::Point& src){

    m.lock();
    
    std::vector<cv::Rect> valid;

    for (int i=0;i<rects[idx].size();++i){
      bool isValid=true;
      for (int j=0;j<rects.size();j++){
	if (j==idx) continue;
	if (!isPresent(rects[idx][i],rects[j])){
	  isValid=false;
	  break;
	}
      }
      if (isValid){
	//std::cout<<" valid found"<<std::endl;
	valid.push_back(rects[idx][i]);
      }      
    }
    
    if (!valid.size()){
      //std::cout<<"none met"<<std::endl;
    }
    double mind = cv::norm(p-src);
    
    for (int i=0;i<valid.size();++i){
      cv::Point center=(rects[idx][i].tl()+rects[idx][i].br())*0.5;
      double d = cv::norm(center-src);
      if (d<mind){
	mind=d;
	p=center;
      }
    }
    m.unlock();    
  }




  std::ostream& operator<<(std::ostream& o, const RectFilter& r){
    o<<"-----{-----"<<std::endl;
    for (int i=0;i<r.rects.size();++i){
      for (int j=0;j<r.rects[i].size();++j){
	o<<r.rects[i][j]<<" ";
      }
      o<<std::endl;
    }
    o<<"-----}-----"<<std::endl;
    
    return o;
  }


