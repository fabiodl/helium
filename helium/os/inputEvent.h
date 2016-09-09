/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags s
 */


#ifndef _HE_INPUTEVENT
#define _HE_INPUTEVENT

#include <string>

namespace helium{

  std::string  getEventFilename(int vendorID,int productID,const std::string& name="");


}



#endif
