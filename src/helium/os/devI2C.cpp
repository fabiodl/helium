#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>


#include <helium/os/devI2C.h>
#include <helium/os/i2c.h>
#include <helium/util/ioUtil.h>
#include <helium/os/time.h>


namespace helium{


  DevI2C::DevI2C(const std::string& filename,int address):
    devname(stringBegin()<<filename<<":"<<address<<stringEnd()){

    if ((fd = open(filename.c_str(), O_RDWR))<0 ){
      throw exc::I2CException(devname,
			      "Failed opening port"
			      );
    }
    
     if (ioctl(fd, I2C_SLAVE, address) < 0) {
       // Set the port options and set the address of the device we wish to speak to       
       throw exc::I2CException(devname,
			       "Unable to get bus access to talk to slave"
			       );
       
     }         else{
      std::cout<<"opened  address "<<address<<std::endl;
    }
  }//DevI2C
    
  
  void DevI2C::write(const unsigned char* c,int n){
    int got;
    if ((got=::write(fd,c,n))!=n){
      throw exc::I2CException(devname,
			      stringBegin()<<"Unable to write "<<n<<" bytes"<<got<<stringEnd()
			      );
    }
    
  }
  
  void DevI2C::read(unsigned char* c,int n){
    if (::read(fd,c,n)!=n){
      throw exc::I2CException(devname,
			      stringBegin()<<"Unable to read "<<n<<" bytes"<<stringEnd()
			      );
    }
  }


  DelayedDevI2C::DelayedDevI2C(const std::string& filename,int address,Time pdelay):DevI2C(filename,address),delay(pdelay){
    
  }

  void DelayedDevI2C::write(const unsigned char* c,int n){
    DevI2C::write(c,n);
    milliSleep(delay);
  }


}
