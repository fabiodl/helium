/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags s
 */


#include <helium/os/inputEvent.h>
#include <helium/core/exception.h>

/*code derived from input-utils 0.0.20051128-4 source package in Ubuntu*/
#include <asm/types.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>

#include <iostream>
#include <linux/input.h>
#include <unistd.h>
/*struct input_id {
	__u16 bustype;
	__u16 vendor;
	__u16 product;
	__u16 version;
};


#define EVIOCGID		_IOR('E', 0x02, struct input_id)	// get device ID
#define EVIOCGVERSION		_IOR('E', 0x01, int)			// get driver version 
#define EV_VERSION		0x010001
*/

static int device_open(const char* filename,int verbose)
{

  int fd, version;
  
  fd = open(filename,O_RDONLY);
  if (-1 == fd) {
    fprintf(stderr,"open %s: %s\n",
	    filename,strerror(errno));
    return -1;
  }
  if (verbose)
    fprintf(stderr,"%s\n",filename);
  
  if (-1 == ioctl(fd,EVIOCGVERSION,&version)) {
    perror("ioctl EVIOCGVERSION");
    close(fd);
    return -1;
  }
  /*if (EV_VERSION != version) {
    fprintf(stderr, "protocol version mismatch (expected %d, got %d)\n",
	    EV_VERSION, version);
    close(fd);
    return -1;
    }*/
  return fd;
}


  
namespace helium{

  

  namespace exc{

    
  }



  std::string  getEventFilename(int vID,int pID,const std::string& name){    

    struct stat statbuf;
    char filename[32];
    for (int i=0;i<32;i++){
      snprintf(filename,sizeof(filename),"/dev/input/event%d",i);
      if (stat(filename, &statbuf) == 0) {
	//std::cout<<"checking "<<filename<<std::endl;
	int fd=device_open(filename,false);	
	if (fd==-1){
	  close(fd);
	  continue;
	}
	struct input_id id;
	ioctl(fd,EVIOCGID,&id);
	//std::cout<<"found "<<std::hex<<id.vendor<<"-"<<std::hex<<id.product<<std::endl;
	close(fd);
	if (id.vendor==vID&&id.product==pID){	  
	  std::cout<<"returning "<<filename<<std::endl;
	  return filename;
	}
      }     
    }
    throw helium::exc::DeviceNotFoundException(vID,pID,name);
  }


}
