/*Fabio Dalla Libera   sourceforge.net/projects/libhelium/  */
#ifndef HE_DEBUG
#define HE_DEBUG

#include <iostream>


namespace helium{
#ifdef TRACEDEBUG
void printTrace(int toIgnore=1,std::ostream &out=std::cerr);
#else
inline void printTrace(int toIgnore=1,std::ostream &out=std::cerr){
}
#endif
}//helium



#endif //ifndef he_debug


