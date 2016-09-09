
namespace helium{

#ifdef _WIN32

  HidRaw::HidRaw(int vendorId,int productId,const char *manufacturer, const char* product,Time timeout){
    COMMTIMEOUTS timeouts;
    getCommTimeouts(getHandle(),&timeouts);
    timeouts.ReadIntervalTimeout=
      timeouts.ReadTotalTimeoutMultiplier=
      timeouts.ReadTotalTimeoutConstant=
      timeouts.WriteTotalTimeoutMultiplier=
      timeouts.WriteTotalTimeoutConstant=      
      timeout;
    SetCommTimout(getHandle(),&timeouts);    
  }



#else

 HidRaw:: HidRaw(int vendorId,int productId,Time time,const char *manufacturer, const char* product):
    UsbHid(vendorId,productId,manufacturer,product),
    timeout(getHandle().fd,time)
  {
  }


#endif

}
