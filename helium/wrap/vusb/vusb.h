#ifndef HE_VUSB
#define HE_VUSB
#include <string>
#include <vusb/libhost/hiddata.h>

namespace helium{
  class VusbDevice{


  public:
    static const int HIDVENDOR=0x16c0;
    static const int HIDDEVICE=0x05dc;
    static const std::string defaultVendor;

    
    VusbDevice(const std::string& productname,
	       const std::string& vendorName=defaultVendor,
	       int vendorid=HIDVENDOR,int deviceid=HIDDEVICE
	       );
    

    ~VusbDevice();

    void hidSend(const unsigned char* c,int n);//the first byte is the report id and is ignored
    //n is the number of bytes to send(with the report id)


  private:
    usbDevice_t  *dev;
    int productId,vendorId;
    std::string productName,vendorName;





  };



};




#endif
