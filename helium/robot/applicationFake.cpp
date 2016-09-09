#include "application.h"
#include "robotProtocol.h"
namespace helium{

  static int fargc=0;
  static char** fargv=NULL;

  Application::Application(): argc(fargc),argv(fargv),machineDescription(mem),help(mem){    
  }

  void Application::launch(int & pargc,char** pargv,const char* mainConfig,RunMode runMode){
    argc=pargc;argv=pargv;
    Defaults def;

    if (mainConfig!=NULL){
      def[MAINCONFIGFILELOCATION]=mainConfig;
    }
    paramToMem(mem,argc,argv,def);
    if (mainConfig!=NULL){
      config.load(mem);
    }
    Thread::setGlobalExceptionHandler(&ph);
    dexport(help,mem);
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
