/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_EHSIGNAL
#define HE_EHSIGNAL

#include <helium/core/exception.h>
#include <helium/signal/signal.h>

namespace helium{


  template<typename D>
  class EhConnReference:public ConnReference<D>,public exc::ExceptionHandler{    
  public:
    virtual void handle(exc::Exception& e)=0;
    EhConnReference(Callback<D>* c):ConnReference<D>(c){
    }
  };


  template<typename D>
  class EhPtrConnReference:public EhConnReference<D>{    
    exc::ExceptionHandler* eh;
  public:
    virtual void handle(exc::Exception& e){
      eh->handle(e);
    }
    EhPtrConnReference(Callback<D>* c,exc::ExceptionHandler* peh):
      EhConnReference<D>(c),eh(peh){
    }
  };

  namespace support{
    template<typename D,typename Data,typename DiscInfo> struct dinfoDataExtracter{
      static inline  Data* get(EhConnReference<D>& r){
	return &(trust_cast<DiscInfo*>(r.disc)->data);
      }
    };
    template<typename D,typename DiscInfo> struct dinfoDataExtracter<D,void,DiscInfo>{
      static inline  void* get(EhConnReference<D>& r){return NULL;}
    };
  }//support

  template<typename D,bool TS=false,typename Data=void> class EhSignal:public Callback<D>,public exc::ExceptionHandler{ 
    EhSignal(const EhSignal& s);
    EhSignal& operator=(const EhSignal& s);
  protected:
    typedef std::list<EhConnReference<D>*> CEhList;
    CEhList cs;    
    typedef IterDiscInfo<D,typename CEhList::iterator,Data> DiscInfo;
  public:
    EhSignal(){}
    void connect(EhConnReference<D>& c){
      c.disc=new DiscInfo(cs.insert(cs.end(),&c));
    }

    void disconnect(EhConnReference<D> &r){
      cs.erase(trust_cast<DiscInfo*>(r.disc)->it);
      delete r.disc;
      r.disc=NULL;
    }
    
    static Data* getDataPtr(EhConnReference<D>& r){
      return support::dinfoDataExtracter<D,Data,DiscInfo>::get(r);
    }
    
    void operator()(D d){
      typename CEhList::iterator i;
      for (i=cs.begin();i!=cs.end();i++){
	(*((*i)->cb))(d);
      }
    }

    void handle(exc::Exception& e){
      typename CEhList::iterator i;
      for (i=cs.begin();i!=cs.end();i++){
	(*i)->handle(e);
      }
    }

    size_t listenerNum(){
      return cs.size();
    }
    
  };



  class ExceptionSignal:public exc::ExceptionHandler{ //ONLY exceptions,not a real signal
  protected:
    std::list<ExceptionHandler*> es;
    typedef IterDiscInfo<void,std::list<ExceptionHandler*>::iterator> DiscInfo;
  public:

    template<typename D> void connect(EhConnReference<D>& e){
      e.disc=new DiscInfo(es.insert(es.end(),&e));      
    }

    template<typename D> void disconnect(EhConnReference<D>& r){
      es.erase(trust_cast<DiscInfo*>(r.disc)->it);
      delete r.disc;
      r.disc=NULL;
    }

    void connect(exc::ExceptionHandler* e){
      es.push_back(e);
    }

    void handle(exc::Exception& e){
      std::list<ExceptionHandler*>::iterator i;
      for (i=es.begin();i!=es.end();i++){
	(*i)->handle(e);
      }
    }

    size_t listenerNum(){
      return es.size();
    }

    virtual ~ExceptionSignal(){}

  };


}

#endif
