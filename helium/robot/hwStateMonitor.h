/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_HWSTATEMONITOR
#define HE_HWSTATEMONITOR

#include <helium/core/slContainer.h>
#include <helium/signal/wrapInfo.h>
#include <helium/system/signalledArray.h>
#include <helium/system/cellSignature.h>
#include <helium/system/cellSignal.h>
#include <helium/system/basicCell.h>
#include <helium/spooling/spooler.h>
#include <helium/robot/hwInterface.h>
#include <helium/robot/hardwareException.h>
#include <helium/core/triple.h>


namespace helium{
  
  


  /*struct HwStateSignature:public CellSignature{
    typedef HwState GT;
    typedef bool GP; //force check;
    typedef HwState ST;
    typedef HwState SP;
    typedef HwState UT;
    typedef Void UP;
    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;
    typedef DefaultAccessPolicy<GT,GP,ST,SP,UT,UP,
				getRequiresParam,setRequiresParam,connectRequiresParam> Access;
    
				};*/


  enum MonitorMode{NOHWMON,FAULTYHWMON,IDLEHWMON,UNKNOWNHWMON};
 
    template<>  struct EnumStrings<MonitorMode>{
    static const int NUM=4;
    static const char* strings[NUM];
  };
  


  struct HwStateGroupSignature:public CellSignature{
    typedef Array<HwState> GT;
    typedef HwIdList GP;
    typedef Void ST;
    typedef Void SP;
    typedef std::pair<int,HwState> UT;
    typedef HwIdList UP;
    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;
  };

  template<>
  struct DefaultAccessOptions<HwStateGroupSignature>{
    static const int value=UPDATEUSESREFERENCE;
  };


  struct HwMonitorModeGroupSignature:public CellSignature{
    typedef Array<MonitorMode> GT;
    typedef HwIdList GP;    
    typedef Array<std::pair<int,MonitorMode> > ST;
    typedef Void SP;
    typedef std::pair<int,MonitorMode> UT;
    typedef HwIdList UP;
    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;
  };

  template<>
  struct DefaultAccessOptions<HwMonitorModeGroupSignature>{
    static const int value=UPDATEUSESREFERENCE;
  };
  
  
  struct HwStateSingleSignature:public CellSignature{
    typedef HwState GT;
    typedef Void GP;

    typedef Void ST;
    typedef Void SP;
    
    typedef HwState UT; 
    typedef Void UP;
    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;

    
  };

  struct HwMonitorModeSingleSignature:public CellSignature{
    typedef MonitorMode GT;
    typedef Void GP;

    typedef MonitorMode ST;
    typedef Void SP;
    
    typedef MonitorMode UT; 
    typedef Void UP;
    static const bool getRequiresParam=false;
    static const bool setRequiresParam=false;
    static const bool connectRequiresParam=false;    
  };





