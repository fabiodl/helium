/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.02.18
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_MOTORERRORHANDLER
#define HE_MOTORERRORHANDLER

#include <helium/robot/hwStateMonitor.h>
#include <helium/robot/motorSpooler.h>
#include <helium/robot/hardwareException.h>
#include <helium/system/cellDescription.h>

namespace helium{


  template<typename MotorSign>
  class MotorExceptionHandler:   
    public HwStateMonitor<typename MotorSpooler<MotorSign>::Motors,	
			  MotSpoolPriority
			  >
  {
    


    
  public:    
    
    typedef
    HwStateMonitor<typename MotorSpooler<MotorSign>::Motors,
		   MotSpoolPriority
		   > Super;
  

    typename Super::StateGroupCell sgCell;
    typename Super::StateCellArray ssCell;

    typename Super::MonitorModeGroupCell mgCell;
    typename Super::MonitorModeCellArray msCell;  //DEBUG uncommented by fb 20120529
  
    //dr is for getting the optimized getWorking split
    MotorExceptionHandler(MotorSpooler<MotorSign>& spool,typename motor::Raw<MotorSign>::Hardware& mhw):
      Super(spool,mhw,spool.getMotorNum()),
      sgCell(*this),
      ssCell(*this,spool.getMotorNum()),
      mgCell(*this),
      msCell(*this,spool.getMotorNum()) //DEBUG uncommented by fb 20120529
    {
    }

  };//class MotorExceptionHandler

 


  template<typename MotorSign>
  class DefDescr<MotorExceptionHandler<MotorSign> >:public Description<MotorExceptionHandler<MotorSign> >{
  public:
    DefDescr(MotorExceptionHandler<MotorSign>& o):Description<MotorExceptionHandler<MotorSign> >(o){
    }

    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(vectMapping(this->obj.ssCell,IDPath("motors.motor","."),"state",IDPath("motors.num",".")));
      members.push_back(scalarMapping(this->obj.sgCell,IDPath("motors.state",".")));
      members.push_back(vectMapping(this->obj.msCell,IDPath("motors.motor","."),"monitorMode",IDPath("motors.num",".")));
      members.push_back(scalarMapping(this->obj.mgCell,IDPath("motors.monitorMode",".")));
    }
    
  };



}//helium


#endif


  /*template<typename MotorSign>
  HwInterface& getWorkChecker(typename motor::MotorAccess<MotorSign>& m){
    return *m.hw;
    }*/

