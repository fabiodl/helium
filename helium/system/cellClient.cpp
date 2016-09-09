/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/system/cellClient.h>
#include <helium/core/exception.h>
#include <helium/util/mout.h>

#include <helium/destroy/autoDestroy.h>
namespace helium{


  namespace exc{

    RemoteException::RemoteException(const std::string& s):Exception("Remote Error"+s){
    }

    void RemoteException::rethrow(){
	throw *this;
      }

    Exception* RemoteException::clone() const{
	return new RemoteException(*this);
      }
    
    
  }


  std::map<std::string,int> status;

  Failable::Failable(){
    status[toString(this)]=1;
  }

  Failable::~Failable(){
    status[toString(this)]=-1;
  }

  GenericSignalInterpreter::GenericSignalInterpreter(){
    status[toString(this)]=1;
  }

  GenericSignalInterpreter::~GenericSignalInterpreter(){
    status[toString(this)]=-1;
  }

  /*  void GenericSignalInterpreter::fail(const std::string& reason){
      throw exc::RemoteException(reason);
      }*/

  CellClient::DefaultFail::DefaultFail(CellClient* pc):c(pc){
  }
  
  void CellClient::DefaultFail::fail(const std::string& reason){ //for the set
    try{
      throw exc::RemoteException(reason);
    }catch(exc::Exception& e){
      c->handle(e);
    }
  }

  void CellClient::SmartSemaphore::StackSemaphore::wait(CellClient* c){
    while(!signalled){
      //std::cout<<"down the stack!"<<getThreadId()<<std::endl;
      c->parseNext();
      //std::cout<<"up the stack!"<<getThreadId()<<std::endl;
    }
  }

  void CellClient::SmartSemaphore::StackSemaphore::signal(){
    signalled=true;
  }

  CellClient::SmartSemaphore::StackSemaphore::StackSemaphore():signalled(false){
  }

  CellClient::SmartSemaphore::SmartSemaphore(CellClient* pc):c(pc){
    if (getThreadId()==c->getCallbackThreadId()){
      si=new StackSemaphore();
    }else{
      si=new ClassicSemaphore();
    }
  };


  void CellClient::SmartSemaphore::wait(){
    si->wait(c);
  }

  void CellClient::SmartSemaphore::signal(){
    si->signal();
  }
  

  CellClient::SmartSemaphore::~SmartSemaphore(){
    delete si;
  }



  void CellClient::GenericRequest::wait(){
    s.c->cancelPending.connect(cr);
    // std::cout<<"connecting to "<<&s.c->onDisconnect<<std::endl;
    s.wait();
    s.c->cancelPending.disconnect(cr);
    if (returnReason==OK) return;
    switch(returnReason){
    case DISCONNECTED:
      throw exc::WaitingOnDisconnectedSocket();
    case REMOTEERROR:{
      throw exc::RemoteException(remoteError);
    }
    case UNKNOWN:
      throw exc::Bug("Finished to wait for no reasons");
    case OK:
      //for compiler warnings
      break;
    }
  }


  CellClient::GenericRequest::GenericRequest(const std::string& premotename,CellClient* c):
    remotename(premotename),s(c),returnReason(UNKNOWN),cr(this){
    status[toString(this)]=1;
  }

  CellClient::GenericRequest::~GenericRequest(){
    status[toString(this)]=-1;
  }

  void CellClient::GenericRequest::setResult(std::istream& i){
    readResult(i);
    returnReason=OK;
    s.signal();
  }
  
  void CellClient::GenericRequest::operator()(){
    returnReason=DISCONNECTED;
    //std::cout<<"GR::Disconencted"<<std::endl;
    s.signal();
  }

  void CellClient::GenericRequest::fail(const std::string& reason){
    returnReason=REMOTEERROR;
    remoteError=reason;	
    s.signal();
  }

  CellClient::StdReplyRequest::StdReplyRequest(CellClient* c):GenericRequest("",c){
  }
  

  void CellClient::StdReplyRequest::readResult(std::istream& i){
    StdReply r;
    i>>r;
  }
  
