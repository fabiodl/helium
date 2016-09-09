/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_PRINTHEX
#define HE_PRINTHEX
#include <iostream>
namespace helium{

  
  void printHex(const unsigned char* c,int n,std::ostream& out=std::cout);
  template<typename T> void printHex(const T& t,std::ostream& out=std::cout){
    printHex(t,t.size());
  }
  void printHexTable(const unsigned char* c,int n,std::ostream& out=std::cout);


  void printBinLSBfirst(unsigned char c,int n,std::ostream& out=std::cout);

  struct asHexString{
    const unsigned char* s;
    int n;
    asHexString(const unsigned char* ps,int pn):s(ps),n(pn){}

  };

  struct asHex{
    int x;
    template<typename T> asHex(const T& px):x(px){}    
  };
  

  inline  std::ostream& operator<<(std::ostream& o,const asHexString& hs){
    printHex(hs.s,hs.n,o);
    return o;
  }


  inline  std::ostream& operator<<(std::ostream& o,const asHex& hs){
    return o<<std::hex<<hs.x<<std::dec;
  }




  template<typename T>
  void rawHexWrite(std::ostream& o,const T& t);
  
  
  template<typename T>
  void rawHexRead(T& t,std::istream& i);
 

}

#include <helium/util/printHex.hpp>
#endif
