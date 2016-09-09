/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_MOUT
#define HE_MOUT
#include <helium/util/mtLog.h>

namespace helium{

  namespace msg{
    enum MsgType {PLAIN,OK,ERR,WARNING,PINFO,NINFO};
  }

  template<typename Out>
  class Mout{
    MtLog<Out>& out;
  public:
    
    typename MtLog<Out>::RealLog& operator()(msg::MsgType,int ident=0);    
    Mout(MtLog<Out>& pout);
  };

  extern MtLog<std::ostream> atomicOut;
  extern MtLog<LinkedLog<std::ostream,MtLog<std::ostream> > > atomicErr;

  extern Mout<std::ostream> mout;
  extern Mout<LinkedLog<std::ostream,MtLog<std::ostream> > > merr;
}
#include <helium/util/mout.hpp>
#endif
