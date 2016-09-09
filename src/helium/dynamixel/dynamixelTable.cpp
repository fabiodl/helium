/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/dynamixel/dynamixelTable.h>

namespace helium{

  namespace dynamixel{
    const char Prop::size[]={
      2,//MODELNUMBERL=0x00,
      0,//MODELNUMBERH=0x01,
      1,//FIRMWAREVERSION=0x02,
      1,//MOTORID=0x03,
      1,//MOTORBAUDRATE=0x04,
      1,//RETURNDELAYTIME=0x05,
      2,//CWANGLELIMITL=0x06,
      0,//CWANGLELIMITH=0x07,
      2,//CCWANGLELIMITL=0x08,
      0,//CCWANGLELIMITH=0x09,
      -1,//0x0A
      1,//TEMPLIMIT=0x0B,
      1,//MINVOLTLIMIT=0x0C,
      1,//MAXVOLTLIMIT=0x0D,
      2,//MAXTORQUEL=0x0E,
      0,//MAXTORQUEH=0x0F,
      1,//STATUSRETURNLEVEL=0x10,
      1,//ALARMLED=0x11,
      1,//ALARMSHUTDOWN=0x12,
      //RAM
      -1,//0x13
      -1,//0x14
      -1,//0x15
      -1,//0x16
      -1,//0x17      
      1,//TORQUEENABLE=0x18,
      1,//LED=0x19,
      1,//GAIND=0x1A,
      1,//GAINI=0x1B,
      1,//GAINP=0x1C,
      -1,//0x1D
      2,//TARGETPOSL=0x1E,
      0,//TARGETPOSH=0x1F,
      2,//SPEEDL=0x20,
      0,//SPEEDH=0x21,
      2,//TORQUELIMITL=0x22,
      0,//TORQUELIMITH=0x23,
      2,//M_POSL=0x24,
      0,//M_POSH=0x25,
      2,//M_SPEEDL=0x26,
      0,//M_SPEEDH=0x27,
      2,//M_LOADL=0x28,
      0,//M_LOADH=0x29,
      1,//M_VOLT=0x2A,
      1,//M_TEMP=0x2B,
      1,//INSTREGISTERED=0x2C,
      -1,//0x2D
      1,//MOVING=0x2E,
      1,//EEPROMLOCK=0x2F,
      2,//PUNCHL=0x30,
      0,//PUNCHH=0x31,
      -1,//0x32
      -1,//0x33
      -1,//0x34
      -1,//0x35
      -1,//0x36
      -1,//0x37
      -1,//0x38
      -1,//0x39
      -1,//0x3A
      -1,//0x3B
      -1,//0x3C
      -1,//0x3D
      -1,//0x3E
      -1,//0x3F
      -1,//0x40
      -1,//0x41
      -1,//0x42
      -1,//0x43
      -1,//0x44
      -1,//0x45
      -1,//0x46
      -1,//0x47
      -1,//0x48
      1,//ACCELERATION=0x49
      -2//all the table
    };//size
      

    const char* Prop::shortName[]={
      "MODEL_N",//MODELNUMBERL=0x00,
      0,//MODELNUMBERH=0x01,
      "FIRMWRV",//FIRMWAREVERSION=0x02,
      "ID     ",//MOTORID=0x03,
      "BAUD   ",//MOTORBAUDRATE=0x04,
      "RETDELT",//RETURNDELAYTIME=0x05,
      "MINLIM ",//CWANGLELIMITL=0x06,
      0,//CWANGLELIMITH=0x07,
      "MAXLIM ",//CCWANGLELIMITL=0x08,
      0,//CCWANGLELIMITH=0x09,
      0,//0x0A
      "TEMPLIM",//TEMPLIMIT=0x0B,
      "MINVOLT",//MINVOLTLIMIT=0x0C,
      "MAXVOLT",//MAXVOLTLIMIT=0x0D,
      "MAXTORQ",//MAXTORQUEL=0x0E,
      0,//MAXTORQUEH=0x0F,
      "STATRET",//STATUSRETURNLEVEL=0x10,
      "ALM_LED",//ALARMLED=0x11,
      "ALM_SHT",//ALARMSHUTDOWN=0x12,
      //RAM
      0,//0x13
      0,//0x14
      0,//0x15
      0,//0x16
      0,//0x17      
      "TOR_ENA",//TORQUEENABLE=0x18,
      "LED    ",//LED=0x19,
      "GAIN D ",//GAIND=0x1A,
      "GAIN I ",//GAINI=0x1B,
      "GAIN P ",//GAINP=0x1C,
      0,//0x1D
      "TARGET ",//TARGETPOSL=0x1E,
      0,//TARGETPOSH=0x1F,
      "SPEED  ",//SPEEDL=0x20,
      0,//SPEEDH=0x21,
      "TORQLIM",//TORQUELIMITL=0x22,
      0,//TORQUELIMITH=0x23,
      "M_POS  ",//M_POSL=0x24,
      0,//M_POSH=0x25,
      "M_SPEED",//M_SPEEDL=0x26,
      0,//M_SPEEDH=0x27,
      "M_LOAD ",//M_LOADL=0x28,
      0,//M_LOADH=0x29,
      "M_VOLT ",//M_VOLT=0x2A,
      "M_TEMP ",//M_TEMP=0x2B,
      "INSTREG",//INSTREGISTERED=0x2C,
      0,//0x2D
      "MOVING ",//MOVING=0x2E,
      "EPROLCK",//EEPROMLOCK=0x2F,
      "PUNCH  ",//PUNCHL=0x30,
      0,//PUNCHH=0x31,
      0,//0x32
      0,//0x33
      0,//0x34
      0,//0x35
      0,//0x36
      0,//0x37
      0,//0x38
      0,//0x39
      0,//0x3A
      0,//0x3B
      0,//0x3C
      0,//0x3D
      0,//0x3E
      0,//0x3F
      0,//0x40
      0,//0x41
      0,//0x42
      0,//0x43
      0,//0x44
      0,//0x45
      0,//0x46
      0,//0x47
      0,//0x48
      "ACCELER",//ACCELERATION=0x49
      0//all the table
    };//shortName


    
    
    int AddrTable[motor::PROPERTIESNUM]={dynamixel::TARGETPOS,dynamixel::M_POS,dynamixel::TORQUEENABLE,dynamixel::M_TEMP,-1};
    
    
    int getAddr(const motor::PropertyRequest& r){    
      if (r.id==motor::RAWPROPERTIES){
	return r.idx;
      }
      if (r.id>=motor::PROPERTIESNUM){
	throw exc::InvalidOperation("Dynamixel does not support property "+toString(r.id));
      }
      return AddrTable[r.id];	
    }    
  



    
  }//ns dynamixel




}//ns helium
