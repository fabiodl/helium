#ifndef HE_IMAGEPIPE
#define HE_IMAGEPIPE

#include <helium/container/bufferPipe.h>
#include <helium/cv/cvMain.h>
#include <helium/os/time.h>

namespace helium{
  template<typename T>
  class  ParamNode<IplImage*,T>{
    IplImage* im;
  public: 
    ParamNode* next;
    ParamNode(int width,int height):next(NULL){
      im=cvCreateImage( cvSize(width,height),
			IPL_DEPTH_8U, 3 );
    }
    ~ParamNode(){
      cvReleaseImage(&im);
    }
    
    IplImage* operator*(){
      return im;
    }
    
  };
  
  
  template<typename T> struct NodeFactory<IplImage*,const T*,const T*, const T*>{
    const T* c;
    NodeFactory(const T* pc):c(pc){}
    
    ParamNode<IplImage*,const T*> *getNew(){
      return new ParamNode<IplImage*,const T*>(c->getWidth(),c->getHeight());
    }
    
  };
  
  
  template<typename SizeRetriever>
  class ImageProcessor:public Thread,public Callback<const IplImage*>{
    BufferPipe<IplImage*,const SizeRetriever*,const SizeRetriever*, const SizeRetriever*> buffer;
    bool running;

  
    void run(){
      running=true;
      while(running){
	//Time zero=getSystemMillis();
	ParamNode<IplImage*,const SizeRetriever*>* n=buffer.getLastReaderNode();
	//Time ret=getSystemMillis();
	//std::cout<<"waited image for "<<ret-zero<<std::endl;
	onFrame(**n);
	//Time proc=getSystemMillis();
	//std::cout<<"processed for "<<proc-ret<<std::endl;	
	buffer.remitReaderNode(n);
      }
    }

  public:
    void operator()(const IplImage* i){
      ParamNode<IplImage*,const SizeRetriever*>* n=buffer.getWriterNode();
      IplImage* im=**n;
      //Time zero=getSystemMillis();
      cvCopy(i,im);
      //Time cp=getSystemMillis();
      //std::cout<<"copied in "<<cp-zero<<std::endl;
      buffer.remitWriterNode(n);      
    }

    ImageProcessor(const SizeRetriever* s):buffer(s),running(false){}


    ~ImageProcessor(){
      running=false;
      join();
    }

    Signal<const IplImage*> onFrame;

  };


}
#endif
