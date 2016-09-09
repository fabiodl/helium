/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */


#ifndef HE_PEDAL
#define HE_PEDAL

#include <helium/thread/thread.h>
#include <helium/signal/signal.h>
#include <helium/util/cbPrinter.h>

namespace helium{




  class LogitecGTForce:public Thread{
    
    FILE* handle;

    int checksum1;
    int checksum2;
    int  rdownThreshold;
    int rupThreshold;
    int ldownThreshold;
    int lupThreshold;
    bool rpedalDown,lpedalDown;
    virtual void run();
    enum DataFormat {WHEEL,BUTTONS,CHECKSUM1,DIFFERENTIAL,RIGHTPEDAL,LEFTPEDAL,CHECKSUM2,DATANUM};
    bool running;


  public:


    static const unsigned char LBUTTON=0x04;
    static const unsigned char RBUTTON=0x08;
    static const unsigned char XBUTTON=0x10;
    static const unsigned char YBUTTON=0x20;
    static const unsigned char ABUTTON=0x40;
    static const unsigned char BBUTTON=0x80;


    Signal<unsigned char*> dataReceived;
    Signal<void> rightPedalDown;
    Signal<void> rightPedalUp;

    Signal<void> leftPedalDown;
    Signal<void> leftPedalUp;

    Signal<int> wheelRotation;
    Signal<char> keyPressed;

    LogitecGTForce(const char* dev, bool autostart=true,
		   int checksum1=207,
		   int checksum2=2,
		   int rdownThreshold=145,
		   int rupThreshold=180,
		   int ldownThreshold=145,
		   int lupThreshold=180		  
		   );
    ~LogitecGTForce();

  };
  
  class LogitecGTEventPrinter{
    LogitecGTForce& gt;
    ConnPrinter<void> rpdown;
    ConnPrinter<void> rpup;
    ConnPrinter<void> lpdown;
    ConnPrinter<void> lpup;
    ConnPrinter<int> wheelRotation;
    ConnPrinter<char> keyPressed;

  public:
    LogitecGTEventPrinter(LogitecGTForce& gt);
    ~LogitecGTEventPrinter();
  };


  
}


#endif
