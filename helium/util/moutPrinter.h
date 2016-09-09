/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_MOUTPRINTER
#define HE_MOUTPRINTER

#include <helium/util/mout.h>
#include <helium/signal/callback.h>

namespace helium{
template<typename D,msg::MsgType msgType> 
class MoutPrinter:public Callback<D>{
  std::string pre,post;
public:
  void operator()(D d){
    mout(msgType)(pre+toString(d)+post);
  }
  
  MoutPrinter(const std::string& ppre="",const std::string& ppost=""):
    pre(ppre),post(ppost){
  }
};

}


#endif
