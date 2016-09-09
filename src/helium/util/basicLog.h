/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags e
 */

#ifndef HE_BASICLOG
#define HE_BASICLOG

#include <fstream>
#include <helium/util/mtLog.h>
#include <helium/os/time.h>

namespace helium{



  class BasicLog:public MtLog<std::fstream>{
 
public:
    std::fstream out;
  BasicLog(const std::string& prefix,
	   const std::string& id=getAsciiDateTime(),
	   const std::string& suffix=".txt"):
    MtLog<std::fstream>(out),
    out((prefix+id+suffix).c_str(),std::fstream::out){}
  
};


  class TimedLog:public BasicLog{

  public:
    template<typename T> RealLog& operator<<(T& t){
      Time time=getSystemMillis();
      return (*static_cast<BasicLog*>(this))<<time<<" "<<t;
    }

    TimedLog(const std::string& prefix,
	   const std::string& id=getAsciiDateTime(),
	     const std::string& suffix=".txt"):BasicLog(prefix,id,suffix){
    }
    

  };



  class RelativeTimedLog:public BasicLog{
    Time zeroT;
  public:
    template<typename T> RealLog& operator<<(T& t){
      return (*static_cast<BasicLog*>(this))<<getElapsedMillis(zeroT)<<" "<<t;
    }

    RelativeTimedLog(const std::string& prefix,
	   const std::string& id=getAsciiDateTime(),
	     const std::string& suffix=".txt"):
      BasicLog(prefix,id,suffix),
      zeroT(getSystemMillis())
    {
    }
    

  };


}

#endif
