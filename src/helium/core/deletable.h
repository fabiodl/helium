/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_DELETABLE
#define HE_DELETABLE
namespace helium{
  struct Deletable{
    virtual ~Deletable(){};
  };
  
  template<typename T>
  struct DeletableData:public Deletable{
    T data;
    virtual ~DeletableData(){};
    DeletableData(){}
    DeletableData(const T& pdata):data(pdata){}
  };
 


}
#endif
