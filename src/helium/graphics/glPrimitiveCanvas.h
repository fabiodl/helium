/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_GLPRIMITIVECANVAS
#define HE_GLPRIMITIVECANVAS

#include <helium/signal/tsSignal.h>
#include <helium/core/time.h>
#include <helium/graphics/keybMouseInteraction.h>
namespace helium{



  
  class Drawable{
  public:
    virtual void onDraw()=0;
  };

  class GlPrimitiveCanvas:public Drawable{
    int width,height; 
    int x,y;
    std::string winName; 
    Time automaticRedraw;
    Time eventParsing;
    std::vector<Drawable*> obj;   
    friend class GlDrawer;
    
  public:
    std::vector<PrimitiveKeyboardInteraction*> keyboardInter;
    std::vector<PrimitiveMouseInteraction*> mouseInter;
    TsSignal<void> requestDraw;
    TsSignal<void> drawCompleted;
    TsSignal<const std::pair<int,int>& > resized;

    void addKeyboardInteraction( PrimitiveKeyboardInteraction* k){
      keyboardInter.insert(keyboardInter.begin(),k);
    }
  
    void addMouseInteraction( PrimitiveMouseInteraction* m){
      mouseInter.insert(mouseInter.begin(),m);
    }
    
    
    
    void addBg(Drawable* d){
      obj.insert(obj.begin(),d);
    }

    void addFg(Drawable* d){
      obj.push_back(d);
    }

    int getWidth() const {    
      return width;
    }
    
    int getHeight() const{
      return height;
    }
    
    int getX() const {
      return x;
    }
    
    int getY() const {
      return y;
    }

    void setAutomaticRedrawPeriod(Time t){      
      automaticRedraw=t;
    }

    void setEventParsingPeriod(Time t){      
      eventParsing=t;
    }

    Time getAutomaticRedrawPeriod() const {
      return automaticRedraw;
    }

    Time getEventParsingPeriod() const {
      return eventParsing;
    }
    
    void setName(const std::string n){
      winName=n;
    }
    
    std::string getName(){
      return winName;
    }
    


    void draw(){
      //this is inserted in the constructor
      for (size_t i=0;i<obj.size();i++)
	obj[i]->onDraw();
      drawCompleted();
    }
    
    void resize(int w,int h){
      width=w;
      height=h;
      onResize(w,h);
      resized(std::make_pair(w,h));
    }

    void move(int px,int py){
      x=px;
      y=py;
    }

  //virtual void onDraw(){}
    virtual void onResize(int w,int h)=0;  

    GlPrimitiveCanvas(int w,int h,std::string pname="",int px=0,int py=0,int frameRate=15,int eventRate=15):
      width(w),height(h),x(px),y(py),winName(pname),automaticRedraw(frameRate?1000/frameRate:0),eventParsing(eventRate?1000/eventRate:0){
      obj.push_back(this);
    }
  
  };//GlPrimitiveCanvas



  
  
};//helium


#endif
