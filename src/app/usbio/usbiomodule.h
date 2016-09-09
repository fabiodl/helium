#include <stdint.h>
#ifndef _USBIOMODULE_H

#ifdef __cplusplus
extern "C"{
#endif 
  void* new_usbio();
  void delete_usbio(void*);

  int setDirection(void*,uint8_t port,uint8_t value);
  int setOutput(void*,uint8_t port,uint8_t value);

#ifdef __cplusplus
}
#endif


#endif
