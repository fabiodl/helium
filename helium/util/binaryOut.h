#ifndef _HE_BINARYOUT_
#define _HE_BINARYOUT_

#include <fstream>
#include <vector>
#include <helium/util/ioUtil.h>

namespace helium{


  class BinaryOut{

  public:
    std::ofstream f;
    
    BinaryOut(const std::string& fname):
      f(fname.c_str(),std::ios::binary)
    {
      
    }
  
    template<typename T>
    BinaryOut& operator<<(const T& t){
      f.write(reinterpret_cast<const char*>(&t),sizeof(T));
      return *this;
    }
  
    template<typename T,typename A>
    BinaryOut& operator<<(const std::vector<T,A>& t){
      for (typename std::vector<T,A>::const_iterator it=t.begin();
	   it!=t.end();++it){
	(*this)<<(*it);
      }      
      return *this;
    }
    
    



    BinaryOut& operator<<(std::ostream& endl (std::ostream& os)){
      std::cout<<"you got endl,"<<std::endl;
      return *this;
    }





  };



  
  class AsciiOut{

  public:
    std::ofstream f;
    
    AsciiOut(const std::string& fname):
      f(fname.c_str())
    {      
    }
  
    template<typename T>
    AsciiOut& operator<<(const T& t){
      print<T>(t,f);
      return *this;
    }
  
    AsciiOut& operator<<(std::ostream& endl (std::ostream& os)){
      endl(f);
      return *this;
    }





  };





  template<typename T> BinaryOut& print(const T& t,BinaryOut& o){
    return o<<t;
  }
  


}



#endif
