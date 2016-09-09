/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */


#ifndef HE_TYPENAME_
#define HE_TYPENAME_


#include <typeinfo>
#include <string>

namespace helium{

  template <typename T> class Typename{
  public:    
    static const std::string name;    
  };
  
  
  
  template <typename T>
  const std::string  Typename<T>::name=std::string(
#ifndef __MINGW32__						   
						   typeid(T).name()
#else
						   "mingw32"
#endif						   
                                                   );
  
}


#endif
