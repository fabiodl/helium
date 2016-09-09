/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#include <iostream> 
#include <stdio.h>
#include <sstream>


#include <stdint.h>

namespace helium{

template<int s,bool single= ((s==1)||(s==2)||(s==4)||(s==8)) > struct RawHexIO;


template<int s>
struct IntSizeAdapter{

};

template<>
struct IntSizeAdapter<1>{
  typedef uint8_t value_type;
  static const char* format;
};


template<>
struct IntSizeAdapter<2>{
  typedef uint16_t value_type;
  static const char* format;
};


template<>
struct IntSizeAdapter<4>{
  typedef uint32_t value_type;
  static const char* format;
};


template<>
struct IntSizeAdapter<8>{
  typedef uint64_t value_type;
  static const char* format;
};
  
  template<int s>
  struct RawHexIO<s,true>{
    template<typename T>
    static void write(std::ostream& o,const T& t){
      char buffer[2*s+1];
      sprintf(buffer,IntSizeAdapter<s>::format,*(const typename IntSizeAdapter<s>::value_type*)(&t));
      o<<buffer;
    }
    
    template<typename T>
    static void read(T& t,std::istream& i){
      std::ios::fmtflags f( i.flags() );
      i>>std::hex>>*((typename IntSizeAdapter<s>::value_type*)(&t));
      i.flags(f);
    }
    
  };
  

  template<typename T>
  void rawHexWrite(std::ostream& o,const T& t){
    RawHexIO<sizeof(T)>::write(o,t);
  }
  
  
  template<typename T>
  void rawHexRead(T& t,std::istream& i){
    RawHexIO<sizeof(T)>::read(t,i);
  }
  


  
}//ns he
