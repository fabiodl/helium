/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags e
 */


#ifndef _HE_CONNECTORPOOL
#define _HE_CONNECTORPOOL
#include <helium/signal/signalType.h>


namespace helium{

class ConnectorPool{

  class GenConn{
  public:
    virtual void disconnect()=0;
    virtual ~GenConn(){};
  };




  template<typename S,typename CR> class Conn:public GenConn{
    S& s;
    CR& cr;
    void disconnect(){
      s.disconnect(cr);
    }
  public:
    Conn(S& _s,CR& _cr):s(_s),cr(_cr){
    }
  };

  template<typename D,typename C,typename P,bool TS,bool EH>
  class FuncConn:
    public GenConn,
    public Signal<D>,
    public ConnReference<D>{

    typedef typename SignalType<D,TS,EH>::value Sig;

    Sig& s;
    C* c;
    void (C::*f)(P);
  public:
    FuncConn(Sig & _s,C* _c,void (C::*_f)(P)):
      ConnReference<D>(this),
      s(_s),c(_c),f(_f){
    }

    void operator()(D d){
      (c->*f)(d);
    }
    void disconnect(){
      s.disconnect(*this);
    }
  };


  template<typename C,bool TS,bool EH>
  class FuncConn<void,C,void,TS,EH>:
    public GenConn,
    public Signal<void>,
    public ConnReference<void>{

    typedef typename SignalType<void,TS,EH>::value Sig;
    Sig& s;
    C* c;
    void (C::*f)();
  public:
    FuncConn(Sig& _s,C* _c,void (C::*_f)()):
      ConnReference<void>(this),
      s(_s),c(_c),f(_f){
    }

    void operator()(){
      (c->*f)();
    }
    void disconnect(){
      s.disconnect(*this);
    }
  };

    

  std::vector<GenConn*> activeConns;

public:
  template<typename S,typename CR>
  void connect(S& s,CR& cr){
    s.connect(cr);
    activeConns.push_back(new Conn<S,CR>(s,cr));
  }



  template<typename D,typename C,typename P>
  void connect(TsSignal<D>& s,C* c,void (C::*f)(P)){
    FuncConn<D,C,P,true,false>* conn=new FuncConn<D,C,P,true,false>(s,c,f);
    s.connect(*conn);
    activeConns.push_back(conn);
  }


  template<typename C>
  void connect(TsSignal<void>& s,C* c,void (C::*f)()){
    FuncConn<void,C,void,true,false>* conn=new FuncConn<void,C,void,true,false>(s,c,f);
    s.connect(*conn);
    activeConns.push_back(conn);
  }


  template<typename D,typename C,typename P>
  void connect(Signal<D>& s,C* c,void (C::*f)(P)){
    FuncConn<D,C,P,false,false>* conn=new FuncConn<D,C,P,false,false>(s,c,f);
    s.connect(*conn);
    activeConns.push_back(conn);
  }


  template<typename C>
  void connect(Signal<void>& s,C* c,void (C::*f)()){
    FuncConn<void,C,void,false,false>* conn=new FuncConn<void,C,void,false,false>(s,c,f);
    s.connect(*conn);
    activeConns.push_back(conn);
  }

  void disconnect(){
    //std::cout<<"Disconnecting"<<std::endl;
    for (std::vector<GenConn*>::iterator it=activeConns.begin();
	 it!=activeConns.end();
	 ++it){
      (*it)->disconnect();
      delete *it;
    }    
    activeConns.clear();
  }


  ~ConnectorPool(){
    disconnect();
  }
  
};



}

#endif
