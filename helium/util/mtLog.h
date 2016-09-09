/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags se
 */

#ifndef HE_MTLOG
#define HE_MTLOG

#include <helium/thread/mutex.h>
#include <helium/util/ioUtil.h>
#include <sstream>
namespace helium{



  template<typename MtLog,typename Out>
  class TRealLog{    
    MtLog& owner;
    Out& out;
  public:
    TRealLog(MtLog& powner,Out& pout):owner(powner),out(pout){
    }
      
    template<typename T>
    TRealLog& operator<<(const T& t){
      write(out,t);
      return *this;
    }
    
    TRealLog& operator<<(const std::stringstream& ss){
	out<<ss.str();
	return *this;
    }
    
    template<typename T>
    void operator()(const T& t){
      write(out,t);
      out<<std::endl;
      owner.acc.unlock();
    }
    
    MtLog& operator<<(std::ostream& (*f)(std::ostream&)){
      //std::cout<<"you pushed a modifier"<<std::endl;
      f(out);
      owner.acc.unlock();
      return owner;
    }
  };





  template<typename Out>
  class MtLog{
  public:
    typedef TRealLog<MtLog<Out>,Out> RealLog;
    
    
  private:
    RealLog rel;
    friend class TRealLog<MtLog<Out>,Out>;
    Mutex acc;
  public:

    void lock(){
      acc.lock();
    }

    void unlock(){
      acc.unlock();
    }
    
    template<typename T> RealLog& operator<<(const T& t){
      acc.lock();
      rel<<t;
      return rel;
    }
        
    MtLog(Out& out):rel(*this,out),acc("atomicLog"){}  
};

  inline std::ostream& endAtomic (std::ostream& o){
    return o;
  }

  template<typename Out,typename ToLock>
  struct LinkedLog{
  };


  //for making two logs use the same lock, e.g. std::out,std::err
  template<typename Out,typename ToLock>
  class MtLog<LinkedLog<Out,ToLock> >{
    
  public:
    typedef MtLog< LinkedLog<Out,ToLock>  > Me;
    typedef TRealLog<Me,Out > RealLog;
  private:
    RealLog rel;   
    friend  class TRealLog<Me,Out>; //cannot declare a typeded friend
    ToLock& acc;
  public:
    
    template<typename T> RealLog& operator<<(const T& t){
      acc.lock();
      rel<<t;
      return rel;
    }

    MtLog(Out& out,ToLock& toLock):rel(*this,out),acc(toLock){
    }


  };



};




#endif