  CellClient::WholeContentRequest::WholeContentRequest(const std::string& premotename,CellClient* c,std::ostream& pout):GenericRequest(premotename,c),out(pout){
  }
  
  void CellClient::WholeContentRequest::readResult(std::istream& i){
    std::string s;
    std::getline(i,s,char(0));
    out<<s;
  }      
  



  void CellClient::get(GenericRequest *r){      
    std::stringstream ss;
    void* id=static_cast<void*>(r);
    ss<<ALIASCHAR<<id<<" "<<IDCHAR<<r->remotename<<" "<<requestChar[GETVALUEMSG]<<COMMANDTERMINATORCHAR;
    send(ss.str());
    r->wait();     
  }
  
  void CellClient::getAsciiPath(GenericRequest *r){      
    std::stringstream ss;
    void* id=static_cast<void*>(r);
    //std::cout<<"forming request"<<r->remotename<<std::endl;
    ss<<ALIASCHAR<<id<<" "<<r->remotename<<" "<<requestChar[GETVALUEMSG]<<COMMANDTERMINATORCHAR;
    //std::cout<<"trying to ask"<<ss.str()<<std::endl;
    send(ss.str());
    r->wait();     
  }
  
  void CellClient::cellConnect(const std::string& remotename,GenericSignalInterpreter* c){
    std::stringstream ss;
    StdReplyRequest r(this);
    c->connRequest=&r ;
    void* id=static_cast<void*>(c);
    ss<<ALIASCHAR<<id<<" "<<IDCHAR<<remotename<<" "<<requestChar[CONNECTMSG]<<COMMANDTERMINATORCHAR;
    send(ss.str());
    r.wait();
  }

  void CellClient::cellConnect(const std::string& remotename,GenericSignalInterpreter* c,const std::string& param ){
    std::stringstream ss;
    StdReplyRequest r(this);
    c->connRequest=&r;
    void* id=static_cast<void*>(c);
    ss<<ALIASCHAR<<id<<" "<<IDCHAR<<remotename<<" "<<requestChar[CONNECTPARAMMSG]<<" "<<param<<COMMANDTERMINATORCHAR;
    //std::cout<<"CCCC"<<ss.str()<<std::endl;
    send(ss.str());
    r.wait();
  }


  
  void CellClient::cellDisconnect(const std::string& remotename){    
    std::stringstream ss;
    StdReplyRequest r(this);
    void* id=static_cast<void*>(&r);
    ss<<ALIASCHAR<<id<<" "<<IDCHAR<<remotename<<" "<<requestChar[DISCONNECTMSG]<<COMMANDTERMINATORCHAR;
    try{
#ifdef  TRACEDEBUG
      exc::ExceptionsAreSilent=true;
#endif
      send(ss.str());
      r.wait();    
#ifdef  TRACEDEBUG
      exc::ExceptionsAreSilent=false;
#endif
    }catch(exc::NetException& ){
      
    }

  }

  void CellClient::cellDisconnect(const std::string& remotename,const std::string& par){    
    std::stringstream ss;
    StdReplyRequest r(this);
    void* id=static_cast<void*>(&r);
    ss<<ALIASCHAR<<id<<" "<<IDCHAR<<remotename<<" "<<requestChar[DISCONNECTPARAMMSG]<<" "<<par<<COMMANDTERMINATORCHAR;

        try{
#ifdef  TRACEDEBUG
      exc::ExceptionsAreSilent=true;
#endif
      send(ss.str());
      r.wait();    
#ifdef  TRACEDEBUG
      exc::ExceptionsAreSilent=false;
#endif
    }catch(exc::NetException& ){
      
    }
  }

  CellClient::CellClient():defaultFail(this),csplit(NULL),tidMutex("cellClient::tid"),threadId(NO_THREAD){
     
  }

  void CellClient::remitUpdateReader(GenericSignalInterpreter* si){
    tidMutex.lock();
    disconnectedUpdateReceivers.push_back(si);
    tidMutex.unlock();
  }

