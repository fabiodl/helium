/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/gtk/gtkSignal.h>
#include <helium/thread/thread_private.h>

namespace helium{



  void GtkSignalling::process(){
    exc::MultipleExceptionHolder meh;    
    while(true){
      gm.lock();      
      if (toSignal.size()==0){
	gm.unlock();
	break;	
      } 
      Callback<void>* c=toSignal.front();
      toSignal.pop_front();
      gm.unlock();
      try{
	(*c)();
      }catch(exc::Exception& e){
	meh.handle(e);
      }
    }
  }
  
  void GtkSignalling::requestProcessing(Callback<void>* c){
    if (this==0) return;
    gm.lock();
    toSignal.push_back(c);
    gm.unlock();
    //std::cout<<"{ disp"<<std::endl;
    disp();
    //std::cout<<"} disp"<<std::endl;
  }
  

  void GtkSignalling::cancelProcessing(Callback<void>* c){
    if (this==0) return;
    gm.lock();
    std::list<Callback<void>* >::iterator it=toSignal.begin();
    do{
      if (*it==c){
	toSignal.erase(it++);
      }else{
	++it;
      }
    }while(it!=toSignal.end());    
    gm.unlock();    
  }
  

    

  GtkSignalling::GtkSignalling():gm("Gtk::Connector"),disp(*this){      
    //disp.connect(sigc::mem_fun(*this,&GtkSignalling::process));
  }

  GtkSignalling::LocklessDispatcher::LocklessDispatcher(GtkSignalling& g):running(true){
	disp.connect(sigc::mem_fun(g,&GtkSignalling::process));
	start();
	
      }
  
 
GtkSignalling::LocklessDispatcher:: ~LocklessDispatcher(){
	running=false;
	s.signal();
	join();
      }


  void GtkSignalling::LocklessDispatcher::operator()(){
	s.signal();
  }
  

GtkSignalling* GtkSignalling::signalling=NULL;


  void GtkSignalling::init(){
    signalling=new GtkSignalling();
  }

  void GtkSignalling::close(){
    delete signalling;
  }
  
    

}//ns helium

