/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_KEYBOARDCELLSERVER
#define HE_KEYBOARDCELLSERVER

#include <helium/thread/thread.h>
#include <helium/system/cellRequestParser.h>
#include <helium/signal/destroyableSignal.h>

namespace helium{

class KeyboardCellServer:public Thread{
  public:
    typedef std::ostream Port;
    Port& defaultPort;
  Port& getPort(Cell* c);
  private:
  Memory &m;
  void run();

  public:
  KeyboardCellServer(Memory &pm);
  
  ~KeyboardCellServer();
  
  DestroyableSignal<void> onStreamTermination;
  void stopReading();
 

};

}


#endif
