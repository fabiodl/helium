/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_BASICENUM
#define HE_BASICENUM

#include <helium/core/enum.h>
namespace helium{
  
  
  /*struct Bool{
    typedef enum {F,T} Type; 
    static const int NUM=2;
    static const char* strings[];
    };*/
  
  
  namespace Axis{
    enum Type {X,Y,Z,NEGX,NEGY,NEGZ};     
  };

  template<>  struct EnumStrings<Axis::Type>{
  static const int NUM=6;
  static const char* strings[NUM];       
};

  namespace Rotation{
     enum Type {YAW,PITCH,ROLL}; 
  };

  template<>  struct EnumStrings<Rotation::Type>{
  static const int NUM=3;
  static const char* strings[NUM];       
};

  


}


#endif
