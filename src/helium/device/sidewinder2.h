/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags e
 */


#ifndef HE_SIDEWINDER2_
#define HE_SIDEWINDER2_

#include <linux/input.h>
#include <utility>
#include <helium/thread/thread.h>
#include <helium/core/enum.h>
//Coordinates are screen coordinates(not cartesian): y is bigger going down
namespace helium{

  class SideWinder2{
    int fd;
    
  public:
    SideWinder2();
    ~SideWinder2();
    
    void uploadEffect(struct ff_effect&);
    void removeEffect(struct ff_effect&);
    void printInfo();
    void setAutocenter(double force);
    void setGain(double force);
    void activateEffect(struct ff_effect&);
    void disactivateEffect(struct ff_effect&);

    void readPos(double& x,double& y);

    //reads maximum of maxEvents, return the actual number of events read
    int getEvents(struct input_event*,int maxSize);

    static void makeSpring(struct ff_effect& eff,double forceCoeff,double forceSaturation,double x,double y,bool newID=true);
    static void makeForce(struct ff_effect& effect,double nx, double ny,bool newID=true);
    static void makeDamper(struct ff_effect& effect,double force,bool newId=true);

    class EventReader:public Thread{

    public:
      EventReader(SideWinder2&,bool autostart=true);
      ~EventReader();
      
      struct State{
	enum Button{BTNTRIGGER,BTN2,BTN3,BTN4,BTN5,BTN6,BTN7,BTN8,BTNDEAD,NUMBUTTONS};
	enum Axis{X,Y,RZ,THROTTLE,HATX,HATY,NUMAXES};
	
	bool button[NUMBUTTONS];       
	double axis[NUMAXES];
	State();
      };

     
      /*struct Mask{
	bool button[State::NUMBUTTONS];
	bool axisMask[State::NUMAXIS];
      };
      */
      
      Signal<State*>  onButtonStateChange[State::NUMBUTTONS];
      Signal<State*>  onAxisStateChange[State::NUMAXES];

      void run();
    private:

      SideWinder2& sw;
      bool running;
      State state;
      
      void parseKey(int button,int key);
      void parseAxis(int axisid,double val);

    }; 


};


  template<>
  class EnumStrings<SideWinder2::EventReader::State::Axis>{
  public:
    static const int NUM=SideWinder2::EventReader::State::NUMAXES;
    static const char* strings[NUM];    
    
  };

    template<>
    class EnumStrings<SideWinder2::EventReader::State::Button>{
    public:
      static const int NUM=SideWinder2::EventReader::State::NUMBUTTONS;
      static const char* strings[NUM];    
    
    };


  
}



#endif
