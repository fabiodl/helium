/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/util/mout.h>


namespace helium{
  const char moutChars[6]={' ','*','E','W','+','-'};
  
  MtLog<std::ostream> atomicOut(std::cout);
  MtLog<LinkedLog<std::ostream,MtLog<std::ostream> > > atomicErr(std::cerr,atomicOut);
  
  Mout<std::ostream> mout(atomicOut);  
  Mout< LinkedLog<std::ostream,MtLog<std::ostream> > > merr(atomicErr);

}
