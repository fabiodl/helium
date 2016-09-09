/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */
#include <helium/robot/application.h>
#include <helium/robot/robotProtocol.h>
#include <helium/system/paramToMem.h>
namespace helium{

  static int fargc=0;
  static char** fargv=NULL;

  Application::Application(): argc(fargc),argv(fargv),machineDescription(mem),help(mem){    
  }

  void Application::addParamID(const IDPath& p){
    knownParamID.push_back(p);
  }
  
  void Application::addParamValue(const IDPath& p,const std::string& value){
    knownParamValue[p]=value;
  }
  
  void Application::launch(int & pargc,char** pargv,const char* mainConfig,RunMode runMode){
    argc=pargc;argv=pargv;
    if (mainConfig!=NULL){
      knownParamValue[MAINCONFIGFILELOCATION]=mainConfig;
    }
    paramToMem(mem,argc,argv,knownParamValue,knownParamID);
   
    if (mainConfig!=NULL){
      config.load(mem);
    }
    Thread::setGlobalExceptionHandler(&ph);
    dexport(mem,help);
    if (mem.hasPath(IDPath("config._","."))){
      mem.merge(
		mem.downPath(IDPath("config._","."))
		);      
    }

    switch (runMode){
    case AS_RUN:
      run();
      break;
    case AS_EXECUTE:
      Thread::execute();
      break;
    case AS_START:
      Thread::start();
      break;
    }    
  }


}
