/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

namespace helium{

  namespace vsTs{
    enum TouchSensorProps{
      M_VIN0 =0x20, // 2 R		measured vin0
      M_VIN1 =0x22, // 2 R		measured vin1
       M_VIN2 =0x24, // 2 R		measured vin2
       M_VIN3 =0x26, // 2 R		measured vin3
       M_VIN4 =0x28, // 2 R		measured vin4
       M_VIN5 =0x2a, // 2 R		measured vin5
       M_VIN6 =0x2c, // 2 R		measured vin6
       M_VIN7 =0x2e, // 2 R		measured vin7
      
      /*fabio extensions*/
      
       CHAN_0_FLAGS =0x30,
       CHAN_1_FLAGS =0x31,
       CHAN_2_FLAGS =0x32,
       CHAN_3_FLAGS =0x33,
       CHAN_4_FLAGS =0x34,
       CHAN_5_FLAGS =0x35,
       CHAN_6_FLAGS =0x36,
       CHAN_7_FLAGS =0x37,
      
       LED_0_DUTY =0x38,
       LED_1_DUTY =0x39,
       LED_2_DUTY =0x3a,
       LED_3_DUTY =0x3b,
       LED_4_DUTY =0x3c,
       LED_5_DUTY =0x3d,
       LED_6_DUTY =0x3e,
       LED_7_DUTY =0x3f  
    };
    
    enum TouchSensorFlags{
      IS_BURST_READ=1,
      IS_BURST_WRITE=2,
      IS_LED=4
    };
    
    enum HwProps{
       CHANNELS=8
    };

    enum Mode{SENSORMODE,LEDMODE};
  }//ns tSens




}//ns helium
