/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SENSORSPOOLER
#define HE_SENSORSPOOLER

#include <helium/robot/sensorInterface.h>
#include <helium/spooling/spooler.h>
#include <helium/spooling/jobShouldRun.h>
#include <helium/container/bufferPipe.h>
#include <helium/system/basicCell.h>
#include <helium/system/cellDescription.h>
#include <helium/robot/hardwareException.h>
#include <helium/smartpointer/sharedPtr.h>



namespace helium{


  struct SensSpoolPriority{
    enum Priority{IDLE,NORMAL,HIGH};
    static const int PRIORITIES=3;
  };


  template<typename T>
  class SensorSpooler:public Spooler<sensor::Set<T> >{
    typedef sensor::Set<T> Sensor;
  public:       
    SensorSpooler(Sensor& psensor,bool autoStart=true);
    size_t size();
  };



  //base class for converting signal of a T* and the exceptions
  template<typename T>
  class SensorNotifier:public Callback<const T*>,//for the update signal
		       public ConnReference<const T*>,//for the update signal
		       public DisconnectInfo//for the external connReference
  {
  public:
    sensor::ReadType type;
    exc::ExceptionHandler& eh;
    
    virtual bool isAffected(const HwIdList& bad)=0; //if just one is bad, we throw away all.
    virtual bool canSignal(const HwIdList& read)=0;    
    
    void handle(const HwIdList& bad,exc::Exception& e,const T* data);
    SensorNotifier(sensor::ReadType ptype,exc::ExceptionHandler& peh);
    virtual ~SensorNotifier(){}
  };


  template<typename T> //this is threadsafe, similar to the EhSignal, but has the special handle function
  class SensorNotifierSignal:public  SignalType<const T*,true>::value{
  public:
    void connect(SensorNotifier<T>& sn);
    void operator()(const T* d,const HwIdList& read);    
    void handle(const HwIdList& bad,exc::Exception& e,const T* data);    
  };

  template<typename T>
  class MultipleSensorNotifier:public SensorNotifier<T>{//still abstract,no operator
  public: 
    HwIdList what;
    bool isAffected(const HwIdList& bad);
    bool canSignal(const HwIdList& read);
    MultipleSensorNotifier(sensor::ReadType ptype,const HwIdList& pwhat,exc::ExceptionHandler& eh);
    MultipleSensorNotifier(sensor::ReadType ptype,int n,exc::ExceptionHandler& eh);

  };
    


  template<typename T>
  class SingleSensorNotifier:public SensorNotifier<T>{
  protected:
    int idx;
    Cell::CellCallback* cb;     
    void operator()(const T* t);  
  public:
    bool canSignal(const HwIdList& read);    
    bool isAffected(const HwIdList& bad);
    SingleSensorNotifier(sensor::ReadType ptype,int pidx,Cell::CellCallback* pcb,exc::ExceptionHandler& eh);    
  };
  
  template<typename T>
  class SingleSensorChangeNotifier:public SingleSensorNotifier<T>{
    bool toUpdate;
    T prevVal;
    void operator()(const T* t);  
  public:
    SingleSensorChangeNotifier(sensor::ReadType ptype,int pidx,Cell::CellCallback* pcb,exc::ExceptionHandler& eh);   
  };
  


    //typed because it is not a cell
  template<typename T>
  class TypedSingleSensorNotifier:public SensorNotifier<T>{
  protected:
    int idx;
    Callback<T>* cb;
 
    bool canSignal(const HwIdList& read);
    void operator()(const T* t);
  public:
    bool isAffected(HwIdList& bad);
    TypedSingleSensorNotifier(sensor::ReadType ptype,int pidx,Callback<T>* pcb,exc::ExceptionHandler& eh);
   
  };


  template<typename T>
  class TypedSingleSensorChangeNotifier:public TypedSingleSensorNotifier<T>{
    bool toUpdate;
    T prevVal;
    void operator()(const T* t);  
  public:
    TypedSingleSensorChangeNotifier(sensor::ReadType ptype,int pidx,Callback<T>* pcb,exc::ExceptionHandler& eh);   
  };


  template<typename T>
  class GroupSensorNotifier:public MultipleSensorNotifier<T>{
    void operator()(const T* t);  
    SensorValList<T> g;
    Cell::CellCallback* cb;   
  public:
    GroupSensorNotifier(sensor::ReadType ptype,const HwIdList& pwhat,Cell::CellCallback* pcb,exc::ExceptionHandler& eh);
  };

  template<typename T>
  class GroupSensorChangeNotifier:public MultipleSensorNotifier<T>{    
    SensorValList<T> g;
    Cell::CellCallback* cb;   
    bool toUpdate;
    void operator()(const T* t);
  public:
    GroupSensorChangeNotifier(sensor::ReadType ptype,const HwIdList& pwhat,Cell::CellCallback* pcb,exc::ExceptionHandler& eh);
    
  };

  
  template<typename T>
  class WholeSensorNotifier:public MultipleSensorNotifier<T>{
    SensorValList<T> g;
    Cell::CellCallback* cb;