  void CellClient::destroyUpdateReaders(){
    tidMutex.lock();
    destroyS(disconnectedUpdateReceivers);
    tidMutex.unlock();
  }





  CellClient::~CellClient(){
    disconnect(); //joins internally
  }

  void CellClient::parseNext(){
    //LifeTime c("parseNext");
    if (csplit->retWaitMessage()) return;
    parse(command);
  }

  void CellClient::setThreadId(){
    tidMutex.lock();
    threadId=getThreadId();
    tidMutex.unlock();
  }
  
  void CellClient::unsetThreadId(){
    tidMutex.lock();
    threadId=NO_THREAD;
    tidMutex.unlock();
  }

  ThreadId CellClient::getCallbackThreadId(){
    ThreadId tid;
    tidMutex.lock();
    tid=threadId;
    tidMutex.unlock();
    return tid;
  }


  void CellClient::callback(NetSocket& socket){

    //LifeTime c("callback");
    
    const char terminator[2]={';',0};
    const char ignoreChars[3]={10,13,0};
    
    CommandSplitter splitter(command,socket,terminator,ignoreChars);
    csplit=&splitter;
    setThreadId();

    try{

      while(true){
	//std::cout<<"waiting for stuff"<<std::endl;
	if (splitter.retWaitMessage()) break;      
	//std::cout<<"processing {"<<command<<"}"<<std::endl;
	parse(command);
      }
      //std::cout<<"Msg parser terminated"<<std::endl;
      unsetThreadId();
      destroyUpdateReaders();
    }catch(...){
      unsetThreadId();
      destroyUpdateReaders();
      throw;
    
    }
    //mout(msg::OK)("Disconnected from server");

  }//callback
  
  
  void CellClient::parse(const std::string & command){
    try{
      //std::cout<<"parsing"<<(command.size()<500?command:"...")<<std::endl; 
    std::stringstream ss;
    char at;
    void* id;
    char oper='_';
    ss<<command;
    if (ss.peek()==ALIASCHAR){ //could be a welcome
      read(at,ss);
      read(id,ss);
    }
    read(oper,ss);
    int req=-1;
    for (int i=0;i<helium::MESSAGETYPES;i++){
      if (helium::replyChar[i]==oper){
	req=i;
	break;
      }
    }        
    switch(req){
    case GETVALUEMSG:
    case GETVALUEPARAMMSG:
    case SETVALUEPARAMMSG:
      {
	GenericRequest* s=trust_cast<GenericRequest*>(id);
	if (status[toString(s)]!=1){
	  throw exc::Bug("Notifying invalid generic request!"+toString(status[toString(s)]));
	}
	s->setResult(ss);
    }
      break;
      
    case UPDATEMSG:
      {
	GenericSignalInterpreter* ur=trust_cast<GenericSignalInterpreter*>(id);
	if (status[toString(ur)]!=1){
	  throw exc::Bug("Notifying invalid generic signal interpreter!"+toString(status[toString(ur)]));
	}
	//std::cout <<"--* "<< ss.str() << std::endl;
	ur->notify(ss);
      }
      break;
      //case replyChar[SETVALUEMSG]: should not reply
      //case replyChar[CONNECTMSG]:
    //case replyChar[DISCONNECTMSG]:   
    case ERRORMSG:{
      Failable* f=trust_cast<Failable*>(id);
     
	if (status[toString(f)]!=1){
	  throw exc::Bug("Notifying invalid failable!"+toString(status[toString(f)]));
	}
	f->fail(ss.str());    
    }     
      break;
    case WELCOMEMSG:
      onWelcome(ss.str());
      break;
    case CONNECTMSG:
    case CONNECTPARAMMSG:
      {
	GenericSignalInterpreter* ur=trust_cast<GenericSignalInterpreter*>(id);
	//std::cout<<"conn msg received "<<ss.str()<<std::endl;
	ur->connRequest->setResult(ss);
      }    
      break;
  case DISCONNECTMSG:
    case DISCONNECTPARAMMSG:
      {
	GenericRequest* r=trust_cast<GenericRequest*>(id);
	r->setResult(ss);
      }
      break;
    default:
      throw exc::SyntaxError(oper+" is an invalid operator");
    }//switch
    }catch(exc::Exception& e){
      //std::cerr<<" EXCEPTION In parsing-!"<<std::endl;
      handle(e);
    }
  }//parse
  
