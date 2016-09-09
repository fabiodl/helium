/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */
#ifndef HE_APPLICATION
#define HE_APPLICATION

#include <helium/thread/thread.h>
#include <helium/robot/config.h>
#include <helium/system/helpCell.h>
#include <helium/system/remoteCellExporter.h>

namespace helium{

  class Application:public Thread{   
  protected:
    int & argc;char** &argv;
    Memory mem;
    Config config;
    std::vector<IDPath> knownParamID;
    std::map <IDPath,std::string> knownParamValue;
  private:
    exc::PrintExceptionHandler ph;
    RemoteCellExporter machineDescription;
    TreeHelp help;
    void start(); //prevent their use
    void execute();
  public:
    Memory& getMem(){return mem;}
    enum RunMode{AS_EXECUTE,AS_RUN,AS_START};//
    Application();
    void addParamID(const IDPath& p);
    void addParamValue(const IDPath& p,const std::string& value);
    
    void launch(int & pargc,char** pargv,const char* mainConfig=NULL,RunMode runMode=AS_EXECUTE);    
  };

}
#endif
