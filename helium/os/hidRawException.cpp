/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#include <helium/os/hidRawException.h>
#include <helium/core/enum.h>
#include <helium/util/ioUtil.h>
namespace helium{

  template<>
  struct EnumStrings<exc::HidRawException::Operation>{    

    static const int NUM=exc::HidRawException::NUMOPERATIONS;
    static const char* strings[NUM];
  };

  const char* EnumStrings<exc::HidRawException::Operation>::strings[EnumStrings<exc::HidRawException::Operation>::NUM]=
    {"open",
     "requestread",
     "read",
     "write",
     "requestloopread"
    };
								     
  
  

  
  namespace exc{

    HidRawException::HidRawException(int dev,int addr,int length,Operation op,const std::string& detail):
      Exception((stringBegin()<<"Device "<<std::hex<<dev<<" operation="<<dev<<en::io<<op<<" addr="<<addr<<std::dec<<" length="<<length<<stringEnd())+addDetail(detail))
    {
  
      
    }

    void HidRawException::rethrow(){
      throw *this;
    }
    
    Exception* HidRawException::clone() const{
      return new HidRawException(*this);
    }
    


  }

}
