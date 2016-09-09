/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef _KEYBMOUSEINTERACTION_
#define _KEYBMOUSEINTERACTION_

#include <iostream>

namespace helium{
  class PrimitiveMouseInteraction{
  public:
    enum MouseButton{NOBUTTON,LEFTBUTTON,RIGHTBUTTON,MIDDLEBUTTON,LEFTANDRIGHTBUTTON};
    virtual void onMouseClicked(MouseButton button,bool down,int x,int y){}
    virtual void onMouseMoved(int x,int y,int prevX,int prevY,MouseButton  pushButton,int pushX,int pushY){}
  };
  
  class PrimitiveKeyboardInteraction{
  public:
    enum KeyParsingState {UNMANAGED,MANAGED,EXCLUSIVE};
    virtual KeyParsingState onKeyPushed(unsigned char key,int x,int y){std::cout<<"Null keyboard action"<<std::endl; return UNMANAGED;}
    virtual KeyParsingState onKeyReleased(unsigned char key,int x,int y){return UNMANAGED;}
  };
}

#endif
