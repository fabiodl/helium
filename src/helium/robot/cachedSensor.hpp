/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


namespace helium{
 

  template<typename T,typename Owner,sensorProcessing::CellType SingleType>    
  std::string BasicSensorCell<T,Owner,SingleType>::getName() const{
      return stringBegin()<<"Basic individual sensor cell for "<<Typename<Owner>::name<<" sensor "<<idx<<stringEnd();
    }

  template<typename T,typename Owner,sensorProcessing::CellType SingleType>    
    void BasicSensorCell<T,Owner,SingleType>::getData(T& t){
      t=owner.read(idx);
    }

  template<typename T,typename Owner,sensorProcessing::CellType SingleType>    
    void BasicSensorCell<T,Owner,SingleType>::connect(Cell::CellConnection& ref,const sensor::ReadType& t){
      SensorNotifier<T>* notifier=new 
	typename selectFirstOnTrue<
	  SingleSensorChangeNotifier<T>,
	  SingleSensorNotifier<T>,
	  SingleType==sensorProcessing::CHANGES_CELL>::value (t,idx,ref.cb,ref);		
      owner.connect(notifier);
      ref.disc=notifier;
    }
    
  template<typename T,typename Owner,sensorProcessing::CellType SingleType>    
    void BasicSensorCell<T,Owner,SingleType>::connect(Cell::CellConnection& ref){
       connect(ref,sensor::READ);
    }

  template<typename T,typename Owner,sensorProcessing::CellType SingleType>    
    void BasicSensorCell<T,Owner,SingleType>::disconnect(Cell::CellConnection& ref){
      owner.disconnect(trust_cast<SensorNotifier<T>*>(ref.disc));
      ref.clear();
    }
  
  template<typename T,typename Owner,sensorProcessing::CellType GroupType>  
  void  BasicSensorGroupCell<T,Owner,GroupType>::getData(SensorValList<T>& g){
      owner.read(g);      
    }

   template<typename T,typename Owner,sensorProcessing::CellType GroupType>  
    void BasicSensorGroupCell<T,Owner,GroupType>::getData(SensorValList<T>& g,const HwIdList& l){
      g.vals.resize(l.ids.size());
      owner.read(g,l);
    }
    
 template<typename T,typename Owner,sensorProcessing::CellType GroupType>  
    void BasicSensorGroupCell<T,Owner,GroupType>::connect(Cell::CellConnection& ref,const std::pair<HwIdList,sensor::ReadType>& t){
      SensorNotifier<T>* notifier=new typename selectFirstOnTrue<
	GroupSensorChangeNotifier<T>,
	GroupSensorNotifier<T>,
	GroupType==sensorProcessing::CHANGES_CELL>::value (t.second,t.first,ref.cb,ref);
	owner.connect(notifier);
    ref.disc=notifier;
  }
    
     template<typename T,typename Owner,sensorProcessing::CellType GroupType>  
    void BasicSensorGroupCell<T,Owner,GroupType>::connect(Cell::CellConnection& ref){
      SensorNotifier<T>* notifier=new typename selectFirstOnTrue<
	WholeSensorChangeNotifier<T>,
	WholeSensorNotifier<T>,
	GroupType==sensorProcessing::CHANGES_CELL>::value (sensor::READ,owner.size()  ,ref.cb,ref);
    owner.connect(notifier);
    ref.disc=notifier;
    }

 template<typename T,typename Owner,sensorProcessing::CellType GroupType>  
    void BasicSensorGroupCell<T,Owner,GroupType>::disconnect(Cell::CellConnection& ref){
      owner.disconnect(trust_cast<SensorNotifier<T>*>(ref.disc));
      ref.clear();
    }

  template<typename T,typename Owner,sensorProcessing::CellType GroupType>  
    std::string BasicSensorGroupCell<T,Owner,GroupType>::getName() const{
      return stringBegin()<<"Sensor group cell for "<<Typename<Owner>::name<<stringEnd();
    }

  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType>  
  T CachedSensor<T,SingleType,GroupType>::read(int idx){
      cacheMutex.lock();
      T r=cache[idx];
      cacheMutex.unlock();     
      return r;
  }
  
  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType>  
    void CachedSensor<T,SingleType,GroupType>::read(SensorValList<T>& g){
      cacheMutex.lock();
      assign(g.vals,cache);
      cacheMutex.unlock();
    }
    
  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType>  
    void CachedSensor<T,SingleType,GroupType>::read(SensorValList<T>& v,const HwIdList& l){
      cacheMutex.lock();
      for (size_t i=0;i<l.ids.size();++i){
	v.vals[i]=cache[l.ids[i]];
      }
      cacheMutex.unlock();      
    }      

  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType>  
    void CachedSensor<T,SingleType,GroupType>::read(T* t,const HwIdList& l){
      cacheMutex.lock();
      for (size_t i=0;i<l.ids.size();++i){
	t[l.ids[i]]=cache[l.ids[i]];
      }
      cacheMutex.unlock();
    }

  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType>  
    void CachedSensor<T,SingleType,GroupType>::read(T* t){
      cacheMutex.lock();
      assignn(t,cache.data,cache.size());
      cacheMutex.unlock();
    }

  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType>  
    void CachedSensor<T,SingleType,GroupType>::connect(SensorNotifier<T>* notifier){
      signal.connect(*notifier);
    }

  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType>  
    void CachedSensor<T,SingleType,GroupType>::disconnect(SensorNotifier<T>* notifier){
      signal.disconnect(*notifier);
    }

  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType>  
    void CachedSensor<T,SingleType,GroupType>::connect(helium::EhConnReference<const T*>& ref, const helium::HwIdList& what, helium::sensor::ReadType type){
      SensorNotifier<T>* notifier=new  TypedGroupSensorNotifier<T>(type,what,ref.cb,ref);
      connect(notifier);
      ref.disc=notifier;
    }

  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType>  
    void CachedSensor<T,SingleType,GroupType>::disconnect(helium::EhConnReference<const T*>& ref){
      disconnect(trust_cast<SensorNotifier<T>*>(ref.disc));
      ref.clearDisc();
    }

   template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType>   
  size_t CachedSensor<T,SingleType,GroupType>::size(){     
      return cache.size();
    }
  
  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType>  
  void CachedSensor<T,SingleType,GroupType>::update(const T* d,const HwIdList& read){
      signal(d,read);
    }

  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType>  
  void CachedSensor<T,SingleType,GroupType>::handle(const HwIdList& bad,exc::Exception& e,const T* data){
    signal.handle(bad,e,data);
  }

  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType>  
  CachedSensor<T,SingleType,GroupType>::CachedSensor(size_t n):cacheMutex("cachedSensor"),cache(n),singleCells(*this,n),groupCell(*this){
				    
    }      

  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType> 
  void DefDescr<CachedSensor<T,SingleType,GroupType> >::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    if (SingleType!= sensorProcessing::NO_CELL){
      members.push_back(scalarMapping(this->obj.groupCell,IDPath("group.value",".")));
    }
    
    if (GroupType!= sensorProcessing::NO_CELL){
      members.push_back(vectMapping(this->obj.singleCells,"sensor","value","num"));
    }
  }//getMapMembers

}
