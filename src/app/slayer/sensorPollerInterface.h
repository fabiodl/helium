/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SENSORPOLLERINTERFACE
#define HE_SENSORPOLLERINTERFACE

#include <helium/robot/sensorSpooler.h>
#include <helium/robot/sensorExceptionHandler.h>

namespace helium{

  template<typename T>
  class SensorPollerInterface{
  public:
    helium::SensorSpooler<T> ss;
    helium::ExportableSensorProcessor<T,helium::sensorProcessing::CHANGES_CELL,helium::sensorProcessing::CHANGES_CELL> sp;
    helium::SensorExceptionHandler<T> eh;
    
    
    SensorPollerInterface(helium::sensor::Set<int>& sens):
      ss(sens),
      sp(ss,sens.size()),
      eh(ss,sens,sens.size())
    {
      ss.setExceptionHandler(&eh);
    }

  };
  

  template<typename T> 
  class DefDescr<SensorPollerInterface<T> >:public Description<SensorPollerInterface<T> >{
  public:
    DefDescr(SensorPollerInterface<T>& spi):Description<SensorPollerInterface<T> >(spi){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(scalarMapping(this->d(this->obj.sp),""));
      members.push_back(scalarMapping(this->d(this->obj.eh),""));
    }
  };




}

#endif
