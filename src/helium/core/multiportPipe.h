/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef _FIFOPIPE_
#define _FIFOPIPE_
#include <cstring>
#include <memory>
#include <helium/thread/mutex.h>
#include <helium/thread/semaphore.h>
//#include <helium/core/slContainer.h>
#include <helium/container/slList.h>
#include <helium/container/circularBuffer.h>
#include <iostream>
namespace helium{


  template <typename T>
    class MultiportPipe{
  public:
    class BackPort;
  private:
    Mutex pipeStructure;
    friend class Junction;
    Semaphore dataArrived;

    class MsgQueue;


    class Junction{
      friend class MsgQueue;
      MultiportPipe *pipe;
      typename CircularBuffer<MsgQueue>::iterator p;
      BackPort* r;      
      Mutex mutex;      
    public:
    Junction(MultiportPipe *ppipe,typename CircularBuffer<MsgQueue>::iterator pp,BackPort* pr):pipe(ppipe),p(pp),r(pr),mutex("junction"){
	(**p).j=this;
	r->j=this;
      }

     void unlinkMsgQueue(){ 

       //std::cout<<"msg queue gone"<<std::endl;
       mutex.lock();
       p=NULL;
       mutex.unlock();
     }

     void unlinkReferrer(){
       //std::cout<<"referrer gone"<<std::endl;
       mutex.lock();
       r=NULL;
       mutex.unlock();
     }

     bool isUseless(){
       return  (p==NULL)&&(r==NULL);
     }
      
     void push_back(const T& t){
       //std::cout<<"Junction "<<this<<" has p "<<p<<std::endl;
       if (p!=NULL){
	 mutex.lock();
	 //std::cout<<"locked "<<std::endl;
	 //std::cout<<"junction pushing "<<t<<"to queue"<<&((**p).msg)<<std::endl;
	 (**p).msg.push_back(t);
	 mutex.unlock();
	 pipe->dataArrived.signal();
       }
     }
     


    };

    
    class MsgQueue{
      friend class Junction;
      //SLFifo<T> msg;
      SlList<T> msg;

      Junction* j;      
    public:      
    MsgQueue():j(NULL){
      }
      virtual ~MsgQueue(){
	j->unlinkMsgQueue();
	if (j->isUseless()) delete j;
      } 

      //void pop(UniqueReference<SLNode<T> > &r){
      void pop(std::auto_ptr<typename SlList<T>::Node> &r){
	//std::cout<<"pop from queue"<<&msg<<std::endl;
	j->mutex.lock();
	r.reset(msg.pop_front());
	j->mutex.unlock();
      }
    };//MsgQueue

    CircularBuffer<MsgQueue> ports;

    

    
  public:
    
  MultiportPipe():pipeStructure("pipeStructure"){
    }

    typedef std::auto_ptr<typename SlList<T>::Node > DataReference;

    class BackPort{
      friend class Junction;
      Junction* j;
    public:

    BackPort():j(NULL){
      //std::cout<<"new backport @ "<<this<<std::endl;
      }
      
      virtual ~BackPort(){
	//std::cout<<"deleting backport @ "<<this<<" with junction "<<j<<std::endl;
	if (j==NULL) return;
	j->unlinkReferrer();
	if (j->isUseless()) delete j;
      }

      BackPort& operator<<(const T& t){	
	j->push_back(t);
	return *this;
      }
    };//BackPort

   



    void attachPort(BackPort &r){
      pipeStructure.lock();
      typename CircularBuffer<MsgQueue>::iterator port;
      port=ports.push_back();
      new Junction(this,port,&r);
      pipeStructure.unlock();
    }

    void pop(DataReference &r){
      bool found=false;
      do{
	pipeStructure.lock();
	//std::cout<<"sweep ports size"<<ports.size()<<std::endl;
       	for (int i=0;i<ports.size();i++){
	  ports.get().pop(r);
	  //std::cout<<"examined"<<std::endl;
	  if (r.get()!=NULL){
	    found=true;
	    break;
	  }
	}
	if (!found){
	  //std::cout<<"no msg"<<std::endl;
	  pipeStructure.unlock();
	  dataArrived.wait();
	}
      }while(!found);   
      //std::cout<<"exiting"<<std::endl;
      pipeStructure.unlock();
    }

    

  };//MultiportPipe


 



}//helium
#endif
