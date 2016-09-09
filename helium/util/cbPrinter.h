/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef  HE_CBPRINTER
#define  HE_CBPRINTER

#include <sstream>
#include <helium/signal/callback.h>
#include <helium/util/mout.h>

namespace helium{
  //template <typename T> class CbPrinter:public Callback<const T&>{
  template <typename T> class CbPrinter:public Callback<T>{
    std::string header,footer;
  public:
    CbPrinter(const std::string& phead="",const std::string& pfoot="\n"):
      header(phead),footer(pfoot)
    {
    }

    //void operator()(const T& t){
    void operator()(T t){
      std::stringstream ss;
      ss<<header;
      write(ss,t);
      ss<<footer;
      atomicOut<<ss<<endAtomic;
    }
  };


  template <> class CbPrinter<void>:public Callback<void>{
    std::string msg;
  public:
    CbPrinter(const std::string& pmsg):
      msg(pmsg)
    {
    }

    void operator()(){
      atomicOut<<msg<<endAtomic;
    }
  };

  template<typename T> class ConnPrinter:public CbPrinter<T>,public ConnReference<T>{
  public: 
    ConnPrinter(const std::string& phead="",const std::string& pfoot="\n"):
      CbPrinter<T>(phead,pfoot),
      ConnReference<T>(this){
    }

  };


  template<> class ConnPrinter<void>:public CbPrinter<void>,public ConnReference<void>{
  public:
    ConnPrinter(const std::string& msg):
      CbPrinter<void>(msg),ConnReference<void>(this){
    }

  };


}

#endif
