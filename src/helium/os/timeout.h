#ifndef HE_TIMEOUT
#define HE_TIMEOUT

#include <sys/select.h>
#include <helium/core/time.h>
//see timer.h for more high level functions

namespace helium{

  //moved above, causes error on mac compile #include <sys/select.h>

class Timeout{
  fd_set set;
  struct timeval timeout;
  int fd;
public:
  Timeout(int fd,Time t);
  
  enum State{ERROR=-1,DATAAVAILABLE=1,TIMEOUT=-2};
  
  State getState();

};



}

#endif
