/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_TSENSOREXCEPTIONHANDLER
#define HE_TSENSOREXCEPTIONHANDLER

#include <helium/core/exception.h>
#include <helium/robot/sensorSpooler.h>
#include <helium/robot/hwStateMonitor.h>
namespace helium{

  //template<typename T>
  //sensor::Set<T>& 
 
  
  template<typename T>
  class SensorExceptionHandler:    
    public HwStateMonitor<sensor::Set<T>,			  			
			  SensSpoolPriority
			  >{   
    
  public:
    typedef HwStateMonitor<sensor::Set<T>,			
			   SensSpoolPriority
			   > Super;
    typename Super::StateGroupCell sgCell;
    typename Super::MonitorModeGroupCell mgCell;
    //pdr is for getting the optimized getWorking split
    SensorExceptionHandler(SensorSpooler<T>& spool,HwInterface& pdr, int n):
      Super(spool,pdr,n),
      sgCell(*this),
      mgCell(*this)
    {
    }
    
  };//class SensorExceptionHandler


  template<typename T>
  class DefDescr<SensorExceptionHandler<T> >:public Description<SensorExceptionHandler<T> >{
  public:
    DefDescr<SensorExceptionHandler<T> >(SensorExceptionHandler<T>& o):
    Description<SensorExceptionHandler<T> >(o){
    }  
    
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(scalarMapping(this->obj.sgCell,IDPath("group.state",".")));      
      members.push_back(scalarMapping(this->obj.mgCell,IDPath("group.monitorMode",".")));      
    }
    
  };
    

}//helium
    


#endif
