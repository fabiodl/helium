#ifndef HE_SONYJOG
#define HE_SONYJOG

#include <helium/thread/thread.h>
#include <helium/thread/timer.h>
#include <helium/system/valueHub.h>

namespace helium{
  class SonyJog:public Thread{
    FILE *fp;
    bool running;
    int packet[8];
    int p;

    void parseMouse();
    void parseKeyb();
    void run();
  protected:
    void push(int c);
    void flush();
  public:        
    SonyJog(const char* devname);
    ~SonyJog();
    ValueHub<bool> leftClick,rightClick;
    Signal<int> stickx,sticky;
    ValueHub<bool>  shift;
    Signal<int>  jog;
    ValueHub<int> keys; 
    Signal<int> wheel; //+1 counterclockwise
                       //-1 clockwise    

    enum {LPACKETTYPE=0};
    enum {KEYBTYPE=1,MOUSETYPE=2};
    enum {MOUSEPACKETLENGTH=5,KEYBPACKETLENGTH=8};
    
    enum {LMOUSEBUTTON=1,LSTICKX=2,LSTICKY=3};
    enum {MOUSELEFT=1,MOUSERIGHT=2};
    
    enum {LSHIFT=1};
    
    static const int KEYS=13;
    
    enum  {SHIFT,
	   OPEN,PREVIEW,CAP,REC,
	   STOP,MODE,PAUSE,
	   REWIND,PLAY,FASTFW,
	   MARK,CANCEL} Keys;

    
    static const int KeyValues[KEYS] ;

    class StickFilter:public Callback<void>,public Callback<int>,
		      public ConnReference<void>,public ConnReference<int>{
    public:
      ValueHub<double> value;
      StickFilter();
      void operator()(int);
    private:
      
      PosticipableTimer timer;
      int zeroCounts;
      void operator()();
    };
    
    
  };
  
  class AbsoluteSonyJog:public SonyJog{
  public:
    StickFilter absoluteX,absoluteY;
    AbsoluteSonyJog(const char* devname);
  };





}

#endif
