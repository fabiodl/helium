/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/thread/exitHandler.h>



namespace helium{

  class ExitHandlerCreator{
  public:
    ExitHandler exitHandler;
  };


  ExitHandlerCreator syst;



  ExitHandler::ExitHandler():systemRunning(true){
  }

  ExitHandler::~ExitHandler(){
    systemRunning=false;
    onGlobalExitRequest();
    onGlobalExitExecution();
  }

  


  /*class ExitHandlerCreator{//this object is destroyed BEFORE onExit executes
  public:
    ExitHandlerCreator(ExitHandler* &who);//to assure the pointer is already there
  };
  
  ExitHandler* exitHandler;
  ExitHandlerCreator creator(exitHandler);
  
  

  
  
  void onExit(){
    exitHandler->onExit();
    delete exitHandler;
  }
  
  
  

  
  
  ExitHandlerCreator::ExitHandlerCreator(ExitHandler* &who){
    who=new ExitHandler();
  }    



  void ExitHandler::onExit(){
    systemRunning=false;
    onGlobalExitRequest();
    onGlobalExitExecution();
  }


  ExitHandler::~ExitHandler(){
    std::cout<<"e hand dest"<<std::endl;
  }

  ExitHandler::ExitHandler():systemRunning(true){
  }
  */

  ExitHandler& ExitHandler::get(){
    return syst.exitHandler;
  }

  /*bool ExitHandler::isSystemRunning(){
    return syst.exitHandler.systemRunning;
    }*/
  

  void OnExitWait::operator()(){
    //    std::cout<<"()Begin"<<std::endl;
    s.wait();
    //std::cout<<"()ENd"<<std::endl;

  }
  
  OnExitWait::OnExitWait():cr(this){
    ExitHandler::get().exitExecutionConnect(cr);
    
  }
  
  OnExitWait::~OnExitWait(){
    //std::cout<<"waiterDistBegin"<<std::endl;
    signalCompletion();
    //std::cout<<"waiterDistEnd"<<std::endl;
  }

  void OnExitWait::signalCompletion(){ //usually not required.for special cases.
    s.signal();
  }

  
}
