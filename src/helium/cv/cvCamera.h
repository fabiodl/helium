#ifndef HE_CVCAMERA_
#define HE_CVCAMERA_
#include <string>
#include <memory>
#include <helium/thread/thread.h>
#include <helium/cv/cvMain.h>
#include <helium/cv/cvWindow.h>


namespace helium{






class CvCamera:public Thread{
private:
  int id;
  int waitKey;
  bool running;
  

protected:
  CvCapture* capture;
  IplImage* frame;
  std::auto_ptr<CvWindow> win;
   

public:  

  CvCamera(int id,bool show=true,std::string windowname="");
  
  ~CvCamera();

  int getWidth() const{
    return frame->width;
  }
  int getHeight() const{
    return frame->height;
  }
  void setWaitKey(int w){
    waitKey=w;
  }
  
  IplImage* getEmptyImage(){
    if (frame==NULL) return NULL;
    return cvCreateImage( cvSize(frame->width,frame->height),
			  IPL_DEPTH_8U, frame->nChannels );
  }
  
  
  void run();  
  inline unsigned char &getPixel(int i,int j,CV::Channel c){
    return ((uchar *)(frame->imageData + i*frame->widthStep))[j*frame->nChannels + c];
  }
  
  inline void setPixel(int i,int j,CV::Channel c,int v){
    ((uchar *)(frame->imageData + i*frame->widthStep))[j*frame->nChannels + c]=v;
  }
  
  //Signal<void> onInitCompleted;
  Signal< const IplImage*> onFrame;
};

  

  namespace exc{
    class CvException:public Exception{
    public:
      CvException(const std::string& s):Exception("OpenCV"+addDetail(s)){
      }
      
      virtual void rethrow(){
	throw *this;
      }

      Exception* clone() const{
	return new CvException(*this);
      }
    };


  }//exc


}//helium


#endif
