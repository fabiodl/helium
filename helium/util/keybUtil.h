#ifndef HE_KEYBUTIL
#define HE_KEYBUTIL
#include <cstdio>

namespace helium{

  inline char waitChar(){
    char c[2];//needs the terminator
    if (fgets(c, 2, stdin)==NULL) return -1;
    return c[0];
  }
}

#endif
