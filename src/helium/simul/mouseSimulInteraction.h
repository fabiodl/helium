#ifndef HE_MOUSESIMULINTERACTION
#define HE_MOUSESIMULINTERACTION

#include <helium/graphics/glPrimitiveCanvas.h>
#include <helium/simul/structure.h>

namespace helium{

class MouseSensorListener{
 public:
  virtual void onSensorSelected(int selId,Robot* selRobot,int prevID,Robot* prevRobot){};
  virtual void onClickOccurred(PrimitiveMouseInteraction::MouseButton button, bool down,int selSensors,const Robot* selRobot){std::cout<<"Null mouse action"<<std::endl;};
};

}

#endif
