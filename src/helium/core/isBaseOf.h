/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_ISBASEOF_
#define HE_ISBASEOF_

#include <helium/core/isSame.h>
#include <helium/core/yesNo.h>
#include <helium/core/isSame.h>

/*

code from
http://stackoverflow.com/questions/2910979/how-is-base-of-works

*/
namespace helium{


template <typename B, typename D>
struct Host
{
  operator B*() const;
  operator D*();
};

template <typename B, typename D>
struct is_base_of
{
  template <typename T> 
  static tt::yes check(D*, T);
  static tt::no check(B*, int);

  static const bool value = sizeof(check(Host<B,D>(), int())) == sizeof(tt::yes);
};

template <typename B> 
   struct getObject{
     static B* get();
   };

  /*template <typename A, typename B>
  struct is_same_of
  {
    static yes check(A*);
    template <typename T>
    static no check(T);
    static const bool value = sizeof(check(getObject<B>::get())) == sizeof(yes);
  };
  */


template <typename B, typename D>
  struct is_base_or_same_of
  {
    static const bool value= isSame<B,D>::value||is_base_of<B,D>::value;
  };



/*

  This does not work with D a private B.


  template<typename B,typename D> struct is_base_or_same_of{

  typedef char (&yes)[2] ;  
  
  static yes test(const B* b);
  static char test(const void* p);

  static const D* d();

  static const bool value=sizeof(test(d()))==sizeof(yes);

  };*/


}//helium
#endif
