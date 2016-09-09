#ifndef HE_USBIO
#define HE_USBIO

#include <helium/os/usbHid.h>
#include <helium/thread/thread.h>
#include <helium/signal/ehSignal.h>

namespace helium{

  namespace avrPort{
    enum Name{A,B,C,D};
    enum Direction{IN,OUT,ALLIN=0,ALLOUT=0xFF};
    
    class ConditionMask{
      
    public:
      ConditionMask(size_t s);
      void reset();
      Array<unsigned char> setCheck,clearCheck;      
      bool operator()(const unsigned char*) const;
    };


  };

  

  class HidUSBIO:public UsbHid,public Thread{

  public:
    EhSignal<const unsigned char*,true> onRead,onUpdate,onAck;

    HidUSBIO(int portNum);
    virtual ~HidUSBIO();
    void setDirection(avrPort::Name p,unsigned char mask);
    void setDirection(avrPort::Name p,int bit,bool value);    
    void setUpdateRead(avrPort::Name p,unsigned char mask);
    void setUpdateRead(avrPort::Name p,int bit,bool value);
    void includeUpdateRead(const avrPort::ConditionMask& s);

    void setOutput(const unsigned char*);
    void setOutput(avrPort::Name p,unsigned char val);
    void setOutput(avrPort::Name p,int bit,bool x);
    void setPullup(avrPort::Name p,unsigned char val);
    void setPullup(avrPort::Name p,int bit,bool x);
    
    

    unsigned char read(avrPort::Name p);
    bool read(avrPort::Name b,int bit);
    void read(unsigned char*);

    void waitFor(const avrPort::ConditionMask& c);

  private:
    volatile bool running;
    enum Cmd{
      CMDDDR,
      CMDPORT,
      CMDREAD,
      CMDUPDATE,
      DATAUPDATE,
      INVALIDCMD
    };
    Array<unsigned char> ddr,updatemask,port,toRead;
    void sendPacket(Cmd,const unsigned char* data);
    void run();
    UsbMessage<array::RESIZABLE> outmsg,inmsg;

    
    class DataWaiter:public Callback<const unsigned char*>,public EhConnReference<const unsigned char*>{
      exc::ExceptionHolder eh;
      Semaphore stateChanged;
      void handle(exc::Exception&);
      void operator()(const unsigned char*);
      
    public:
      DataWaiter(size_t size);
      Array<unsigned char> data;
      void waitData();//may throw exceptions
    };
    DataWaiter readWaiter,updateWaiter,ackWaiter;
    
    
  };//HidUSBIO
  

}



#endif
