/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_GTKPROXY
#define HE_GTKPROXY

#include <helium/system/valueProxy.h>
#include <helium/gtk/gtkSignal.h>

namespace helium{

  
  template<typename PGT,int options=0,bool lastValueOnly=false,bool safe=true, 
	   typename Access=typename  selectFirstOnTrue<DefaultAccessPolicy<PGT>,
					      DerivedAccessPolicy<PGT,options>,
					      options==0>::value
	   >
  class GtkProxy:public ValueProxy<PGT,Access,
				   GtkSignal<typename signalArg<PGT,Access>::value,
					     typename cellTypes<PGT>::value::UT, 
					     lastValueOnly,
					     safe>,
				   &GtkSignal<typename signalArg<PGT,Access>::value,
					      typename cellTypes<PGT>::value::UT, 
					      lastValueOnly,
					      safe>::onRemote >{
  public:
  typedef ValueProxy<PGT,Access,
		     GtkSignal<typename signalArg<PGT,Access>::value,
  typename cellTypes<PGT>::value::UT, 
	    lastValueOnly,
	    safe>, 
		     &GtkSignal<typename signalArg<PGT,Access>::value,
				typename cellTypes<PGT>::value::UT, 
				lastValueOnly,
				safe>::onRemote> VP;
  using VP::operator=;


    //GtkProxy(){}
  template<typename T> GtkProxy(const T& t):VP(t){}


   

  };//GtkProxy

  
  template<typename PGT,int opt,bool l,bool s,typename Access>
  void assureConnection(GtkProxy<PGT,opt,l,s,Access>& p,ConnReference<typename signalArg<PGT,Access>::value> &conn){
      if (conn.disc==NULL){
	p.connect(conn);
      }
    }


  template<typename PGT,int opt,bool l,bool s,typename Access>
  void assureDisconnection(GtkProxy<PGT,opt,l,s,Access>& p,ConnReference<typename signalArg<PGT,Access>::value> &conn){
      if (conn.disc!=NULL){
	p.disconnect(conn);
      }
    }
  

}


#endif
