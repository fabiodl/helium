#ifndef HE_TSENSOREXCEPTIONHANDLER
#define HE_TSENSOREXCEPTIONHANDLER

#include <helium/core/exception.h>
#include <helium/robot/sensorSpooler.h>
namespace helium{

  template<typename T>
  sensor::Set<T>& getWorkChecker(sensor::Set<T>& s){
    return s;
  }
  
  template<typename T>
  class TsensorExceptionHandler:
    public exc::ExceptionHandler,
    public HwStateMonitor<sensor::Set<T>,
			  sensor::Set<T>,
			  getWorkChecker,
			  SensorSpooler<T>,
			  typename sensor::Set<T>::Token
			  >{

    

    void handle(exc::Exception& e){
      try{
	e.rethrow();
      }
      catch(exc::FaultyValueTsensorException& me){
	this->notifyState(me.id,FAULTYVALUEHWSTATE);
      }
      catch(exc::UnreachableTsensorException& me){
	this->notifyState(me.id,UNREACHABLEHWSTATE);
      }
      catch(exc::UnreachableTsBoardException& be){
	for (size_t i=0;i<be.affectedSensor.size();i++){
	  this->notifyState(be.affectedSensor[i],UNREACHABLEHWSTATE);
	}
      }
      catch(exc::FaultyValTsBoardException& be){
	for (size_t i=0;i<be.affectedSensor.size();i++){
	  this->notifyState(be.affectedSensor[i],FAULTYVALUEHWSTATE);
	}
      }      
      catch(exc::TsensorBoardException &be){
	for (size_t i=0;i<be.affectedSensor.size();i++){
	  this->notifyState(be.affectedSensor[i],GENERICFAULTYHWSTATE);
	}
      }      
      catch(exc::SingleTsensorException& me){       
	this->notifyState(me.id,GENERICFAULTYHWSTATE);      
      }
      catch(exc::MultipleException& e){
	e.handleUsing(this);
      }
    }
    
  public:
    typedef HwStateMonitor<sensor::Set<T>,
			   sensor::Set<T>,
			   getWorkChecker,
			   SensorSpooler<T>,
			   typename sensor::Set<T>::Token
			   > Super;
    typename Super::GroupCell gCell;
    TsensorExceptionHandler(SensorSpooler<T>& spool,const HwInterface& pdr, int n):
      Super(spool,pdr,n),
      gCell(*this)
    {
      
    }
    
  };//class TsensorExceptionHandler


  template<typename T>
  class DefDescr<TsensorExceptionHandler<T> >:public Description<MotorExceptionHandler<T> >{
  public:
    DefDescr<TsensorExceptionHandler<T> >(TsensorExceptionHandler<T>& o):
    MotorExceptionHandler<T>(o){
    }  
    
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(scalarMapping(this->obj.gCell,IDPath("tsensors.state",".")));      
    }
    
  };
    

}//helium
    


#endif
