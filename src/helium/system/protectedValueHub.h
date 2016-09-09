/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_PROTECTEDVALUEHUB
#define HE_PROTECTEDVALUEHUB

#include <helium/system/valueHub.h>
#include <helium/system/protectedSetter.h>

namespace helium{


  template <typename T,typename Reason,
	   Reason remoteReason>
  class ProtectedValueHub:private ValueHub<T>,
			  public ProtectedSetter<T,T,
						 Reason,remoteReason,
						 ValueHub<T> 
						 >{


  private:
    ProtectedValueHub& operator=(ProtectedValueHub&){return *this;}
  
  public:


    typedef ProtectedSetter<T,T,Reason,remoteReason,
			    ValueHub<T>
			    > Setter;

    ProtectedValueHub(const T& t):ValueHub<T>(t),
				  Setter((ValueHub<T>&)*this)
    {
    }

    ProtectedValueHub():Setter((ValueHub<T>&)*this){}



    using Setter::set;    
    using Setter::assure;
    using Setter::connect;
    using Setter::disconnect;
    using Setter::assureExcept; //fb 20120403
    


    void signalCache(){
      ValueHub<T>::signalCache();
    }
    const T& get(){
      return this->data;
    }
    
    operator const T&() const{
      return this->data;
    }

    template<typename X> void connect(X& x){
      ValueHub<T>::connect(x);
    }

    void disconnect(ConnReference<T>& c){
      ValueHub<T>::disconnect(c);
    }


  };

};




#endif
