/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_USBHID
#define HE_USBHID


#include <helium/core/exception.h>
#include <helium/util/ioUtil.h>
#include <stdint.h>
//#ifndef _WIN32
//#include <helium/os/timeout.h>
//#endif

namespace helium{

  namespace exc{
    class UsbException:public Exception{
    public:
      UsbException(const std::string& s=""):Exception("USB exception"+addDetail(s)){
      }
      
      UsbException(const std::string& on,const std::string& s):Exception("USB exception"+addDetail("while "+on+":"+s)){
      }
      
      virtual void rethrow(){
	throw *this;
      }

      Exception* clone() const {
	return new UsbException(*this);
      }
      
    };

    class UsbPeripheralNotFound:public UsbException{
    public:




      UsbPeripheralNotFound(int vendorId,int productId,const std::string& name="",const std::string& detail=""):
	UsbException("peripheral:"+toHexString(vendorId,4)+":"+toHexString(productId,4)+addDetail(name," aka ","")+" not found"+addDetail(detail))
      {
      }

      virtual void rethrow(){
	throw *this;	
      }

      Exception* clone() const {
	return new UsbPeripheralNotFound(*this);
      }


    };//    USBPeripheralNotFound

    

    class UsbWriteException:public UsbException{
    public:
      int errnum;
      UsbWriteException(int size,const std::string& s="",int perrno=-1):
	UsbException("unable to write "+toString(size)+" bytes"+addDetail(s)),errnum(perrno){
      }
	
      virtual void rethrow(){
	throw *this;	
      }

      Exception* clone() const {
	return new UsbWriteException(*this);
      }
      
    };


    class UsbReadException:public UsbException{
      int errnum;
    public:
      UsbReadException(int size,const std::string& s="",int perrno=-1):
	UsbException("unable to read "+toString(size)+" bytes"+addDetail(s)),errnum(perrno){
      }


      virtual void rethrow(){
	throw *this;	
      }

      Exception* clone() const {
	return new UsbReadException(*this);
      }

    };


  }

  

  template<int N,
	   bool padRead=
#if defined( _WIN32)||defined(USE_LIBUSB)
	   true
#else
	   false
#endif
	   ,
	   bool padWrite=
	   true
	   >
  class UsbMessage:public Array<unsigned char,
				N==array::RESIZABLE?array::RESIZABLE: N+1
				>
{
private:
  typedef Array<unsigned char,
		N==array::RESIZABLE?array::RESIZABLE: N+1
		> Super;
  size_t size();//avoid using this,specify as under
 
public:
  UsbMessage& operator=(const unsigned char*);
  operator const unsigned char*() const;
  operator unsigned char*() ;
  
  const unsigned char& operator[](int idx) const;    
  unsigned char& operator[](int idx);
    
  size_t payloadSize() const ;
  size_t completeSize() const;

  void resize(size_t n);

  UsbMessage(int size=0);
  void setReportId(uint8_t id);
  uint8_t getReportId(uint8_t id);
 

  };
  


  template<int N,bool padRead,bool padWrite>
  std::ostream& operator<<(std::ostream&,const   UsbMessage<N,padRead,padWrite>&);
  
  //the following Hid class includes a timeout

class UsbHid{
public:

#ifdef _WIN32
  typedef void* Handle;  
#else


#if defined(USE_LIBUSB)||defined(__APPLE__)
  struct Handle{
    void* device;
    Handle():device(NULL){}
  };

#else
  struct Handle{
    int fd;
    Handle():fd(-1){}
    /*void* hid;
      Array<int> fromDevicePath;
      Array<int> toDevicePath;
      Handle():hid(NULL){}*/
  };
#endif  


#endif
private:
  Handle hid;
  
protected:
  Handle getHandle();

public:
  UsbHid(int vendorId,int productId,const char *manufacturer=NULL, const char* product=NULL);  
  ~UsbHid();

  //the first byte of buff is 0 for send in both linux and win
  //for recv, only in windows
  //these functions throw exceptions when there're errors,no need to check for return value -1
  int send(const unsigned char* buff,int size); 
  int recv(unsigned char* data,int size);
  void flushInput();
  


  //Pass A UsbMessage for OS portability;
  template<int N,bool padRead,bool padWrite> void sendAll(const UsbMessage<N,padRead,padWrite>&);
  template<int N,bool padRead,bool padWrite> void recvAll(UsbMessage<N,padRead,padWrite>&);
    template<int N,bool padRead,bool padWrite> int recv(UsbMessage<N,padRead,padWrite>&);
  

  void close();
};

 


}

#include <helium/os/usbHid.hpp>

#endif
