/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_VALUEWAITER
#define HE_VALUEWAITER

#include <helium/thread/semaphore.h>

namespace helium{


  template<typename T> struct toReference{
    inline static T& get(T* t){
      return *t;
    }
    
    inline static const T& get(const T* t){
      return *t;
    }
    
    inline static T& get(T& t){
      return t;
    }
    
    inline static const T& get(const T& t){
      return t;
    }

    template<typename U>
    inline static T& get(U* u){
      return *trust_cast<T*>(u);
    }

    template<typename U>
    inline static const T& get(const U* u){
      return *trust_cast<T*>(u);
    }
  };

  //waits someone to put a val



  template<typename T,bool EH,typename TC=T> class ValueWaiter:public Callback<T>{
    typename SemaphoreType<false>::value s;
    TC* c;

    void operator()(T t){
      *c=toReference<TC>::get(t);
      s.signal();
    }

  public:
    
    ValueWaiter(TC* pc):c(pc){
    };
   

    ValueWaiter():c(NULL){
    };

    void setResultDst(TC* pc){
      c=pc;      
    }

    void wait(){
      s.wait();
    }    
  };


  template<typename T,typename TC> class ValueWaiter<T,true,TC>:public Callback<T>,public exc::ExceptionHandler{
    typename SemaphoreType<true>::value s;
    TC* c;

    void operator()(T t){
      *c=toReference<TC>::get(t);
      s.signal();
    }

  public:
    
    ValueWaiter(TC* pc):c(pc){
    };
   

    ValueWaiter():c(NULL){
    };

    void setResultDst(TC* pc){
      c=pc;      
    }

    void wait(){
      s.wait();
    }    

    void handle(exc::Exception& e){
      s.handle(e);
    }

  };




};//helium

#endif
