#include <helium/cv/cvWindow.h>

namespace helium{

  int  CvWindow::getNextId(){
    static int winId=1;
    static Mutex idlock("winidlock");
    idlock.lock();
    int r=winId;
    winId++;
    idlock.unlock();
    return r;      
  }


}
