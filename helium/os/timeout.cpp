#include <helium/os/timeout.h>
#include <cstring>
namespace helium{


  Timeout::Timeout(int pfd,Time t):fd(pfd){
    timeout.tv_sec=t/1000;
    timeout.tv_usec=(t%1000)*1000;
    FD_ZERO(&set); /* clear the set */
    FD_SET(fd, &set); /* add our file descriptor to the set */
  }


  
  Timeout::State  Timeout::getState(){
    switch(select(fd + 1, &set, NULL, NULL, &timeout)){
    case 0:
      return TIMEOUT;
    case -1:
      return ERROR;
    default:
      return DATAAVAILABLE;
    }
  }




}
