/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SIGNALLEDVALUE
#define HE_SIGNALLEDVALUE

#include <helium/system/cell.h>

namespace helium{
  
  template<typename T> class SignalledValue:public Signal<Cell::CellSignalData>{
    T val;
  public:

    SignalledValue& operator=(const T& t){
      val=t;
      (*this)(&val);
      return *this;
    }       

    bool operator==(const T& t){
      return t==val;
    }

    void assure(const T& t){
      if (val==t) return;
      val=t;
      (*this)(&val);
    }

    operator const T&(){return val;}

    const T& get(){
      return val;
    }

    T& getDirectReference(){
      return val;
    }

    void signalUpdate(){
      (*this)(&val);
    }

    SignalledValue(const T& pval):val(pval){
    }
    SignalledValue(){
    }

  };


};



#endif
