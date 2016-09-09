/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_DYNAMIXELTABLE
#define HE_DYNAMIXELTABLE

#include <helium/robot/motorInterface.h>

namespace helium{

  namespace dynamixel{
    enum MemoryMap{
      //EEPROM
      MODELNUMBERL=0x00,
      MODELNUMBERH=0x01,
      FIRMWAREVERSION=0x02,
      MOTORID=0x03,
      MOTORBAUDRATE=0x04,
      RETURNDELAYTIME=0x05,
      CWANGLELIMITL=0x06,
      CWANGLELIMITH=0x07,
      CCWANGLELIMITL=0x08,
      CCWANGLELIMITH=0x09,
      TEMPLIMIT=0x0B,
      MINVOLTLIMIT=0x0C,
      MAXVOLTLIMIT=0x0D,
      MAXTORQUEL=0x0E,
      MAXTORQUEH=0x0F,
      STATUSRETURNLEVEL=0x10,
      ALARMLED=0x11,
      ALARMSHUTDOWN=0x12,
      //RAM
      TORQUEENABLE=0x18,
      LED=0x19,
      GAIND=0x1A,
      GAINI=0x1B,
      GAINP=0x1C,
      TARGETPOS=0x1E,
      TARGETPOSL=0x1E,
      TARGETPOSH=0x1F,
      SPEEDL=0x20,
      SPEEDH=0x21,
      TORQUELIMITL=0x22,
      TORQUELIMITH=0x23,
      M_POS=0x24,      
      M_POSL=0x24,
      M_POSH=0x25,
      M_SPEEDL=0x26,
      M_SPEEDH=0x27,
      M_LOADL=0x28,
      M_LOADH=0x29,
      M_VOLT=0x2A,
      M_TEMP=0x2B,
      INSTREGISTERED=0x2C,
      MOVING=0x2E,
      EEPROMLOCK=0x2F,
      PUNCHL=0x30,
      PUNCHH=0x31,
      ACCELERATION=0x49,
      TABLESIZE=0x4A
    };

    struct Prop{
      static const char size[];
      static const char* shortName[];
    };
    
    extern int getAddr(const motor::PropertyRequest& r);
    
    


  }//ns dynamixel


}//ns helium




#endif
