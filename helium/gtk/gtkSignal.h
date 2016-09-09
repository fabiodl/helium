/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_GTKSIGNAL
#define HE_GTKSIGNAL

#include <helium/gtk/gtkExceptionHandling.h>
#include <helium/signal/signal.h>
#include <helium/thread/mutex.h>
#include <helium/thread/semaphore.h>
#include <helium/thread/thread.h>
#include <gtkmm.h>

//#ifdef _WIN32
//#include <helium/core/pairFix.h>
//#endif

namespace helium{

  template<typename D,typename DS> struct SignalledElement{
    //#ifdef _WIN32
    //    typedef helium::pair<DS,const Callback<D>*> Type;
    //#else
    typedef std::pair<DS,const Callback<D>*> Type;
    //#endif
  };
 
  //DS is data storage
  //D is the callback type
  template<typename D,typename DS,typename Signal,
	   bool lastValue> class DataList;
  
  template<typename D,typename DS,typename Signal>
  class DataList<D,DS,Signal,false>{


    std::list<typename SignalledElement<D,DS>::Type > vals;
  public:
    void sendUpdates(Signal* s);    
    void push_back(const typename SignalledElement<D,DS>::Type& p){vals.push_back(p);}
  };


  template<typename D,typename DS,typename Signal>
  class DataList<D,DS,Signal,true>{
  public:
    typename SignalledElement<D,DS>::Type val;
    void sendUpdates(Signal* s);    
    void push_back(const typename SignalledElement<D,DS>::Type& p){
      //std::cout<<"val ";
      //print(p)<<std::endl;
      val=p;
    }
  };




  template<typename D,typename DS,bool lastValue,bool safe>
  class GtkSignal:public Callback<D>,Callback<void>{
    Mutex m;// used for both the vals list

    DataList<D,DS,GtkSignal<D,DS,lastValue,safe>,lastValue > vals;

    std::list<Callback<D>*> cs;

    friend class DataList<D,DS,GtkSignal<D,DS,lastValue,safe>,lastValue >;
    void notifyValue(typename SignalledElement<D,DS>::Type& it);

    void operator()();//called by the dispatcher of the gtksignalling
  private:
    GtkSignal(const GtkSignal&);//Cannot copy this

    GtkSignal& operator= (const GtkSignal &cSource);  
    sigc::connection conn;
    
  public:
    
    void onRemote(D d);
    void operator()(D d);
    
    void operator()(D d,const Callback<D>* noEx);
    void operator()(D d,const ConnReference<D>& noEx);

    void connect(Callback<D>* cb);    
    void connect(ConnReference<D> & c);
    void disconnect(ConnReference<D> &r);

    size_t listenerNum();
    
    GtkSignal();
    ~GtkSignal();
    

  };//GtkSignal



  class GtkSignalling{

  public:

    
    static void init(); //to be called in the Gtk thread
    static void close(); //to be called in the Gtk thread

    
     void requestProcessing(Callback<void>*);
     void cancelProcessing(Callback<void>*);
    static GtkSignalling* signalling; 
  private:
    std::list<Callback<void>* > toSignal;
    GtkSignalling();

    Mutex gm;

    void process();


    class LocklessDispatcher:public Thread{

      Semaphore s;
      bool running;
      
      Glib::Dispatcher disp;    

      void run(){
	while(true){
	  s.wait();
	  if (!running) return;
	  disp();
	}
      }
    public:
      LocklessDispatcher(GtkSignalling& g);
     
      ~LocklessDispatcher();
      void operator()();
    };

    LocklessDispatcher disp;


  };






}


#include <helium/gtk/gtkSignal.hpp>


#endif
