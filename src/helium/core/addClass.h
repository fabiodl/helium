/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_ADDCLASS
#define HE_ADDCLASS

namespace helium{

  template<typename C,typename Add,bool b> struct addClass{
  };
  
  
  template<typename C,typename Add> struct addClass<C,Add,false>{
    typedef C value;
  };
  
  template<typename C,typename Add> struct addClass<C,Add,true>{
    class value:public C,public Add{
    };
  };

}

#endif
