#ifndef HE_TRIGGERCELL
#define HE_TRIGGERCELL

#include <helium/core/spooling.h>
#include <helium/system/basicCell.h>

namespace helium{




  template <typename T,typename C>
  class StdTriggerCell: public TypedCell<T>,public JobTrigger<T,C>,public Callback<Cell::CellSignalData>{
    T lastVal;
    Signal<Cell::CellSignalData> onSet;
  public:

    std::string getName(){
      return "StdTriggerCell";
    }
    
    
    void getData(T& t){
      t=lastVal;
    }

    void operator()(Cell::CellSignalData d){
      lastVal=*trust_cast<T*>(d);
    }

    Callback<Cell::CellSignalData>* clone() const{
      return NULL;

    }

    class ASignaller:public Callback<void>{
      T t;
      Signal<Cell::CellSignalData> &onSet;
    public:

      ASignaller(const T& pt,Signal<Cell::CellSignalData> &ponSet):
	t(pt),onSet(ponSet){
      }
      void operator()(){
	onSet(trust_cast<DataP>(&t));
	delete this;
      }
      Callback<void>* clone() const{
	return new ASignaller(t,onSet);
      }
    };


    void setData(const T& t){     
      asynchSetAndSignal(t,new ASignaller(t,onSet));
    }

    void setData(StdReply& r,const T& t){
      synchSet(t);
      onSet((Cell::CellSignalData)&t);
    }

    
    
    void connect(Cell::CellConnection& ref){
      onSet.connect(ref);
    }

    void disconnect(Cell::CellConnection& ref){
      onSet.disconnect(ref);
    }

    ConnReference<Cell::CellSignalData> conn;
    StdTriggerCell(JobManager* jm,C* c,typename JobTrigger<T,C>::JobCreatorF f,const T& initVal):      
      JobTrigger<T,C>(jm,c,f),lastVal(initVal),conn(this){     
      // std::cout<<"connected "<<&conn<<std::endl;
      onSet.connect(conn);
    }

    virtual ~StdTriggerCell(){
      //std::cout<<"disconnecting "<<&conn<<std::endl;
      onSet.disconnect(conn);
    }


  };


  class StdTriggerCopyJob:public Job{   //everything is done by the deconstructor
  public:
 
    void operator()(){
    }
      
  };



};//helium



#endif