  template<typename Resource,	 
	   typename Priorities
	   >
  class HwStateMonitor:public Job<Resource>,
		       public exc::ExceptionHandler
  {

    typedef Spooler<Resource> Spool;

    Spool& spool;
    HwInterface& hwRes;
    SignalledArray<HwState> state;
    SignalledArray<MonitorMode> monitorMode;
    //Mutex access;
    HwIdList wholeList;


    
    HwIdList sensOnCheck;


    Array<HwInterface::WorkingToken> onCheckToken;

    int onCheck; //the curson running on onCheckToken; -1 indicates no token


     std::string name;



    class HwStateGroupCell:public TypedCell<HwStateGroupSignature>{
      HwStateMonitor& owner;

      std::string getName() const{
	return owner.name+" group state";	
      }


      typedef std::pair<Array<HwState>*,const HwIdList*> GetLParam;


      void do_getLData(const GetLParam &mwhat){
	//owner.access.lock();
	
	owner.hwRes.isWorking(*mwhat.first,*mwhat.second);	
	owner.updateState(*mwhat.second,*mwhat.first);
	//owner.access.unlock();
      }

      friend class StackJob<Resource,HwStateGroupCell,
			    const GetLParam&,
			    onlyParam<const GetLParam&>,
			    &HwStateGroupCell::do_getLData,true>;



      typedef triple<Cell::CellConnection*,const HwIdList*,MultiConnDinfo<HwIdList>* > ConnectLParam;
      
      void do_connect (const ConnectLParam& cl){
	Cell::CellConnection& ref=*cl.first;
	const HwIdList& l=*cl.second;
	bool toUpdate=false;
	for (size_t i=0;i<l.ids.size();i++){
	  int idx=l.ids[i];
	  toUpdate=toUpdate|owner.state.connect(cl.third->dinfo[idx],ref.cb,idx);
	}
	if (toUpdate){
	  owner.updateToken();
	}
      }

      void do_disconnect(MultiConnDinfo<HwIdList>* disc){
	bool toUpdate=false;
	for (size_t i=0;i<disc->dinfo.size();i++){
	  toUpdate=toUpdate|owner.state.disconnect(disc->dinfo[i],disc->add.ids[i]);
	}	
	if (toUpdate){
	  owner.updateToken();
	}
      }


    public:

      void getData(Array<HwState>& m,const HwIdList &what){
	m.resize(what.ids.size());
	StackJob<Resource,HwStateGroupCell,
		 const GetLParam&,
		 onlyParam<const GetLParam&>,
		 &HwStateGroupCell::do_getLData,true> job(this,GetLParam(&m,&what));
	owner.spool.waitJob(&job,Priorities::NORMAL);
	
      }

      void getData(Array<HwState>& m){
	getData(m,owner.wholeList);
      }


      void connect(Cell::CellConnection& ref,const HwIdList& l){
	MultiConnDinfo<HwIdList>* p=new MultiConnDinfo<HwIdList>(l.ids.size(),l);
	StackJob<Resource,HwStateGroupCell,
		 const ConnectLParam&,
		 onlyParam<const ConnectLParam&>,
		 &HwStateGroupCell::do_connect,true> job(this,ConnectLParam(&ref,&l,p));
	owner.spool.waitJob(&job,Priorities::HIGH);
	ref.disc=p;
      }

      void connect(Cell::CellConnection& ref){
	connect(ref,owner.wholeList);
      }

      void disconnect(Cell::CellConnection& ref){
	MultiConnDinfo<HwIdList>* disc=trust_cast<MultiConnDinfo<HwIdList>*>(ref.disc);	       
	StackJob<Resource,HwStateGroupCell,
		 MultiConnDinfo<HwIdList>*,
		 onlyParam<MultiConnDinfo<HwIdList>*>,
		 &HwStateGroupCell::do_disconnect,true> 
	job(this,disc);
      
      owner.spool.waitJob(&job,Priorities::HIGH);
      
      ref.clearDisc();

      
      }
      
      HwStateGroupCell(HwStateMonitor& powner):owner(powner){
      }

    };


    class HwMonitorModeGroupCell:public TypedCell<HwMonitorModeGroupSignature>{
      HwStateMonitor& owner;

      std::string getName() const{
	return owner.name+" group monitor mode";	
      }
      
      typedef std::pair<Array<MonitorMode>*,const HwIdList*> GetParam;
      
      void do_getData(const GetParam& p){
	for (size_t i=0; i<p.second->ids.size();i++){
	  (*p.first)[i]=owner.monitorMode[i]; //this is the mode, not the state
	}
      }

      typedef triple<Cell::CellConnection*,const HwIdList*,MultiConnDinfo<HwIdList>* > ConnectParam;

      void do_connect(const ConnectParam& cl){
	Cell::CellConnection& ref=*cl.first;
	const HwIdList& l=*cl.second;
	for (size_t i=0;i<l.ids.size();i++){
	  int idx=l.ids[i];
	  owner.monitorMode.connect(cl.third->dinfo[idx],ref.cb,idx);
	}
      }


      void do_disconnect(MultiConnDinfo<HwIdList>* disc){
	for (size_t i=0;i<disc->dinfo.size();i++){
	  owner.state.disconnect(disc->dinfo[i],disc->add.ids[i]);
	}
      }

    public:

      void getData(Array<MonitorMode>& m,const HwIdList &what){
	m.resize(what.ids.size());
	StackJob<Resource,HwMonitorModeGroupCell,
		 const GetParam&,
		 onlyParam<const GetParam&>,
		 &HwMonitorModeGroupCell::do_getData,true>
	job(this,GetParam(&m,&what));
	owner.spool.waitJob(&job,Priorities::HIGH);
      }
      
      void getData(Array<MonitorMode>& m){
	getData(m,owner.wholeList);
      }
      
      void setData(const Array<std::pair<int,MonitorMode> >& m){
	StackJob<Resource,HwStateMonitor,
	  const Array<std::pair<int,MonitorMode> >&,
	  onlyParam<const Array<std::pair<int,MonitorMode> >&>,
		 &HwStateMonitor::setMonitorMode,true>
	  job(&owner,m);
	owner.spool.waitJob(&job,Priorities::HIGH);
      }
      

      void connect(Cell::CellConnection& ref,const HwIdList& l){
	MultiConnDinfo<HwIdList>* p=new MultiConnDinfo<HwIdList>(l.ids.size(),l);
	StackJob<Resource,HwMonitorModeGroupCell,
	  const ConnectParam&,
		 onlyParam<const ConnectParam&>,
		 &HwMonitorModeGroupCell::do_connect,true>
	  job(this,ConnectParam(&ref,&l,p));	
	owner.spool.waitJob(&job,Priorities::HIGH);
	ref.disc=p;
      }

      void connect(Cell::CellConnection& ref){
	connect(ref,owner.wholeList);
      }

      void disconnect(Cell::CellConnection& ref){
	MultiConnDinfo<HwIdList>* disc=trust_cast<MultiConnDinfo<HwIdList>*>(ref.disc);	       
	StackJob<Resource,HwMonitorModeGroupCell,
		 MultiConnDinfo<HwIdList>*,
		 onlyParam<MultiConnDinfo<HwIdList>*>,
		 &HwMonitorModeGroupCell::do_disconnect,true>
	  job(this,disc);		
	owner.spool.waitJob(&job,Priorities::HIGH);
	ref.clearDisc();
      }
      
      HwMonitorModeGroupCell(HwStateMonitor& powner):owner(powner){
      }

    };

    

    class HwStateSingleCell:public TypedCell<HwStateSingleSignature>{
      HwStateMonitor& owner;
      int id;

      std::string getName() const{
	return owner.name+toString(id)+" state";	
      }
            
      void do_getData(HwState* s){
	//std::cout<<getSystemMillis()<<"asking if working"<<std::endl;
	*s=owner.hwRes.isWorking(id);
	//std::cout<<getSystemMillis()<<"got"<<*s<<std::endl;
	owner.updateState(id,*s);
      }

      void do_connect(Cell::CellConnection& ref){
	owner.state.abbrvConnect(ref,id);
      }

       void do_disconnect(Cell::CellConnection& ref){
	 owner.state.abbrvDisconnect(ref,id);
      }
     
      
      void getData(HwState& s){
	StackJob<Resource,HwStateSingleCell,
		 HwState*,
		 onlyParam<HwState*>,
		 &HwStateSingleCell::do_getData,true> job(this,&s);
	//std::cout<<getSystemMillis()<<" created requester job"<<std::endl;	
	owner.spool.waitJob(&job,Priorities::NORMAL);
	//std::cout<<getSystemMillis()<<" requester job completed"<<std::endl;	

	//std::cout<<getSystemMillis()<<" updated state"<<std::endl;	
      }
      

      void connect(Cell::CellConnection& ref){
	StackJob<Resource,HwStateSingleCell,
		 Cell::CellConnection&,
		 onlyParam<Cell::CellConnection&>,
		 &HwStateSingleCell::do_connect,true
		 
		 >
	  job(this,ref);
	owner.spool.waitJob(&job,Priorities::HIGH);	
      }

      void disconnect(Cell::CellConnection& ref){
	StackJob<Resource,HwStateSingleCell,
		 Cell::CellConnection&,
		 onlyParam<Cell::CellConnection&>,
		 &HwStateSingleCell::do_disconnect,true>
	  job(this,ref);
	owner.spool.waitJob(&job,Priorities::HIGH);	
      }
      
    public:
      
      HwStateSingleCell(HwStateMonitor& powner,int pid):
	owner(powner),id(pid){
      }
      
    };//HwSingleStateCell


    class HwMonitorModeSingleCell:public TypedCell<HwMonitorModeSingleSignature>{
      HwStateMonitor& owner;
      int id;

      std::string getName() const{
	return owner.name+" monitor mode";	
      }
      


      void do_getData(MonitorMode& m){
	m=owner.monitorMode[id];
      }
      
      void do_setData(const MonitorMode& m){
	owner.setMonitorMode(id,m);
      }

      void do_connect(Cell::CellConnection& ref){
	owner.monitorMode.abbrvConnect(ref,id);
      }

      void do_disconnect(Cell::CellConnection& ref){
	owner.monitorMode.abbrvDisconnect(ref,id);
      }

    public:

      void getData(MonitorMode& m){
	StackJob<Resource,HwMonitorModeSingleCell,
		 MonitorMode&,
		 onlyParam<MonitorMode&>,		 
		 &HwMonitorModeSingleCell::do_getData,true>
	  job(this,m);
	owner.spool.waitJob(&job,Priorities::HIGH);		
      }
      

      void setData(const MonitorMode& m){
	StackJob<Resource,HwMonitorModeSingleCell,
		 const MonitorMode&,
		 onlyParam<const MonitorMode&>,		 
		 &HwMonitorModeSingleCell::do_setData,true>
	  job(this,m);
	owner.spool.waitJob(&job,Priorities::HIGH);	
      }      

      void connect(Cell::CellConnection& ref){
	StackJob<Resource,HwMonitorModeSingleCell,
		 Cell::CellConnection&,
		 onlyParam<Cell::CellConnection&>,
		 &HwMonitorModeSingleCell::do_connect,true>
	  job(this,ref);
	owner.spool.waitJob(&job,Priorities::HIGH);	
      }

      void disconnect(Cell::CellConnection& ref){
	StackJob<Resource,HwMonitorModeSingleCell,
		 Cell::CellConnection&,
		 onlyParam<Cell::CellConnection&>,
		 &HwMonitorModeSingleCell::do_disconnect,true>
	  job(this,ref);
	owner.spool.waitJob(&job,Priorities::HIGH);	
      }
      
    HwMonitorModeSingleCell(HwStateMonitor& powner,int pid):owner(powner),id(pid){
      }

    };
 

  private:

    void setMonitorMode(int id,MonitorMode m){
      bool up=(monitorMode[id]!=m);
      monitorMode.set(id,m);      
      if (up){
	updateToken();
      }
    }


    void setMonitorMode(const Array<std::pair<int,MonitorMode> >& m){
      bool up=false;
      for (size_t i=0;i<m.size();i++){
	int id=m[i].first;
	if (monitorMode[id]!=m[i].second){
	  up=true;
	  monitorMode.set(id,m[i].second);      
	}
      }
      if (up){
	updateToken();
      }
    }

    friend class StackJob<Resource,HwStateMonitor,
			  const Array<std::pair<int,MonitorMode> >&,
			  onlyParam<const Array<std::pair<int,MonitorMode> >&>,
			  &HwStateMonitor::setMonitorMode,true>;
  


        
       
      
   bool operator()(Resource& r){

     struct Notified:public Callback<const HwStateNotification&>{
       HwIdList ids; //this will be a subset of sensOnCheck
       std::vector<HwState> state;
       
       void operator()(const HwStateNotification& n){
	 ids.ids.push_back(n.first);	 
	 state.push_back(n.second);	 
       }
     };
     
     if (onCheckToken.size()>0){
       onCheck=onCheck%onCheckToken.size();
       Notified notified;     
       hwRes.isWorking(&notified,onCheckToken[onCheck]);
       updateState(notified.ids,notified.state);
       onCheck++;      
     }     
     return onCheck==-1;
   }


    bool isToCancel(){
      return onCheck==-1;      
    }



    void updateToken(){
      bool wasStopped=(onCheck==-1);
      sensOnCheck.ids.clear();
      for (size_t i=0;i<monitorMode.size();i++){
	if (monitorMode[i]==IDLEHWMON||
	    (monitorMode[i]==FAULTYHWMON&&isFaulty(state[i]))){
	  sensOnCheck.ids.push_back(i);
	}
      }
      if (sensOnCheck.ids.size()>0){
	hwRes.getWorkingOptimizedSplit(onCheckToken,sensOnCheck);	
	if (wasStopped){
	  onCheck=0;
	  spool.addJob(this,Priorities::IDLE);
	}
      }else{
	onCheck=-1;
      }
    }


    void updateState(int id,HwState s){ 
      if (state[id]!=s){
	state.set(id,s);
	if(monitorMode[id]==FAULTYHWMON){
	  updateToken();
	}
      }      
    }

    template<typename V>
    void updateState(const HwIdList& l,const V& s){
      bool up=false;
      for (size_t i=0;i<l.ids.size();i++){
	int id=l.ids[i];
	if (state[id]!=s[i]){//s is a subset of length l.size()
	  state.set(id,s[i]);
	  up=up||(monitorMode[id]==FAULTYHWMON);
	}
      }
      if (up){
	updateToken();
      }
    }


    

    template<typename V>
    void updateScalarState(const HwIdList& l,const V& s){


      bool up=false;
      for (size_t i=0;i<l.ids.size();i++){
	int id=l.ids[i];
	if (state[id]!=s){//s is a subset of length l.size()
	  state.set(id,s);
	  up=up||(monitorMode[id]==FAULTYHWMON);
	}
      }
      if (up){
	updateToken();
      }
    }



  protected:
    
    
  public:
    typedef HwStateMonitor<Resource,Priorities> Smonitor;
  typedef Array<HwStateSingleCell,
		array::RESIZABLE,
		array::InternalAllocator<ParamIdxAllocator<HwStateSingleCell,Smonitor&,Smonitor& > > > StateCellArray;
  typedef HwStateGroupCell StateGroupCell;
  

  typedef Array<HwMonitorModeSingleCell,
		array::RESIZABLE,
		array::InternalAllocator<ParamIdxAllocator<HwMonitorModeSingleCell,Smonitor&,Smonitor& > > > MonitorModeCellArray;
  typedef HwMonitorModeGroupCell MonitorModeGroupCell;
 


    /*void notifyState(int id,HwState s){
      access.lock();      
      updateState(id,s);
      access.unlock();
      }

   
    
    template<typename V>
    void notifyState(const HwIdList& l,const V& s){
      access.lock();
      updateState(l,s);
      access.unlock();
      }


    template<typename V>
    void notifyScalarState(const HwIdList& l,const V& s){
      StackJob<HwStateMonitor, Resource,
	const std::pair<const HwIdList*,const V*>&,
	const std::pair<const HwIdList*,const V*>&,
	       &updateScalarState<V>,true>
	job(this,std::make_pair(&l,&s));
      job.

    }
    
    */
    


    void handle(exc::Exception& ex){
      //std::cout<<"I am here"<<std::endl;
      try{
	ex.rethrow();
      }catch(exc::HardwareDeviceException<UNNAMEDDEVICE>& e){
	updateScalarState(wholeList,e.getState());
      }catch(exc::HardwareDeviceException<NAMEDDEVICE>& e){
	updateState(e.id,e.getState());
      }catch(exc::HardwareDeviceException<MULTIPLEXINGDEVICE>& e){
	updateScalarState(e.affected,e.getState());
      }catch(exc::HardwareDeviceException<NAMEDMULTIPLEXINGDEVICE>& e){
	updateScalarState(e.affected,e.getState());
      }catch(exc::MultipleException& e){
	e.handleUsing(this);
      }catch(exc::Exception& e){
	std::cerr<<"[Unexpected] "<<e<<std::endl;
	//notifyScalarState(wholeList,GENERICFAULTYHWSTATE);
      }
    }




  HwStateMonitor(Spool& pspool, HwInterface& phwRes,size_t pn): //the hwinterface is used only in the spooling thread, 
                                                             //instread of the real resource
      spool(pspool),hwRes(phwRes),state(pn),monitorMode(pn),
      //access("hweh"),
      wholeList(pn),
      onCheck(-1),name("state")
    {
      state.setAll(UNKNOWNHWSTATE);
      monitorMode.setAll(FAULTYHWMON);
      wholeList.fillConsecutive();
    }

  };

}//helium


#endif
