#ifndef HE_TREADEDSIGNAL
#define HE_TREADEDSIGNAL


template<typename T> class ThreadedSignal;


template<> class ThreadedSignal<void>
{

  void run(){
    while(true){
      
    }

  }

  void signal(){
  };
  

};



#endif
