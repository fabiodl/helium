#ifndef HE_ROBOTEXCEPTION
#define HE_ROBOTEXCEPTION

#include <helium/core/exception.h>
#include <helium/util/ioUtil.h>
#include <string>

namespace helium{
namespace exc{
  
  class HardwareException:public Exception{
  public:
    HardwareException(const std::string& s=""):Exception("Hardware problem"+addDetail(s)){
    }
    void rethrow(){
      throw *this;
    }
    
    Exception* clone() const{
      return new HardwareException(*this);
    }
  };

  class MotorException:public HardwareException{
  public:
    MotorException(const std::string& s=""):
      HardwareException("Motor"+addDetail(s)){}

    void rethrow(){
      throw *this;
    }
    
    Exception* clone() const{
      return new MotorException(*this);
    }
  };  
  
  class SingleMotorException:public MotorException{
  public:
    int id;
    SingleMotorException(int pid,const std::string& s=""):MotorException("id "+toString(pid)+addDetail(s)),id(pid){
    }
    void rethrow(){
      throw *this;
    }    
    Exception* clone() const{
      return new SingleMotorException(*this);
    }
  };

  class UnreachableMotorException:public SingleMotorException{
 public:
    UnreachableMotorException(int id,const std::string& s): SingleMotorException(id,"unreachable"+addDetail(s)){}
    void rethrow(){
      throw *this;
    }    
    Exception* clone() const{
      return new UnreachableMotorException(*this);
    }
  };

  class FaultyValueMotorException:public SingleMotorException{
  public:
    FaultyValueMotorException(int id,const std::string& s): SingleMotorException(id,"faulty"+addDetail(s)){}
    void rethrow(){
      throw *this;
    }    
    Exception* clone() const{
      return new FaultyValueMotorException(*this);
    }
  };


  class TsensorException:public HardwareException{
  public:
    TsensorException(const std::string& s=""):HardwareException("touch sensor"+addDetail(s)){
    }
    void rethrow(){
      throw *this;
    }
    
    Exception* clone() const{
      return new TsensorException(*this);
    }
  };


  class TsensorBoardException:public TsensorException{
  public:
    int id;
    const std::vector<int>& affectedSensor;
    TsensorBoardException(int pid,const std::vector<int>& paffectedSensor,const std::string& s=""):
      TsensorException("board "+toString(pid)+addDetail(s)),
      id(pid),affectedSensor(paffectedSensor){
    }

     void rethrow(){
      throw *this;
    }
    
    Exception* clone() const{
      return new TsensorBoardException(*this);
    }

  };//TSensorBoardException
  
  class UnreachableTsBoardException:public TsensorBoardException{
  public:
    UnreachableTsBoardException(int id,
				     const std::vector<int>& affectedSensors,
				     const std::string& s=""
				):TsensorBoardException(id,affectedSensors,std::string("unreachable")+addDetail(s)){
    }
     void rethrow(){
      throw *this;
    }
    
    Exception* clone() const{
      return new UnreachableTsBoardException(*this);
    }
  };

   class FaultyValTsBoardException:public TsensorBoardException{
   public:
     FaultyValTsBoardException(int id,const std::vector<int>& affectedSensors, const std::string& s=""):
       TsensorBoardException(id,affectedSensors,"faulty value"+addDetail(s)){
    }
     void rethrow(){
      throw *this;
    }
    
    Exception* clone() const{
      return new FaultyValTsBoardException(*this);
    }
  };
  


    

  class SingleTsensorException:public TsensorException{
  public:
    int id;
    SingleTsensorException(int pid,const std::string& s=""):
      TsensorException("sensor "+toString(pid)+addDetail(s)),id(pid){
    }

     void rethrow(){
      throw *this;
    }
    
    Exception* clone() const{
      return new SingleTsensorException(*this);
    }

  };//TSensorException

  class UnreachableTsensorException:public SingleTsensorException{
  public:
    
    UnreachableTsensorException(int id,const std::string& s=""):SingleTsensorException(id,"unreachable"+addDetail(s)){
    }

     void rethrow(){
      throw *this;
    }
    
    Exception* clone() const{
      return new UnreachableTsensorException(*this);
    }

  };
  

  class FaultyValueTsensorException:public SingleTsensorException{
  public:
    
    FaultyValueTsensorException(int id,const std::string& s=""):SingleTsensorException(id,"unreachable"+addDetail(s)){
    }

     void rethrow(){
      throw *this;
    }
    
    Exception* clone() const{
      return new FaultyValueTsensorException(*this);
    }

   };
  


}//ns exc
}//sn he

#endif
