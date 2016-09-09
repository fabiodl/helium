#include <helium/wrap/vusb/vusb.h>
#include <helium/core/exception.h>
#include <helium/util/ioUtil.h>
namespace helium{


  const std::string VusbDevice::defaultVendor="fabiodl@gmail.com";


  namespace exc{

  class VUSBException:public Exception{

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new VUSBException(*this);
    }


    std::string getErrorDescription(int errCode){      
      switch(errCode){
      case USBOPEN_ERR_ACCESS:      return "Access to device denied";
      case USBOPEN_ERR_NOTFOUND:    return "The specified device was not found";
      case USBOPEN_ERR_IO:          return "Communication error with device";
      default:
	return "Unknown USB error "+toString(errCode);
      }
      return "BUG";
    }

  public:
    VUSBException(int vendorid, 
		  int deviceid,
		  const std::string& vendorname,		  
		  const std::string& productname,		  
		  int errCode):Exception(
					 stringBegin()<<"V-usb device "<<std::hex<<vendorid<<":"<<deviceid<<" ("<<vendorname<<":"<<productname<<"):"<<getErrorDescription(errCode)<<stringEnd())
{      
      
    }

  };


  }

  VusbDevice:: VusbDevice(const std::string& pproductName,
	       const std::string& pvendorName,
			  int vendorid,int deviceid):
    dev(NULL),
    productId(deviceid),vendorId(vendorid),productName(pproductName),
    vendorName(pvendorName)									 
  {
    
    int err;
    if(( err = usbhidOpenDevice(&dev, vendorid, const_cast<char*>(vendorName.c_str()), deviceid, const_cast<char*>(productName.c_str()), 0)) != 0){
      throw exc::VUSBException(vendorid,deviceid,vendorName,productName,err);
    }
    
    

  }

  VusbDevice:: ~VusbDevice(){
    usbhidCloseDevice(dev);
  } 
  
  void VusbDevice::hidSend(const unsigned char* c,int n){
    int err;
    if((err = usbhidSetReport(dev, (char*)c,n)) != 0){   /* add a dummy report ID */
      throw exc::VUSBException(vendorId,productId,vendorName,productName,err);
    }
   
  }
  
}
