/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SERIALVSRC
#define HE_SERIALVSRC

#include <helium/os/serial.h>
#include <helium/robot/sensorInterface.h>
#include <helium/vstone/serialProbe.h>

namespace helium{

  class SerialVsRc003{
    
    Serial& s;
    unsigned char buffer[256];
    void readToBuffer(int address,int length);

  public:
    
    SerialVsRc003(Serial& s);
    

    void read(int*,int address,int length);

    class Gyro:public sensor::Set<int>{
      SerialVsRc003& owner;
    public:
      int rawRead(int idx);
      void rawRead(int*);
      void rawRead(int*, const HwIdList&);
      void rawRead(SensorValList<int>& , const HwIdList&);
      void getToken(Token& t, const HwIdList& toRead);
      void rawRead(int*,const Token& t);
      size_t size() const;
      Gyro(SerialVsRc003& powner);
      HwState isWorking(int id);
      void isWorking(Array<HwState>& ,const HwIdList&);
      void getWorkingOptimizedSplit(Array <WorkingToken>& tk,const HwIdList &) const;
      void isWorking(Callback<const HwStateNotification&>*,const WorkingToken&);

    };    

    Gyro gyro;

    class Probe:public SerialProbe::Prober{
    public:
      Probe(const std::string& name);
      bool probe(Serial* s);
    };


  };


};



#endif
