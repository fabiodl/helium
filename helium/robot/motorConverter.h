#ifndef HE_MOTORCONVERTER
#define HE_MOTORCONVERTER
namespace helium{

class MotorConverter{
public:
  virtual double toRelative(int j,int absVal)=0;
  virtual  int toAbsolute(int j,double relVal)=0;


};

}

#endif
