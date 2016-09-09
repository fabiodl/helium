/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_COPYASNEW_
#define HE_COPYASNEW_


namespace helium{

template<typename T>
class CopyAsNew:public T{
public:
  CopyAsNew(){};
  CopyAsNew(const CopyAsNew&):T(){}
  CopyAsNew& operator=(const CopyAsNew&){return *this;}
  
};

}
#endif
