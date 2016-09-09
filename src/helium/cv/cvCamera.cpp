#include <helium/cv/cvCamera.h>
#include <helium/util/ioUtil.h>
#include <iostream>
#include <sstream>
using namespace std;
using namespace helium;



CvCamera::CvCamera(int pid,bool show,string windowname):
  id(pid),waitKey(10),
  running(true),
  frame(NULL){

  capture = cvCaptureFromCAM( id );
  if( !capture ) {
    throw exc::CvException("No capture "+toString(id));		      
  }

  frame = cvQueryFrame( capture );  
  if( !frame ) {
    throw exc::CvException("Unable to acquire frame from camera"+toString(pid));
  }
  std::cout<<"Camera "<<id<<" inited"<<std::endl;


  if (show){
    win.reset(new CvWindow(windowname==""?("Camera "+toString(pid)):windowname));
    onFrame.connect(win.get());
  }

 
}

CvCamera::~CvCamera(){
  running=false;
  join();
  cvReleaseCapture( &capture );
  //cvReleaseImage(&image); do not touch the frame returned by queryframe!
}


void CvCamera::run(){


  // Create a window in which the captured images will be presented
  
  // Show the image captured from the camera in the window and repeat
  while( running ) {
  
    // Get one frame
    frame = cvQueryFrame( capture );
    if( !frame ) {
      throw exc::CvException("Unable to acquire frame");
    }
    
    // Do not release the frame!

    onFrame(frame);    
    //if( (cvWaitKey(waitKey) & 255) == 27 ) break;
  }

  // Release the capture device housekeeping

 
}//run


