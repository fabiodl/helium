/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_TSVALUEHUB
#define HE_TSVALUEHUB


#include <helium/signal/signal.h>
#include <helium/destroy/deleteList.h>
#include <iostream>

namespace helium{

  template <typename T>
  class TsValueHub{   
  protected:

    Mutex m;
    TsSignal<T,true> sig;
    T data;
    

    //DeleteList<Callback<T> > del;
public:

    TsValueHub(const TsValueHub& s):m("TSVH"),sig(m),data(s.data){
    }
    
    TsValueHub():m("TSVH"),sig(m){
    }
    
    TsValueHub(const T& v):m("TSVH"),sig(&m),data(v){
    }
    
    void set(const T& t){
      m.lock();
      data=t;
      m.unlock();
      sig(data);
    }
    
    void signalCache(){
      m.lock();
      T t(data);
      m.unlock();
      sig(t);
    }

  void setCache(const T& t) {
    data=t;
  }

    template<typename X>
    TsValueHub& operator=(const X& t){
      set(t);
      return *this;
    }
    
   

  bool isToSet(const T& t){
    return t!=data;
  }

  void assure(const T& t){
    if (isToSet(t)){
      m.lock();
      data=t;
      m.unlock();
      sig(data);
    }
  }


  void setExcept(const T& t,const ConnReference<T>& noExec){
    m.lock();
    data=t;
    m.unlock();
    sig(data,noExec.cb);    
  }


  void assureExcept(const T& t,const ConnReference<T>& noExec){
    if (isToSet(t)){
      m.lock();
      data=t;
      m.unlock();
      sig(data,noExec.cb);
    }
  }


  void setExcept(const T& t,const Callback<T>* cb){
    m.lock();
    data=t;
    m.unlock();
    sig(data,cb);    
  }


  void assureExcept(const T& t,const Callback<T>* cb){
    if (isToSet(t)){
      m.lock();
      data=t;
      m.unlock();
      sig(data,cb);
    }
  }



  const T get(){
    m.lock();
    T d(data);
    m.unlock();
    return d;
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
  std::ostream& operator<<(std::ostream& o, const TsValueHub<T>& v){
    return  o<<v.get();
  }

  template <typename T>
  std::istream& operator>>(std::istream& i, TsValueHub<T>& v){
    T t;
    i>>t;
    v.set(t);
    return i;
  }


}//helium



#endif
