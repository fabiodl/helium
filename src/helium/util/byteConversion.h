/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_BYTECONVERSION
#define HE_BYTECONVERSION



namespace helium{

  inline int fromLE(const unsigned char* data){
    return data[0]+(data[1]<<8);
  }

  inline void toLE(unsigned char* data,int v){
    data[0]=v&0xFF;
    data[1]=(v>>8)&0xFF;
  }


  class BytePair{
    unsigned char data[2];
  public:  
    operator const unsigned char*(){
      return data;
    }
    
    template<typename A,typename B> BytePair(A a,B b){
      data[0]=a;
      data[1]=b;
    }
    
  };

  
}

#endif