    void operator()(const T* t);
    bool isAffected(const HwIdList& bad);//overloading for optimization: always true    

    
  public:
    WholeSensorNotifier(sensor::ReadType ptype,int n,Cell::CellCallback* pcb,exc::ExceptionHandler& eh);
  };


  template<typename T>
  class WholeSensorChangeNotifier:public MultipleSensorNotifier<T>{
    SensorValList<T> g;
    Cell::CellCallback* cb;    
    bool toUpdate;
    void operator()(const T* t);
    bool isAffected(const HwIdList& bad);//overloading for optimization: always true    
  public:
    WholeSensorChangeNotifier(sensor::ReadType ptype,int n,Cell::CellCallback* pcb,exc::ExceptionHandler& eh);
  };
  

  //typed because it is not a cell
  template<typename T>
  class TypedGroupSensorNotifier:public MultipleSensorNotifier<T>{

    Callback<const T*> *cb;
    void operator()(const T* t);
  public:
    TypedGroupSensorNotifier(sensor::ReadType ptype,const HwIdList& pwhat,Callback<const T*> *pcb,exc::ExceptionHandler& eh);
  };


  template<typename T>
  class TypedGroupSensorChangeNotifier:public MultipleSensorNotifier<T>{
    Callback<const T*> *cb;
    SensorValList<T> prevVal;
    bool toUpdate;
    void operator()(const T* t);
   
  public:
    TypedGroupSensorChangeNotifier(sensor::ReadType ptype,const HwIdList& pwhat,Callback<const T*> *pcb,exc::ExceptionHandler& eh);
  };


  namespace sensorProcessing{
    enum CellType {NO_CELL,ALLUPDATES_CELL,CHANGES_CELL} ;

    template<typename A,typename B,typename C,CellType t> struct getType;
    
    template<typename A,typename B,typename C> struct getType<A,B,C,NO_CELL>{
      typedef A value;
    };

    template<typename A,typename B,typename C> struct getType<A,B,C,ALLUPDATES_CELL>{
      typedef B value;
    };

    template<typename A,typename B,typename C> struct getType<A,B,C,CHANGES_CELL>{
      typedef C value;
    };

    

  };
  
  
  

  template<typename T>
  class SensorProcessor:public Thread,public sensor::FilterInterface<T>
  {   
    typedef SensorSpooler<T> Spool;
    

    struct FailableData{
      Array<T> vals;
      exc::ExceptionHolder exc;
      SharedPtr<HwIdList> readData;
      FailableData(int n):vals(n){}
    };

    
    typedef BufferPipe<FailableData ,int> Pipe; //int is the parameter given to the constructor
    typedef sensor::Set<T> Sensor;


  

    class ReadWork:public Job<sensor::Set<T> >{      
      SensorProcessor& owner;
      SensSpoolPriority::Priority priority;
      Array<int> listeners; 
      SharedPtr<HwIdList> toRead; //this is shared wit the readData of each FailableData in the pipe
      JobShouldRun<sensor::Set<T> > jobState;
      typename sensor::Set<T>::Token readToken;      
      bool operator()(sensor::Set<T>& s);//push in the pipe
      bool isToCancel();      
      void update(sensor::Set<T>& s);//used by mark/unmark
    public:

      void mark(sensor::Set<T>& s,int idx);
      void mark(sensor::Set<T>& s,std::vector<int>& ids);      
      void unmark(sensor::Set<T>& s,int idx);
      void unmark(sensor::Set<T>& s,std::vector<int>& ids);
      ReadWork(SensorProcessor& powner,int n,typename SensSpoolPriority::Priority ppriority);
      
    };

     
    //single sensor
  protected:
    
    template<bool changeOnly>
    class SensorCell:public TypedCell<SensorCellSignature<T> >{
      SensorProcessor& owner;   
      int idx;
      std::string getName() const ;      
      void do_read(Sensor& s,T& t);
      void do_connect(sensor::Set<T>& s,SensorNotifier<T>* n);      
      void do_disconnect(sensor::Set<T>& s,SensorNotifier<T>* n);     
    protected:
      void anyConnect(SensorNotifier<T>* n,DisconnectInfo* &disc);
      void anyDisconnect(DisconnectInfo* &disc);    
    public:
      //Cell
      void connect(Cell::CellConnection& ref);
      void connect(Cell::CellConnection& ref,const sensor::ReadType& t);
      void disconnect(Cell::CellConnection& ref);

      //Type
      void getData(T& t);
      void connect(EhConnReference<T>& ref,const sensor::ReadType& t);
      void disconnect(EhConnReference<T>& ref);
      SensorCell(SensorProcessor& powner,int pidx);
    };


   
    template<bool changeOnly>
    class SensorGroupCell:public TypedCell<SensorGroupCellSignature<T> >{
      SensorProcessor& owner;              
      std::string getName() const ;
      
