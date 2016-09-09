/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/util/ioUtil.h>
namespace helium{
 template<typename Signature>
  void deserializeUpdate(typename Signature::UT& u,std::istream& i){
    readWholeShws(u,i);    
  }

  template<typename Signature>
  void deserializeGet(typename Signature::GT& u,std::istream& i){
    readWholeShws(u,i);    
  }

  template<typename Signature>
  void deserializeSetParam(typename Signature::SP& u,std::istream& i){
    readWholeShws(u,i);    
  }
}
