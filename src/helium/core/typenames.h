/*Fabio Dalla Libera   sourceforge.net/projects/libhelium/  */

#ifndef HE_TYPENAMES_
#define HE_TYPENAMES_
#include <string>
#include <typeinfo>
#include <sstream>
namespace helium{


  template <typename T> class TypeName{
  public:
   
    static std::string getName(){
      return typeid(T).name();
    }
    
  };



  /*template <> class TypeName<int>{
  public:
    static std::string getName(){
      return "int";
    }
  };

  template <> class TypeName<double>{
  public:
    static std::string getName(){
      return "double";
    }
  };

  template <> class TypeName<char>{
  public:
    static std::string getName(){
      return "char";
    }
  };

  template <> class TypeName<std::string>{
  public:
    static std::string getName(){
      return "std::string";
    }
    };*/

  template <typename T>  inline std::string getTypeName(const T &t){
    return TypeName<T>::getName();
  }

  template <typename T>  inline std::string getTypeName(){
    return TypeName<T>::getName();
  }

    
  /*inline std::string getTypeName(char){
    return "char";
  }

  inline std::string getTypeName(int){
    return "int";
  }

  inline std::string getTypeName(double){
    return "double";
  }

  inline std::string getTypeName(const std::string&){
    return "string";
  }

  template <typename T>  inline std::string getTypeName(const T &){
    return "unkwown type";
    }*/

};
#endif
