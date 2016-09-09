/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_BIT
#define HE_BIT

namespace helium{

  template<int maxN,typename T> 
  inline int numOfTrueBits(T& t){
    int c=0;
    for (int i=0;i<maxN;i++){
      if (t&(1<<i)){
	c++;
      }
    }
    return c;
  }
  
  template<typename T> void setBit(T& t,int bit,bool value){
    t= (t&~(1<<bit))|(value<<bit);
  }


  template<int N,typename T> unsigned char getByte(T& t){
    return 0xFF&(t>>(8*N));
  }

  template<int N,typename T> void setByte(T& t,unsigned char val){
    return t=(t&~(0xFF<<(8*N)))|(val<<(8*N));
  }

};




#endif
