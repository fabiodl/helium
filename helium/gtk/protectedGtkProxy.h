/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_PROTECTEDGTKPROXY
#define HE_PROTECTEDGTKPROXY

#include <helium/system/protectedValueProxy.h>
#include <helium/gtk/gtkSignal.h>

namespace helium{
  template<typename PGT,
	   typename Reason,
	   Reason remoteReason,
	   typename Access=DefaultAccessPolicy<PGT>,bool lastValueOnly=false,bool safe=true>
  class ProtectedGtkProxy:public ProtectedValueProxy<PGT,
						     Reason,
						     remoteReason,
						     Access,
						     GtkSignal<typename signalArg<PGT,Access>::value,
							       typename cellTypes<PGT>::value::UT, 
							       lastValueOnly,
							       safe>,
						     &GtkSignal<typename signalArg<PGT,Access>::value,
								typename cellTypes<PGT>::value::UT, 
								lastValueOnly,
								safe>::onRemote >{
  public:
    ProtectedGtkProxy(){}
    template<typename T> ProtectedGtkProxy(const T& t):
      ProtectedValueProxy<PGT,
			  Reason,
			 remoteReason,
			  Access,
			  GtkSignal<typename signalArg<PGT,Access>::value,
				    typename cellTypes<PGT>::value::UT, 
				    lastValueOnly,
				    safe>,
			  &GtkSignal<typename signalArg<PGT,Access>::value,
				     typename cellTypes<PGT>::value::UT, 
				     lastValueOnly,
				     safe>::onRemote >
      (t){}
  };
}//ns helium


#endif