      //void readAll(Sensor& s,T* t);
      //      typedef std::pair<SensorValList<T>*,const HwIdList*> ReadGroupParam;
      //typedef std::pair<T*,const HwIdList*> VReadGroupParam;
      //void readGroup(Sensor& s,const ReadGroupParam& p);
      //void vreadGroup(Sensor& s,const VReadGroupParam& p);
      //void do_connect(sensor::Set<T>& s,MultipleSensorNotifier<T>* n);
      //void do_disconnect(sensor::Set<T>& s,MultipleSensorNotifier<T>* n);     
      //void anyConnect(MultipleSensorNotifier<T>* n,DisconnectInfo* &disc);
      //void anyDisconnect(DisconnectInfo* &disc);

    public:
      void getData(SensorValList<T>& g);
      void getData(T* t);
      void getData(SensorValList<T>& g,const HwIdList& l);
      void getData(T* g,const HwIdList& l);
      void connect(Cell::CellConnection& ref,const std::pair<HwIdList,sensor::ReadType>& t);
      void connect(Cell::CellConnection& ref);
      void disconnect(Cell::CellConnection& ref);    
      void connect(EhConnReference<const T*>& ref,const HwIdList& what, sensor::ReadType type);
      void disconnect(EhConnReference<const T*>& ref);
      SensorGroupCell(SensorProcessor& powner);
    };
    


  private:

    template<typename C,void (C::*f)()> void execAndHandle(C* c,T* data);


    struct CheckAndNotify{	
      typename Pipe::Node* n;	
      SensorNotifierSignal<T>& toNotify;
      void operator()();
      CheckAndNotify(SensorNotifierSignal<T>& ptoNotify);
    };
       
    void run();



    Spool& spool;      
    Pipe pipe;
    ReadWork* reads[sensor::READTYPES];
    SensorNotifierSignal<T> toNotify;
    bool running;
    
    void do_read(Sensor& s,T& t,int idx);
    void do_readAll(Sensor& s,T* t);
    void do_readGroup(Sensor& s,SensorValList<T>& v,const HwIdList& what);
    void do_vreadGroup(Sensor& s,T* v,const HwIdList& what);

    void do_connect(sensor::Set<T>& s,MultipleSensorNotifier<T>* n);
    void do_disconnect(sensor::Set<T>& s,MultipleSensorNotifier<T>* n);
    void anyConnect(MultipleSensorNotifier<T>* n,DisconnectInfo* &disc);
    void anyDisconnect(DisconnectInfo* &disc);
  public:
    T read(int idx);
    void read(T* t);
    void read(SensorValList<T>& v,const HwIdList& what);
    void read(T* t,const HwIdList& what);
    void connect(EhConnReference<const T*>& ref,const HwIdList& what, sensor::ReadType type);  
    void disconnect(EhConnReference<const T*>& ref);    
    SensorProcessor(Spool& pspool,bool autoStart=true);
    ~SensorProcessor();
    
  };


  template<typename T,
	   sensorProcessing::CellType SingleType=sensorProcessing::ALLUPDATES_CELL,
	   sensorProcessing::CellType  GroupType=sensorProcessing::ALLUPDATES_CELL>
  class ExportableSensorProcessor:public SensorProcessor<T>{
    template<bool changeOnly>
    struct SingleSensArray{
      
      typedef typename SensorProcessor<T>::template SensorCell<changeOnly> SensCell;
      typedef Array<SensCell,
		    array::RESIZABLE,
		    array::InternalAllocator<ParamIdxAllocator<SensCell,SensorProcessor<T>&,SensorProcessor<T>&> > >    
      Type;
    };
  public:
    //getType picks, the first, second or third type depending on the fourth value
    typename sensorProcessing::getType<Void,
				       typename SingleSensArray<false>::Type,
				       typename SingleSensArray<true>::Type,
				       SingleType
				       >::value     sensorCells;    
			      
    typename sensorProcessing::getType<Void,
				       typename SensorProcessor<T>::template SensorGroupCell<false>, 
				       typename SensorProcessor<T>::template SensorGroupCell<true>,				      		
				       GroupType
				       >::value sensorGroups; //can connect "typed"  

    ExportableSensorProcessor(SensorSpooler<T>& pspool,bool autoStart=true);
  };
  


  template<typename T,sensorProcessing::CellType SingleType,sensorProcessing::CellType GroupType> 
  class DefDescr<ExportableSensorProcessor<T,SingleType,GroupType> >:public Description<ExportableSensorProcessor<T,SingleType,GroupType> > {  
  public:
    DefDescr(ExportableSensorProcessor<T,SingleType,GroupType>& im):Description<ExportableSensorProcessor<T,SingleType,GroupType> >(im){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
};

}//helium

#include <helium/robot/sensorSpooler.hpp>

#endif
