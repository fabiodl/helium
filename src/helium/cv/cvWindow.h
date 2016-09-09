#ifndef HE_CVWINDOW
#define HE_CVWINDOW

#include <helium/util/ioUtil.h>
#include <helium/cv/cvMain.h>
#include <helium/cv/cvImage.h>

namespace helium{

  

  class CvWindow:public Callback<const IplImage*>{
  protected:
    const std::string windowname;
    static int getNextId();

  public:
    CvWindow(const std::string& pwindowname=""):
      windowname(pwindowname==""?"OpenCV Win "+toString(getNextId()):pwindowname){
      cvNamedWindow(windowname.c_str(), CV_WINDOW_AUTOSIZE );
    }
  
    void operator()(const IplImage* frame){
      //std::cout<<"showing"<<std::endl;
      cvShowImage(windowname.c_str(), frame);
    }

    ~CvWindow(){
      cvDestroyWindow(windowname.c_str());
    }
    
  };

  class CvCanvas:public CvImage,public CvWindow{
    
  public:
    CvCanvas(int width,int height,const std::string& windowname=""):
      CvImage(width,height),
      CvWindow(windowname){
    }
    

    void show(){
      cvShowImage(windowname.c_str(), data);
    }
  };
  


}

#endif
