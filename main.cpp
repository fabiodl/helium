#include <helium/os/usbHid.h>
#include <stdlib.h>
#include "megankro.h"


int main(int argc,char** argv){
  
  if (argc<static_cast<int>(MegaNkro::NUMKEYS+1)){
    std::cout<<"Usage "<<argv[0]<<" followed by "<<MegaNkro::NUMKEYS<<" hex values, according to Universal Serial Bus HID Usage Tables, Table 12: Keyboard/Keypad Page "<<std::endl;
    return 0;
  }
  
 
  uint8_t keys[MegaNkro::NUMKEYS];
  for (size_t i=0;i<MegaNkro::NUMKEYS;i++){
    keys[i]=static_cast<uint8_t>(strtol(argv[i+1],NULL,16));
    std::cout<<"key "<<(int)keys[i]<<std::endl;
  }
  MegaNkro nkro;
  nkro.set(keys);


}
