#ifndef HE_HEX
#define HE_HEX


namespace helium{
  struct Hex{
    int val;    
    int& operator=(const int &i){
      val=i;
      return val; //fb 20110917 was return &val;
    }
    Hex():val(-1){
    }
    Hex(const int& x):val(x){
    }
    operator int&(){
      return val;
    }
  };
  

  inline std::ostream& operator<<(std::ostream& o,const Hex& x){
    return o<<std::hex<<x.val<<std::dec;
  }
  inline std::istream& operator>>(std::istream& i,Hex& x){
    return i>>std::hex>>x.val>>std::dec;
  }
}

#endif
