/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_VSBYTE
#define HE_VSBYTE

namespace helium{

  void to14(unsigned char* data,int val);
  void from14(int& val,const unsigned char* data);
  int from14(const unsigned char* data);
  void to28(unsigned char* data,int val);
  void from28(int& val,const unsigned char* data);
  int from28(const unsigned char* data);

  void fromLE(int& val,const unsigned char* data);



}



#endif
