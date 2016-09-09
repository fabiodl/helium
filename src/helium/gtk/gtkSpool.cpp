/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/gtk/gtkSpool.h>
#include <helium/thread/mutex.h>

#ifdef _WIN32
#undef max
#endif
#include <gtkmm.h>


namespace helium{

  class GlobalDisp{
    Mutex lock;
    Glib::Dispatcher disp;
    Callback<void>* toDo;
  public:
    GlobalDisp():lock("globalDisp"){
      disp.connect(sigc::mem_fun(*this,&GlobalDisp::onDisp));
    }

    void onDisp(){
      (*toDo)();
    }

    void execute(Callback<void>* ptoDo){
      lock.lock();
      toDo=ptoDo;
      disp();
      lock.unlock();
    }
    
  };

  GlobalDisp globalDisp;






  void gtkExecute(Callback<void>* cb){
    globalDisp.execute(cb);
  }

  


}
