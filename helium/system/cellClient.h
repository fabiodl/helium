/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_CELLCLIENT
#define HE_CELLCLIENT

#include <helium/net/clientSocket.h>
#include <helium/net/commandSplitter.h>
#include <helium/util/ioUtil.h>
#include <helium/system/cell.h>



namespace helium{
  

  namespace exc{
    
    class RemoteException:public Exception{
    public:
      RemoteException(const std::string& s);
      virtual void rethrow();
      Exception* clone() const;
    };//class RemoteException
  }//exc
  

  class Failable{
    public:
      virtual void fail(const std::string& reason)=0;
    Failable();
    virtual ~Failable();
  };

  class GenericSignalInterpreter; //fwd decl, needs CellClient::GenericRequest
  

  class CellClient:public ClientSocket{

    class DefaultFail:public Failable{
      CellClient* c;
      virtual void fail(const std::string& reason);//notifies the cellclient thread exc handler
    public:
      DefaultFail(CellClient* c);
    };
    DefaultFail defaultFail;
  
  public:


    class SmartSemaphore{
      struct SemaphoreInterface{
	virtual void wait(CellClient*)=0;
	virtual void signal()=0;
	virtual ~SemaphoreInterface(){}
      };
      
      class ClassicSemaphore:public SemaphoreInterface{
	Semaphore s;
	void wait(CellClient*){s.wait();}
	void signal(){s.signal();}
      };

      class StackSemaphore:public SemaphoreInterface{
	bool signalled;
	void wait(CellClient*);
	void signal();
      public:
	StackSemaphore();
      };

    public:
      CellClient* c;
      SemaphoreInterface* si;
      void wait();
      void signal();     
      SmartSemaphore(CellClient* c);
      ~SmartSemaphore();
    };



    class GenericRequest:public Failable,public Callback<void>{
      friend class CellClient;
      friend class RemoteCellImporter;
      const std::string& remotename;
      SmartSemaphore s;      
      virtual void readResult(std::istream& i)=0;
      enum ReturnReason {OK,DISCONNECTED,REMOTEERROR,UNKNOWN} returnReason;      
      ConnReference<void> cr;
      std::string remoteError;
    public:
            
      
      void wait();
      GenericRequest(const std::string& premotename,CellClient* c);      
      virtual ~GenericRequest();
      void setResult(std::istream& i);
      void operator()();//called for disconnections
      void fail(const std::string& reason);

    };//GenericRequest

    class StdReplyRequest:public GenericRequest{
    public:
      StdReplyRequest(CellClient* c);
      void readResult(std::istream& i);//does not need a reference to a data
    };


    template <typename T >
      class TypedRequest:public GenericRequest{
      T& t;    
      void readResult(std::istream& i);
    public:
      TypedRequest(const std::string& premotename,CellClient* c,T* pt);
    };//Request

    


    template <typename Signature >
    class GetRequest:public GenericRequest{
      typename Signature::GT& t;    
      void readResult(std::istream& i);
    public:
      GetRequest(const std::string& premotename,CellClient* c,typename Signature::GT* pt);
    };//Request

    
    template <typename Signature >
    class SetParamRequest:public GenericRequest{
      typename Signature::SP& t;    
      void readResult(std::istream& i);
    public:
      SetParamRequest(const std::string& premotename,CellClient* c,typename Signature::SP* pt);
    };//Request


    class WholeContentRequest:public GenericRequest{
      std::ostream& out;
    public:
      WholeContentRequest(const std::string& premotename,CellClient* c,std::ostream& pout);
      void readResult(std::istream& i);
      
    };
   
    CommandSplitter* csplit;
    
    std::string command;
    Mutex tidMutex;
    ThreadId threadId;
    void parseNext();
    void setThreadId();
    void unsetThreadId();
    std::vector<GenericSignalInterpreter*> disconnectedUpdateReceivers; //wait the client disconnection to throw away the disconnected objects (to which late updates may arrive)
    void destroyUpdateReaders();
  public:

    ThreadId getCallbackThreadId();
    

    CellClient();
    ~CellClient();

    void remitUpdateReader(GenericSignalInterpreter* si);
    void callback(NetSocket& s);

    void parse(const std::string & command);
    void get(GenericRequest *r);
    void getAsciiPath(GenericRequest *r);
    template <typename T,typename P> void get(GenericRequest *r,const P* p);
    template <typename T> void set(std::string& remotename,const T* t);
    template <typename T> void set(GenericRequest *r,const T* t);
    void cellConnect(const std::string& remotename,GenericSignalInterpreter* c); 
    void cellConnect(const std::string& remotename,GenericSignalInterpreter* c,const std::string& param); 
    void cellDisconnect(const std::string& remotename);          
    void cellDisconnect(const std::string& remotename,const std::string& par);          
    virtual void onWelcome(const std::string& s);

    TsSignal<void> cancelPending;

    void onDisconnected();
    

    };


  class GenericSignalInterpreter:public Failable{
    friend class CellClient;
    CellClient::GenericRequest* connRequest; //used by CellClient to wait  the connection completion
  public:
   
    virtual void notify(std::istream& i)=0;
    GenericSignalInterpreter();
    virtual ~GenericSignalInterpreter()=0;
    //void fail(const std::string& reason); //throws in the calling thread
  };
  


}//helium
  
#include "cellClient.hpp"
  


#endif
