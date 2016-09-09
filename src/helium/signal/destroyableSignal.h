/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_DESTROYABLESIGNAL
#define HE_DESTROYABLESIGNAL

#include <list>
#include <helium/signal/tsSignal.h>


namespace helium{



  template<typename D>
  class DestroyableSignal:public TsSignal<D>{


  private:
    void disconnect(){}//to hide the tssignal disconnect
  public:


    class Junction:public ConnReference<D>{
      Mutex m;
      DestroyableSignal* act;
      bool passDead;
      friend class DestroyableSignal;
      Junction(DestroyableSignal* pact,Callback<D>* pcb):ConnReference<D>(pcb),m("DestSigJun"),act(pact),passDead(false){		
      }
      typename std::list<Junction*>::iterator meInAct;
    public:
      void disconnect(){
	m.lock();
	if (act!=NULL&&this->disc!=NULL){
	  static_cast<TsSignal<D>*>(act)->disconnect(*static_cast<ConnReference<D>*>(this));
	}
	m.unlock();
      }

      void unlinkPass(){
	m.lock();
	passDead=true;
	m.unlock();
      }
      
      
      void unlinkAct(){
	m.lock();
	act=NULL;
	m.unlock();
      }
      

      
      bool isUseless(){
	return passDead&&act==NULL;
      }
      
      ~Junction(){
	m.lock();
	if (act!=NULL){
	  act->removeJunction(meInAct);
	}
	m.unlock();
      }
    };

    
    class SignalReference{
      Junction* j;
      friend class DestroyableSignal;
      void init(Junction* pj){j=pj;}
      Callback<D>* cb;
    public:
      SignalReference(Callback<D>* pcb):j(NULL),cb(pcb){
      }
      void disconnect(){
	j->disconnect();
      }
      ~SignalReference(){
	j->disconnect();
	j->unlinkPass();
	if (j->isUseless()){
	  delete j;
	}
      }
    };


    std::list<Junction*> junctions;

    void connect(SignalReference& ref){
      Junction* j=new Junction(this,ref.cb);
      j->meInAct=junctions.insert(junctions.end(),j);
      TsSignal<D>::connect(*j);
      ref.init(j);
    }

    ~DestroyableSignal(){
      for (typename std::list<Junction*>::iterator it=junctions.begin();it!=junctions.end();++it){
	(*it)->unlinkAct();
	if ((*it)->isUseless()){
	  delete *it;
	}
      }
    }

  private:
    friend class Junction;
    void removeJunction(typename std::list<Junction*>::iterator& it){
      junctions.erase(it);
    }




    



    
  };



}



#endif

