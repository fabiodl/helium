/*!
 * \brief   android
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags e
 */


#ifndef HE_CALLBACKPRINTER
#define HE_CALLBACKPRINTER


#include <iostream>
#include <helium/util/ioUtil.h>
#include <helium/signal/signal.h>
#include <helium/os/time.h>
namespace helium{
  template<typename T>
  class CallbackPrinter:public Callback<T>,public ConnReference<T>{  

    std::string prefix,suffix;

    
  public:    
    void operator()(T t){
      std::cout<<prefix;
      print(t)<<suffix<<std::flush;
    }

    CallbackPrinter(const std::string& pprefix="",const std::string& psuffix="\n"):
      ConnReference<T>(this),
      prefix(pprefix),suffix(psuffix)
      {
    }


};



  template<typename T>
  class CallbackTimePrinter:public Callback<T>,public ConnReference<T>{  

    std::string prefix,suffix;

    void operator()(T t){
      static const Time zeroT=getSystemMillis();
      std::cout<<(getSystemMillis()-zeroT)<<prefix;
      print(t)<<suffix<<std::flush;
    }
    
  public:    
    CallbackTimePrinter(const std::string& pprefix=" ",const std::string& psuffix="\n"):
      ConnReference<T>(this),
      prefix(pprefix),suffix(psuffix)
      {
    }


};


}

#endif
