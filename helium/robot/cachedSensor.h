/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_UPDATEONLYSENSOR
#define HE_UPDATEONLYSENSOR

#include <helium/robot/sensorInterface.h>
#include <helium/robot/sensorSpooler.h>

namespace helium{



  template<typename T,typename Owner,sensorProcessing::CellType SingleType>
  class BasicSensorCell:public TypedCell<SensorCellSignature<T> >{
    Owner& owner;
    int idx;        
    std::string getName() const;
    void getData(T& t);
    void connect(Cell::CellConnection& ref,const sensor::ReadType& t);   
    void connect(Cell::CellConnection& ref);
    void disconnect(Cell::CellConnection& ref);    
  public:
    BasicSensorCell(Owner& powner,int pidx):owner(powner),idx(pidx){}
    
  };

  template<typename T,typename Owner,sensorProcessing::CellType GroupType>
    class BasicSensorGroupCell:public TypedCell<SensorGroupCellSignature<T> >{
    Owner& owner;
    
    void getData(SensorValList<T>& g);
    void getData(SensorValList<T>& g,const HwIdList& l);    
    void connect(Cell::CellConnection& ref,const std::pair<HwIdList,sensor::ReadType>& t);       
    void connect(Cell::CellConnection& ref);
    void disconnect(Cell::CellConnection& ref);
    std::string getName() const;

  public:
    BasicSensorGroupCell(Owner& powner):owner(powner){
    }

  };
	   


  template<typename T,sensorProcessing::CellType SingleType,
	   sensorProcessing::CellType GroupType>
  class CachedSensor:public sensor::FilterInterface<T>{   
    SensorNotifierSignal<T> signal;
    Mutex cacheMutex;
    Array<T> cache;
  public:
    
    T read(int idx);
    void read(SensorValList<T>& g);    
    void read(SensorValList<T>& v,const HwIdList& l);
    void read(T* t,const HwIdList& l);
    void read(T* t);
    void connect(SensorNotifier<T>* notifier);
    void disconnect(SensorNotifier<T>* notifier);
    void connect(helium::EhConnReference<const T*>& ref, const helium::HwIdList& what, helium::sensor::ReadType type);
    void disconnect(helium::EhConnReference<const T*>& ref);


    typedef BasicSensorCell<T,CachedSensor,SingleType> SingleSensorCellType;
    typename selectFirstOnTrue<Void::ParamEater,
			       Array<SingleSensorCellType				 
				     ,array::RESIZABLE,
				     array::InternalAllocator<ParamIdxAllocator<SingleSensorCellType,
										CachedSensor&,
										CachedSensor&> 
							      > >,
			       SingleType==sensorProcessing::NO_CELL
			       >::value singleCells;    
    
 
    typename selectFirstOnTrue<Void::ParamEater,
			       BasicSensorGroupCell<T,CachedSensor,GroupType>,
			       GroupType==sensorProcessing::NO_CELL
			       >::value groupCell;    

  public:
    size_t size();
    void update(const T* d,const HwIdList& read);
    void handle(const HwIdList& bad,exc::Exception& e,const T* data);    
    CachedSensor(size_t n);
};

  
  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType> 
  class DefDescr<CachedSensor<T,SingleType,GroupType> >:public Description<CachedSensor<T,SingleType,GroupType> > {  
  public:
    DefDescr(CachedSensor<T,SingleType,GroupType>& c):Description<CachedSensor<T,SingleType,GroupType> >(c){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };



}


#include "cachedSensor.hpp"


#endif
