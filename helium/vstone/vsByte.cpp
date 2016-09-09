/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/vstone/vsByte.h>

namespace helium{

 void to14(unsigned char* data,int val){
    data[0]=val&0x7F;
    data[1]=(val>>7)&0x7F;
  }

  void from14(int& val,const unsigned char* data){
    val=(data[0]&0x7F)|((data[1]&0x7F)<<7);
  }

  int from14(const unsigned char* data){
    return (data[0]&0x7F)|((data[1]&0x7F)<<7);
  }


  void to28(unsigned char* data,int val){
    data[0]=val&0x7F;
    data[1]=(val>>7)&0x7F;
    data[2]=(val>>14)&0x7F;
    data[3]=(val>>21)&0x7F;
      
  }

  void from28(int& val,const unsigned char* data){
    val=(data[0]&0x7F)|((data[1]&0x7F)<<7)|((data[2]&0x7F)<<14)|((data[3]&0x7F)<<21) ;
  }

  int from28(const unsigned char* data){
    return (data[0]&0x7F)|((data[1]&0x7F)<<7)|((data[2]&0x7F)<<14)|((data[3]&0x7F)<<21) ;
  }


  void fromLE(int& val,const unsigned char* data){
    val=((data[1]<<8)&0xff00) | (data[0]&0xff);
  }

}
