/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.1
 * Release_flags s
 */

#ifndef HE_SIGNAL_
#define HE_SIGNAL_


#include <list>
#include <helium/signal/callback.h>
#include <helium/debug/debugOptions.h>



namespace helium{


  class DisconnectInfo{            
  public:
    virtual ~DisconnectInfo(){}
  };
  
  
  template <typename D,typename IT=typename std::list<Callback<D>*>::iterator,typename Data=void>
  class IterDiscInfo:public DisconnectInfo{
  public:
    
    Data data;
    IT it;


    IterDiscInfo(IT pit):it(pit){
    }
    
  };//class IterDiscInfo


  template <typename D,typename IT>
  class IterDiscInfo<D,IT,void>:public DisconnectInfo{
  public:
    
    IT it;

    IterDiscInfo(IT pit):it(pit){
    }
    
  };//class IterDiscInfo


  template <typename D> 
  class ConnReference{
  private:
    ConnReference(const ConnReference&);//avoid the copy constructor
    
  public:
    Callback<D>* cb;
    DisconnectInfo* disc;
    ConnReference(Callback<D>* pcb):cb(pcb),disc(NULL){
    }


    void clearDisc(){
      delete disc;
      disc=NULL;
    }

    void clear(){
      if (disc!=NULL) delete disc;
      disc=NULL;
    }
    
    virtual ~ConnReference(){
      if (disc!=NULL) delete disc;
    }


  };//class ConnReference
  
  


  template <typename D> class Signal:public Callback<D>{
    //protected:
  private:
  std::list<Callback<D>*> cs;

    Signal(const Signal& s);
    Signal& operator=(const Signal& s);
 public:
    Signal(){}

    
  void connect(ConnReference<D>& c){
    
  
#ifdef DEBUGCONNECTONCE
    if (c.disc!=NULL){
      throw exc::Bug("Connecting two times!");
    }
#endif

   
    c.disc=new IterDiscInfo<D>(cs.insert(cs.end(),c.cb));
  }

  void disconnect(ConnReference<D> &r){
    cs.erase(trust_cast<IterDiscInfo<D>*>(r.disc)->it);
    delete r.disc;
    r.disc=NULL;
  }
    
    void connect(DisconnectInfo* &disc,Callback<D>* cb){
    
      disc=new IterDiscInfo<D>(cs.insert(cs.end(),cb));
    }
    
    void disconnect(DisconnectInfo* &disc){
      cs.erase(trust_cast<IterDiscInfo<D>*>(disc)->it);
      delete disc;
      disc=NULL;
    }


  void connect(Callback<D>* cb){
    cs.insert(cs.end(),cb);
  }

  
  void disconnect(Callback<D>* cb){
    cs.remove(cb);
  }


    inline void operator()(D d){
    typename std::list<Callback<D>*>::iterator i;
    for (i=cs.begin();i!=cs.end();i++){
      (*(*i))(d);
    }
  }

    
    inline void operator()(D d,const Callback<D>* noExec){
      typename std::list<Callback<D>*>::iterator i;
      for (i=cs.begin();i!=cs.end();i++){
	if (*i!=noExec)
	  (*(*i))(d);
      }
    }

    void operator()(D d,ConnReference<D>& noExec){
      (*this)(d,noExec.cb);
    }
    

    void signalAndClear(D d){
      for (typename std::list<Callback<D>*>::iterator it=cs.begin();it!=cs.end();){
	(**it)(d);
	cs.erase(it++);
      }
    }

    void clear(){
      cs.clear();
    }

  inline int listenerNum(){
    return (int)cs.size();
  }

    Callback<D>* clone() const{
      Signal* s=new Signal<D>();
      s->cs=cs;
      return s;
    }  
  
};//Signal<D>
 
 
  template <> class Signal<void>:public Callback<void>{
  std::list<Callback<void>*> cs;
    Signal(const Signal& s);
    Signal& operator=(const Signal& s);
 public:
    Signal(){}
    /*template <typename C> void connect(C* c,void (C::*m)()){   
    Callback<void> *cb=new dcb::ClassCallback<void,C>(c,m);
    cs.push_back(cb);
  }

  template <typename C,typename P> void connect(C* c,void (C::*m)(P),P p){   
    Callback<void> *cb=new dcb::ParamClassCallback<void,C,P>(c,m,p);
    cs.push_back(cb);
    }*/

  void connect(ConnReference<void>& c){

#ifdef DEBUGCONNECTONCE
    if (c.disc!=NULL) throw exc::Bug("Connecting two times!");
#endif

    c.disc=new IterDiscInfo<void>(cs.insert(cs.end(),c.cb));
  }

  void disconnect(ConnReference<void> &r){
    cs.erase(trust_cast<IterDiscInfo<void>*>(r.disc)->it);
    delete r.disc;
    r.disc=NULL;
  }

    
    void connect(Callback<void>* cb){
      cs.insert(cs.end(),cb);
    }
  
    void disconnect(Callback<void>* cb){
      cs.remove(cb);
  }
    


  void operator()(){
    std::list<Callback<void>*>::iterator i;
    for (i=cs.begin();i!=cs.end();i++){
      (*(*i))();
    }
  }


    inline void operator()(const Callback<void>* noExec){
       std::list<Callback<void>*>::iterator i;
      for (i=cs.begin();i!=cs.end();i++){
	if (*i!=noExec)
	  (*(*i))();
      }
    }


    void operator()(ConnReference<void>& noExec){
      (*this)(noExec.cb);
    }


    void signalAndClear(){
      for (std::list<Callback<void>*>::iterator it=cs.begin();it!=cs.end();){
	(**it)();
	cs.erase(it++);
      }
    }

    void clear(){
      cs.clear();
    }

  inline int listenerNum(){
    return (int)cs.size();
  }


    Callback<void>* clone() const{
      Signal* s=new Signal();
      s->cs=cs;
      return s;
    }

};//Signal<void>

 
 

}//helium




#endif
