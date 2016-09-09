/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */


#include <helium/gtk/gtkExceptionHandling.h>

namespace helium{
class MsgHandler:public helium::exc::ExceptionHandler{
  
  
  void handle(exc::Exception& e){
    Gtk::MessageDialog dialog(e.what(),false,Gtk::MESSAGE_ERROR);
    dialog.run();
  }

} msgHandler;


  exc::ExceptionHandler* GtkExceptionHandling::defaultHandler=&msgHandler;
}
