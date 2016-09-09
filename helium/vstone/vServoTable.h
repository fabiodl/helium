/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */
#ifndef HE_VSERVOTABLE
#define HE_VSERVOTABLE

#include <helium/robot/motorInterface.h>
#include <helium/vstone/vServoSign.h>

namespace helium{
  
  namespace vservo{
    enum VServoProps{   
    SYS_PN=0x00,  /* R     system product number   */
    SYS_VER=0x02,  /* R     system version          */
    SYS_UID=0x04,  /* R     system unique ID        */
    SYS_SID=0x08,  /* R/W   system short ID         */
    SYS_RST=0x09,  /* R/W   system reset/FUPD       */
    SYS_BR=0x0a,  /* R/W   system baudrate         */
    SYS_T0=0x0c,  /* R/W   feedback period         */
    SYS_RID=0x0e, /*	1 R/W	SID randomize		*/
    
    SYS_PCT=0x10,  /* R/W   feedback vd_n           */
    SYS_DCT=0x11,  /* R/W   feedback vd_n           */
    SYS_ECT=0x12,  /* R/W   feedback ve_n           */
    SYS_MOD=0x13, /*	1 R/W	mode flag(b4:free-rot)  */
    /*				b5:enable BST_CHK */
    /*                              b6:rom high page(from 0x4000)*/
    
    SYS_ULK=0x14, /*	1 R/W	unlock(0x55) ram&flash  */
    
    PWM_PSN=0x18,  /* R/W   PWM prescale normal     */
    PWM_PSA=0x19,  /* R/W   PWM prescale alarm      */
    PWM_SST=0x1a,  /* R/W   PWM soft start time    */
    PWM_SLP=0x1c,  /* R/W   PWM output slope limit    */
    
    
    M_POS=0x20,  /* R     measured pos            */
    M_SPD=0x22,  /* R     calced   spd            */
    M_VE=0x24,  /* R     measured VE             */
    M_TEMP=0x26,  /* R     measured temp           */
    M_VI=0x28,  /* R     measured VI             */
    M_IERR=0x2a,  /* R     calced   IERR           */
    
    FB_TPOS=0x30,  /* R/W   feedback target pos     */
    FB_PG=0x32,  /* R/W   feedback Pgain          */
    FB_DG=0x33,  /* R/W   feedback Dgain          */
      FB_EG=0x34,  /* R/W   feedback Egain          */
    FB_IG=0x35,  /* R/W   feedback Igain          */
    FB_ILIM=0x36,  /* R/W   feedback ierr limit     */
    FB_PDB=0x38,  /* R/W   feedback pos deadband   */
    FB_DDB=0x39,  /* R/W   feedback spd deadband   */
    FB_EDB=0x3a,  /* R/W   feedback VE  deadband   */
    PWM_OFF=0x3b,  /* R/W   PWM off (free)          */
    PWM_LIM=0x3c,  /* R/W   PWM limit (maxduty)     */
    PWM_PCH=0x3d,  /* R/W   PWM punch               */
    PWM_OUT=0x3e,  /* R     PWM output              */
    
    AL_TEMP=0x40,  /* R/W   temp alarm              */
    SD_TEMP=0x42,  /* R/W   temp shutdown           */
    AL_VI=0x44,  /* R/W   VI alarm                */
    SD_VI=0x46,  /* R/W   VI shutdown             */
    
    BST_DUM=0x4d,  /* R/W   dummy for burst write   */
    BST_LEN=0x4e,  /* R/W   dummy for burst write   */
    BST_SYN=0x4f,  /* R/W   burst sync              */
    BST_WA0=0x50,  /* R/W   burst write adr #0      */
    BST_WA1=0x51,  /* R/W   burst write adr #1      */
    BST_WA2=0x52,  /* R/W   burst write adr #2      */
    BST_WA3=0x53,  /* R/W   burst write adr #3      */
    BST_WA4=0x54,  /* R/W   burst write adr #4      */
    BST_WA5=0x55,  /* R/W   burst write adr #5      */
    BST_WA6=0x56,  /* R/W   burst write adr #6      */
    BST_WA7=0x57,  /* R/W   burst write adr #7      */
    BST_RA0=0x58,  /* R/W   burst read  adr #0      */
    BST_RA1=0x59,  /* R/W   burst read  adr #1      */
    BST_RA2=0x5a,  /* R/W   burst read  adr #2      */
    BST_RA3=0x5b,  /* R/W   burst read  adr #3      */
    BST_RA4=0x5c,  /* R/W   burst read  adr #4      */
    BST_RA5=0x5d,  /* R/W   burst read  adr #5      */
      BST_RA6=0x5e,  /* R/W   burst read  adr #6      */
    BST_RA7=0x5f,  /* R/W   burst read  adr #7      */

    /* Fabio extensions */
    CALIBMIN=0x60,
    CALIBMAX=0x62,
    };

    struct Prop{
      static const char size[];
    };

    double getTemperature(int val);
    int getAddr(const motor::PropertyRequest& r);


    class PropNormalize:public motor::Raw<VServoSign>::PropNormalizer{
      motor::Raw<VServoSign>::PosNormalizer& posNorm;
      motor::Interface::Property normProp(int value,const motor::PropertyRequest& r);
      motor::Raw<VServoSign>::Property unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r);
    public:
      PropNormalize(motor::Raw<VServoSign>::PosNormalizer& pposNorm):posNorm(pposNorm){}
    };


  }


}





#endif
