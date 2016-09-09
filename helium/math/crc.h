/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */
#ifndef HE_CRC
#define HE_CRC

#include <stdint.h>

namespace helium{
  
  namespace crc8maxim{
    void crcPush(unsigned char& crc, unsigned char crc_data);
    unsigned char getCrc(const unsigned char* buffer,int n,unsigned char startCrc=0);
    unsigned char getCrc(unsigned char crc);
  }

  namespace crc16ccitt{
    void crcPush(uint16_t& crc, unsigned char data);
    uint16_t getCrc(const unsigned char* buffer,int n,uint16_t startCrc=0xFFFF);
    uint16_t getCrc(unsigned char crc);
  }

  namespace crc16ibm{
    uint16_t getCrc(const uint8_t* buffer,int n,uint16_t startCrc=0);
  }
}//ns helium
  

#endif