  void CellClient::onDisconnected(){
    //std::cout<<"Cancelling pending operations"<<std::endl;
    cancelPending(); //we need first to throw exceptions, and then send the signal (for possible deletion of objects,)
    //std::cout<<"    ...ok"<<std::endl;
    ClientSocket::onDisconnected();
  }
  

   void CellClient::onWelcome(const std::string& s){
     mout(msg::PINFO,1)("Received welcome "+s);     
   }
  
}//helium






  /*  class WorkerSet{
    CellClient& cc;    
    Mutex listLock;	      
    class WorkerThread;
    friend class WorkerThread;
    std::list<WorkerThread*> workers;
    std::vector<WorkerThread*> existingWorkers;
    
    
    
    void addWorker(WorkerThread* wt);
    
    friend std::ostream& operator<<(std::ostream& o,const WorkerThread& w);
    
    class WorkerThread:public Thread{
      Semaphore hasWork;
      CellClient& cc;
      WorkerSet& ws;
      std::string cmd;
      volatile bool toExit;
      friend std::ostream& operator<<(std::ostream& o,const WorkerThread& w);
    public:		
      WorkerThread(CellClient& pcc,WorkerSet& pws);
      void setWork(const std::string& pcmd);
      void run();	
      ~WorkerThread();
    };
    
    
    public:
    
    WorkerSet(CellClient& cc);
    void addWork(const std::string& cmd);
    
    ~WorkerSet();
    
    
  };

  
  inline std::ostream& operator<<(std::ostream& o,const WorkerSet::WorkerThread& w){
    return o<<w.cmd;
  }



  WorkerSet::WorkerThread::WorkerThread(CellClient& pcc,WorkerSet& pws):cc(pcc),ws(pws){
  }


  void WorkerSet::WorkerThread::setWork(const std::string& pcmd){
    cmd=pcmd;	  
    hasWork.signal();
  }
  

  void WorkerSet::WorkerThread::run(){    
    toExit=false;
    while(true){	    
      //std::cout<<"worker "<<this<<"waiting for work"<<std::endl;
      hasWork.wait();
      if (toExit){
	//std::cout<<"worker "<<this<<"should die"<<std::endl;
	return;
      }
      try{	
	//std::cout<<"worker "<<this<<"parsing"<<(cmd.length()<500?cmd:"...")<<std::endl;
	cc.parse(cmd);
      }catch(exc::Exception& e){
	handle(e);
      }
      ws.addWorker(this);
      //std::cout<<"worker "<<this<<"wants to work again"<<std::endl;      
    }

  }

  WorkerSet::WorkerThread::~WorkerThread(){
    toExit=true;
    hasWork.signal();
    join();
  }


  void WorkerSet::addWorker(WorkerThread* wt){
    listLock.lock();
    workers.push_back(wt);
    listLock.unlock();
  }

  void WorkerSet::addWork(const std::string& cmd){
    listLock.lock();
    if (workers.size()==0){
      WorkerThread* wt=new WorkerThread(cc,*this);

      wt->setExceptionHandler(cc.getExceptionHandler());
      existingWorkers.push_back(wt);
      workers.push_back(wt);
      workers.front()->setWork(cmd); //we need to set before it is started;
      wt->start();
    }else{
      workers.front()->setWork(cmd);
    }

    workers.pop_front();
    listLock.unlock();
  }

  WorkerSet::WorkerSet(CellClient& pcc):cc(pcc),listLock("workerSet::listLock"){    
    
  }

  WorkerSet::~WorkerSet(){
    MutexLock lock(listLock);
    destroyS(existingWorkers);
  }
  
  */
