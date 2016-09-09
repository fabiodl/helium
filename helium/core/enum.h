/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */


#ifndef HE_ENUM
#define HE_ENUM
#include <iostream>


namespace helium{


  
template<typename E>  struct EnumStrings{
  static const int NUM=0;
  //static const char* strings[NUM];       
};
  
#define DECLARE_ENUM(t,n) template<> struct EnumStrings<t >{static const int NUM= n ;static const char* strings[NUM]; };

#define DEFINE_ENUM(t) const char* EnumStrings<t >::strings[EnumStrings<t >::NUM]


  //OLD ENUM
  /*EXAMPLE 
struct Orientation{
public:
typedef enum {RIGHT,LEFT,NUM} Type; //an enum vals,ending with NUM
static const char* strings[];       //a static array of strings
};

const char* Orientation::strings[]={"right","left"};

helium::Enum<helium::Orientation> types = helium::Orientation::RIGHT;
cout << types  << endl;               // "right"
*/
 template <typename E>
struct Enum{ 
  typedef typename E::Type Type;
  typename E::Type val;
  Enum(){}
  Enum(Type v):val(v){}
};


template<typename E>
std::istream& operator>>(std::istream& is,Enum<E>& e){
  std::string s;
  is>>s;
  for (int i=0;i<E::NUM;i++){
    if (E::strings[i]==s){
      e.val=(typename E::Type)i;
      return is;
    }
  }
  throw 7;
}


template<typename E>
std::ostream& operator<<(std::ostream& os,const Enum<E>& e){
  return os<<E::strings[e.val];
}
  

}//helium
#endif
