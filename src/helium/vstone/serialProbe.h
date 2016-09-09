/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SERIALPROBE
#define HE_SERIALPROBE

#include <map>
#include <memory>

#include <helium/os/serial.h>

namespace helium{
class SerialProbe{
  static const int TIMEOUT=300;
  static const int BAUDRATE=115200;
public:  
  class Prober{  
  public:
    std::string name;
    std::auto_ptr<Serial> serial;
    virtual bool probe(Serial* s)=0;
    Prober(const std::string& pname):name(pname),serial(NULL){}
    virtual ~Prober(){}
  };
private:
  std::vector<std::string> probe,serial;  


  std::vector<Prober*> probers;
  std::map<std::string,std::vector<Prober*> > onTest;
  
  enum Target{TSPECIFIC,TALL};
  
  Prober* getProber(const std::string& s);
  void fillOnTest();
  void setSpecified(); 
  void removeProber(Prober* p);
public:
  //probestrings: 
  //format [all] [serial 1] ...[serial n] [prober id 1] [serial 1] ...[serial n] [prober id 2] [serial 1] ...[serial n] 
  //serialStrings:
  //format [prober id 1 serial 1] ... [prober id n serial n]

  SerialProbe(const std::string& probeStrings,
	      const std::string& serialStrings);
  void execProbe();

  void addProber(Prober& p);
};
  
  
  class VsIdProber:public SerialProbe::Prober{
    int id;
    int addr;
    int size;    
  public:
    
    VsIdProber(const std::string& name,int pid,int paddr,int psize);
    bool probe(Serial* s);
    //void force(Serial* s);
  };


  class VsCrcIdProber:public SerialProbe::Prober{
    int id;
    int addr;
    int size;    
  public:    
    VsCrcIdProber(const std::string& name,int pid,int paddr,int psize);
    bool probe(Serial* s);
    //void force(Serial* s);
  };


  




}//helium

#endif
