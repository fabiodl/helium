/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/util/printHex.h>

namespace helium{
  
void printHex(const unsigned char* c,int n,std::ostream& out){
  out<<std::hex;
  for (int i=0;i<n;++i){
    out<<(int)(unsigned char)c[i]<<" ";
  }
  out<<std::dec;
}


void printHexTable(const unsigned char* c,int n,std::ostream& out){
  out<<std::hex;
  int cnt=0;
  for (int i=0;i<n;++i){
    out<<(c[i]<0x10?"0":"")<<(int)(unsigned char)c[i]<<" ";
    cnt=(cnt+1)%16;
    if (cnt==8) out<<" ";
    else if (!cnt) out<<std::endl;
  }
  out<<std::dec;
}

 void printBinLSBfirst(unsigned char c,int n,std::ostream& out){
    for (int i=0;i<n;i++){
      out<< (c&(1<<i)?1:0)<<" ";
    }
 }

  const char* IntSizeAdapter<1>::format="%02x";
  const char* IntSizeAdapter<2>::format="%04x";
  const char* IntSizeAdapter<4>::format="%08llX";
  const char* IntSizeAdapter<8>::format="%016llx";

}
