/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags g
 */


#ifndef HE_VALUEHUB
#define HE_VALUEHUB


#include <helium/signal/signal.h>
#include <helium/destroy/deleteList.h>
#include <iostream>

namespace helium{

  template <typename T>
  class ValueHub{   
  protected:
  T data;

  Signal<T> sig;
  //DeleteList<Callback<T> > del;
public:

  ValueHub(const ValueHub& s):data(s.data){
  }

  ValueHub(){
  }

  ValueHub(const T& v):data(v){
  }
  
  void set(const T& t){
    data=t;
    sig(data);
  }

  void signalCache(){
    sig(data);
  }

  void setCache(const T& t) {
    data=t;
  }


  ValueHub& operator=(const T& t){
    set(t);
    return *this;
  }


  ValueHub& operator=(const ValueHub& t){
    set(t.data);
    return *this;
  }


  bool isToSet(const T& t){
    return t!=data;
  }

  void assure(const T& t){
    if (isToSet(t)){
      data=t;
      sig(data);
    }
  }


  void setExcept(const T& t,const ConnReference<T>& noExec){
    data=t;
    sig(data,noExec.cb);    
  }


  void assureExcept(const T& t,const ConnReference<T>& noExec){
    if (isToSet(t)){
      data=t;
      sig(data,noExec.cb);
    }
  }


  void setExcept(const T& t,const Callback<T>* cb){
    data=t;
    sig(data,cb);    
  }


  void assureExcept(const T& t,const Callback<T>* cb){
    if (isToSet(t)){
      data=t;
      sig(data,cb);
    }
  }



  const T& get()const{
    return data;
  }

  operator const T&() const{
    return data;
  }
  

  void connect(ConnReference<T>& c){
    sig.connect(c);
  }

  void connect(Callback<T>* c){
    sig.connect(c);
  }



  void disconnect(ConnReference<T>& c){
    sig.disconnect(c);
  }


};

  template <typename T>
  std::ostream& operator<<(std::ostream& o, const ValueHub<T>& v){
    return  o<<v.get();
  }

  template <typename T>
  std::istream& operator>>(std::istream& i, ValueHub<T>& v){
    T t;
    i>>t;
    v.set(t);
    return i;
  }


}//helium



#endif
