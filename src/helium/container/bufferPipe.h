/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_BUFFERPIPE
#define HE_BUFFERPIPE

#include <helium/thread/mutex.h>
#include <helium/container/minimalList.h>
#include <helium/core/referenceType.h>


namespace helium{



  template<typename T,typename NP>
  class ParamNode{
    T t;
  public:
    ParamNode* next;
    ParamNode(NP p):t(p),next(NULL){}
    T& operator*(){
      return t;
    }
  };
  
  template<typename T>
  class ParamNode<T,Void>{
    T t;
  public:
    ParamNode* next;
    ParamNode():next(NULL){}
    T& operator*(){
      return t;
    }
  };


  template<typename T,typename SP,typename PP,typename NP> struct NodeFactory{
    SP data;        
    NodeFactory(PP p):data(p){}
    ParamNode<T,NP>* getNew(){
      return new ParamNode<T,NP>(data);
    }
  };

  template<typename T,typename PP,typename NP> struct NodeFactory<T,Void,PP,NP>{
    NodeFactory(Void p){}
    ParamNode<T,NP>* getNew(){
      return new ParamNode<T,Void>();
    }
  };




  template<typename T,
	   typename SP=void,
	   typename PP=typename constReference<SP>::value,
	   typename NP=typename constReference<SP>::value> class BufferPipe{
    Mutex wm,rm;
    
    NodeFactory<T,SP,PP,NP> factory;

  public:    
    typedef ParamNode<T,NP> Node;
  private:
    MinimalList<Node> writer;
    MinimalList<Node> reader;
    Semaphore dataArrived;    
    Node* sentinel;
  public:
    
    BufferPipe(PP p):wm("bpWm"),rm("bpRm"),factory(p),sentinel(NULL){
      //std::cout<<" created mutex "<<&wm<<" and "<<&rm<<std::endl;
    }

    ~BufferPipe(){
      //std::cout<<" destroying mutex "<<&wm<<" and "<<&rm<<std::endl;
      if (sentinel) delete sentinel;
    }
      
    Node* getWriterNode(){
      wm.lock();
      //std::cout<<"before popping writer"<<writer<<std::endl;
      Node* w=writer.pop_front();
      //std::cout<<"popped "<<w<<" remaining "<<writer<<std::endl;
      wm.unlock();
      if (w!=NULL){
	return w;
      }
      return factory.getNew();
    }
    
    
    bool isSentinel(Node* n){
      return n==sentinel;
    }
    
    void sendSentinel(){ //be sure the reader checks for sentinels when using this!!
      if (sentinel==NULL) sentinel=factory.getNew();
      remitWriterNode(sentinel);
    }
    

    void remitWriterNode(Node* n){
      rm.lock();      
      //std::cout<<"pushing "<<n<<std::endl;
      //      std::cout<<"before pushing reader "<<reader<<std::endl;
      reader.push_back(n);
      //      std::cout<<"after pushing reader "<<reader<<std::endl;
      rm.unlock();
      dataArrived.signal();
    }
      
    Node* getReaderNode(){
      Node *n;
      rm.lock();
      do{
	//std::cout<<"before popping reader"<<reader<<std::endl;
	n=reader.pop_front();
	//	std::cout<<"popped "<<n<<" remaining "<<reader<<std::endl;
	if (n==NULL){
	  rm.unlock();
	  dataArrived.wait();
	  rm.lock();
	}
      }while(n==NULL);
      rm.unlock();
      return n;      
    }

    Node* getLastReaderNode(){
      Node *n;
      rm.lock();
      do{
	//std::cout<<"before popping reader"<<reader<<std::endl;

	n=reader.pop_list();
	//	std::cout<<"popped "<<n<<" remaining "<<reader<<std::endl;
	if (n==NULL){
	  rm.unlock();
	  dataArrived.wait();
	  rm.lock();
	}
      }while(n==NULL);
      rm.unlock();
      Node* nstart=n;
      Node* nend=NULL;      
      while(n->next!=NULL){
	nend=n;
	n=n->next;
	std::cout<<"discard"<<std::endl;
      }
     
      if (nend!=NULL){
	wm.lock();
	nend->next=NULL;
	writer.push_list(nstart,nend);
	wm.unlock();
      }

      return n;      
    }

    void remitReaderNode(Node* n){
      wm.lock();
      //std::cout<<"giving the writer "<<n<<std::endl;
      writer.push_back(n);
      wm.unlock();
    }

  };//BufferPipe

  template<typename T> class BufferPipe<T,void,void,void>:
    public BufferPipe<T,Void>

  {
    BufferPipe():BufferPipe<T,Void>(Void()){}
  };


  
}//helium


#endif
