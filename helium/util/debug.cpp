/*Fabio Dalla Libera   sourceforge.net/projects/libhelium/  */

#include <helium/core/debugLevel.h>
#include <helium/util/debug.h>
#include <iostream>
#include <helium/thread/mutex.h>

#ifdef TRACEDEBUG


namespace helium{
namespace exc{
  bool ExceptionsAreSilent=false;
}
}


#if defined(_WIN32)||defined(__CYGWIN__)
namespace helium{
  void printTrace(int toIgnore,std::ostream &o){
  }
#else


#include <execinfo.h>
#include <cxxabi.h>
#include <cstring>
#include <cstdlib>

namespace helium{

  /* much of this code comes from http://tombarta.wordpress.com/2008/08/01/c-stack-traces-with-gcc/ */

void printTrace(int toIgnore,std::ostream &out)
{
    const size_t max_depth = 100;
    size_t stack_depth;
    void *stack_addrs[max_depth];
    char **stack_strings;

    stack_depth = backtrace(stack_addrs, max_depth);
    stack_strings = backtrace_symbols(stack_addrs, stack_depth);

    out<<"Call stack"<<std::endl;

    for (size_t i = toIgnore; i < stack_depth; i++) {
      //out<<"**"<<stack_strings[i]<<endl;
      size_t sz = 200; // just a guess, template names will go much wider
    char *function = static_cast<char*>(malloc(sz));
    char *begin = 0, *end = 0;
    // find the parentheses and address offset surrounding the mangled name
    for (char *j = stack_strings[i]; *j; ++j) {
        if (*j == '(') {
            begin = j;
        }
        else if (*j == '+') {
            end = j;
        }
    }
    if (begin && end) {
        *begin++ = 0;
        *end = 0;
        // found our mangled name, now in [begin, end)

        int status;
        char *ret = abi::__cxa_demangle(begin, function, &sz, &status);
        if (ret) {
            // return value may be a realloc() of the input
            function = ret;
        }
        else {
            // demangling failed, just pretend it's a C function with no args
            std::strncpy(function, begin, sz);
            std::strncat(function, "()", sz);
            function[sz-1] = 0;
        }
	out<<"    "<<stack_strings[i]<<":"<<function<<std::endl;
    }
    else
    {
        // didn't find the mangled name, just print the whole line
      out<<"    "<<stack_strings[i]<<std::endl;
    }
    free(function);
    }
    


    free(stack_strings); // malloc()ed by backtrace_symbols
    out<<std::flush;
}

#endif //!win32
}//helium
#endif //ifdef tracedebug

