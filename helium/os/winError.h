/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#if !defined(HE_WINERROR)&&defined(_WIN32)
#define HE_WINERROR
#include <windows.h>
#include <string>
#include <sstream>
namespace helium{

  inline std::string getLastErrorMsg(){
    LPTSTR lpMsgBuf; //long pointer constant T(wide or not) char*
    FormatMessage( 
		  FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		  FORMAT_MESSAGE_FROM_SYSTEM | 
		  FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL,
		  GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		  (LPTSTR) &lpMsgBuf, //we want to allocate ,and return the address
		  0,
		  NULL 
		   );
    std::stringstream ss;
    ss<<lpMsgBuf;
    LocalFree( lpMsgBuf );  
    return ss.str();
  }
  
}
#endif
