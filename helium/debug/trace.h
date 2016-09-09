/*!
 * \details http://sourceforge.net/projects/libhelium/
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_DEBUG
#define HE_DEBUG

#include <helium/debug/debugOptions.h>
#include <iostream>
namespace helium{


#ifdef TRACEDEBUG
  void printTrace(int toIgnore=1,std::ostream &out=std::cerr);

  namespace exc{
    extern bool ExceptionsAreSilent;
    class TraceSilencer{
    public:
      TraceSilencer(){ExceptionsAreSilent=true;}
      ~TraceSilencer(){ExceptionsAreSilent=false;}
    };
  }
#else
  inline void printTrace(int toIgnore=1,std::ostream &out=std::cerr){
  }
#endif

}//helium


#endif //ifndef he_debug


