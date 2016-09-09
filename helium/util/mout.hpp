/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/util/stringUtil.h>
namespace helium{

  template<typename Out>  
  Mout<Out>::Mout(MtLog<Out>& pout):out(pout){
  }
  extern const char moutChars[6];

  template<typename Out>
  typename MtLog<Out>::RealLog&  Mout<Out>::operator()(msg::MsgType mt,int ident){
    return out<<spaces(ident*3)<<"["<<moutChars[mt]<<"]";

  }


}
