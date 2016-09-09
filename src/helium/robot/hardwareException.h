/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_HWEXCEPTION
#define HE_HWEXCEPTION

#include <helium/core/exception.h>
#include <helium/util/ioUtil.h>
#include <helium/robot/hwInterface.h>
#include <string>

namespace helium{


  

namespace exc{
  



  class HardwareException:public Exception{
  public:
   HardwareException(const std::string& s=""):Exception("Hardware problem"+addDetail(s)){
    }
    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new HardwareException(*this);
    }

  };




  class HardwareMismatch:public HardwareException{
  public:
    HardwareMismatch(const std::string& expected,const std::string& found):HardwareException(stringBegin()<<"expected "<<expected<<" , found "<<found<<stringEnd() ){
    }

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new HardwareMismatch(*this);
    }

  };
 

  
  /*  template<>  struct EnumStrings<HwType>{
    static const int NUM=3;
    static const char* strings[NUM];       
    };*/

  template<DevType D,HwState S=GENERICFAULTYHWSTATE>
  class HardwareDeviceException;



  template<> 
  class HardwareDeviceException<UNNAMEDDEVICE,GENERICFAULTYHWSTATE>:public HardwareException{
  public:
    HardwareDeviceException(const std::string& device,const std::string& details=""):
      HardwareException(device+":"+addDetail(details)){
    }    

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new HardwareDeviceException(*this);
    }

    virtual HwState getState(){
      return GENERICFAULTYHWSTATE;
    };
    
  };




  template<HwState S>
  class HardwareDeviceException<UNNAMEDDEVICE,S>:
    public HardwareDeviceException<UNNAMEDDEVICE,GENERICFAULTYHWSTATE>{
  public:
    HardwareDeviceException(const std::string& device,const std::string& details=""):
      HardwareDeviceException<UNNAMEDDEVICE,GENERICFAULTYHWSTATE>(device,toString(S)+addDetail(details)){
    }    
    
    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new HardwareDeviceException(*this);
    }

    virtual HwState getState(){
      return S;
    };

  };



  template<>
  class HardwareDeviceException<NAMEDDEVICE,GENERICFAULTYHWSTATE>:public HardwareException{
  public:
    int id;
    HardwareDeviceException(const std::string& device,int pid,const std::string& details=""):
      HardwareException(device+" "+toString(pid)+addDetail(details)),
      id(pid)
    {
    }   

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new HardwareDeviceException(*this);
    }
    
    virtual HwState getState(){
      return GENERICFAULTYHWSTATE;
    };
   
  };


  template<HwState S>
  class HardwareDeviceException<NAMEDDEVICE,S>:
    public HardwareDeviceException<NAMEDDEVICE,GENERICFAULTYHWSTATE>{
  public:   
    HardwareDeviceException(const std::string& device,int pid,const std::string& details=""):
      HardwareDeviceException<NAMEDDEVICE,GENERICFAULTYHWSTATE>(device,pid,toString(S)+addDetail(details))
    {
    }   

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new HardwareDeviceException(*this);
    }

     virtual HwState getState(){
      return S;
    };
    
  };




    template<>
  class HardwareDeviceException<MULTIPLEXINGDEVICE,GENERICFAULTYHWSTATE>:
    public HardwareException{
  public:  
    const HwIdList& affected;
    
    HardwareDeviceException(const std::string& device,const HwIdList& paffected,const std::string& details=""):
      HardwareException(device+addDetail(details)),
      affected(paffected)
    {
    }   

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new HardwareDeviceException(*this);
    }

      virtual HwState getState(){
	return GENERICFAULTYHWSTATE;
      };
    
  };
    

  template<HwState S>
  class HardwareDeviceException<MULTIPLEXINGDEVICE,S>:
    public HardwareDeviceException<MULTIPLEXINGDEVICE,GENERICFAULTYHWSTATE>{
  public:
    
    HardwareDeviceException(const std::string& device,const HwIdList& paffected,const std::string& details=""):
      HardwareDeviceException<MULTIPLEXINGDEVICE,GENERICFAULTYHWSTATE>(device,paffected,toString(S)+addDetail(details))
    {
    }   

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new HardwareDeviceException(*this);
    }

     virtual HwState getState(){
	return S;
     };

    
    
  };




  template<>
  class HardwareDeviceException<NAMEDMULTIPLEXINGDEVICE,GENERICFAULTYHWSTATE>:
    public HardwareException{
  public:
    int id;
    const HwIdList& affected;

    HardwareDeviceException(const std::string& device,int pid,const HwIdList& paffected,const std::string& details=""):
      HardwareException(device+" "+toString(pid)+addDetail(details)),
      id(pid),affected(paffected)
    {

    }   

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new HardwareDeviceException(*this);
    }

    virtual HwState getState(){
      return GENERICFAULTYHWSTATE;
    };
    
  };
    

  template<HwState S>
  class HardwareDeviceException<NAMEDMULTIPLEXINGDEVICE,S>:
    public HardwareDeviceException<NAMEDMULTIPLEXINGDEVICE,GENERICFAULTYHWSTATE>{
  public:
   
    
    HardwareDeviceException(const std::string& device,int pid,const HwIdList& paffected,const std::string& details=""):
      HardwareDeviceException<NAMEDMULTIPLEXINGDEVICE,GENERICFAULTYHWSTATE>(device,pid,paffected,toString(S)+addDetail(details))
    {
    }   

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new HardwareDeviceException(*this);
    }
    
  };



  
  /*


  class MotorException:public HardwareException{
  public:
    MotorException(const std::string& s=""):
      HardwareException("Motor"+addDetail(s)){}

    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new MotorException(*this);
    }
  };  





  
  class SingleMotorException:public MotorException{
  public:
    int id;
    SingleMotorException(int pid,const std::string& s=""):MotorException("id "+toString(pid)+addDetail(s)),id(pid){
    }
    virtual void rethrow(){
      throw *this;
    }    
    virtual Exception* clone() const{
      return new SingleMotorException(*this);
    }
  };

  class UnreachableMotorException:public SingleMotorException{
 public:
    UnreachableMotorException(int id,const std::string& s): SingleMotorException(id,"unreachable"+addDetail(s)){}
    virtual void rethrow(){
      throw *this;
    }    
    virtual Exception* clone() const{
      return new UnreachableMotorException(*this);
    }
  };

  class FaultyValueMotorException:public SingleMotorException{
  public:
    FaultyValueMotorException(int id,const std::string& s): SingleMotorException(id,"faulty"+addDetail(s)){}
    virtual void rethrow(){
      throw *this;
    }    
    virtual Exception* clone() const{
      return new FaultyValueMotorException(*this);
    }
  };


  class TsensorException:public HardwareException{
  public:
    TsensorException(const std::string& s=""):HardwareException("touch sensor"+addDetail(s)){
    }
    virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new TsensorException(*this);
    }
  };


  class TsensorBoardException:public TsensorException{
  public:
    int id;
    const HwIdList& affected;
    TsensorBoardException(int pid,const HwIdList& paffected,const std::string& s=""):
      TsensorException("board "+toString(pid)+addDetail(s)),
      id(pid),affected(paffected){
    }

     virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new TsensorBoardException(*this);
    }

  };//TSensorBoardException
  
  class UnreachableTsBoardException:public TsensorBoardException{
  public:
    UnreachableTsBoardException(int id,
				     const HwIdList& affecteds,
				     const std::string& s=""
				):TsensorBoardException(id,affecteds,std::string("unreachable")+addDetail(s)){
    }
     virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new UnreachableTsBoardException(*this);
    }
  };

   class FaultyValTsBoardException:public TsensorBoardException{
   public:
     FaultyValTsBoardException(int id,const HwIdList& affecteds, const std::string& s=""):
       TsensorBoardException(id,affecteds,"faulty value"+addDetail(s)){
    }
     virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new FaultyValTsBoardException(*this);
    }
  };
  


    

  class SingleTsensorException:public TsensorException{
  public:
    int id;
    SingleTsensorException(int pid,const std::string& s=""):
      TsensorException("sensor "+toString(pid)+addDetail(s)),id(pid){
    }

     virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new SingleTsensorException(*this);
    }

  };//TSensorException

  class UnreachableTsensorException:public SingleTsensorException{
  public:
    
    UnreachableTsensorException(int id,const std::string& s=""):SingleTsensorException(id,"unreachable"+addDetail(s)){
    }

     virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new UnreachableTsensorException(*this);
    }

  };
  

  class FaultyValueTsensorException:public SingleTsensorException{
  public:
    
    FaultyValueTsensorException(int id,const std::string& s=""):SingleTsensorException(id,"faulty value"+addDetail(s)){
    }

     virtual void rethrow(){
      throw *this;
    }
    
    virtual Exception* clone() const{
      return new FaultyValueTsensorException(*this);
    }

   };
  
  */

}//ns exc
}//sn he

#endif
