#ifndef HE_GENERICFILTER
#define HE_GENERICFILTER

#include <helium/robot/sensorInterface.h>
#include <helium/robot/sensorStructures.h>
#include <helium/signal/connectableSignal.h>
#include <helium/signal/ehSignal.h>
#include <helium/system/basicCell.h>

namespace helium{
 

 
  namespace sensor{
 

    
    template<typename R>
    class FilterConnInfo:public DisconnectInfo{
	//DisconnectInfo* disc;
      public:
	HwIdList what;
	sensor::ReadType type;	
	exc::ExceptionHandler& eh;
     
	Callback<const R*>* cb;

	typename std::list<FilterConnInfo*>::iterator it;

	void handle(const HwIdList& bad,exc::Exception& e,const R*);		
	FilterConnInfo(const HwIdList&,sensor::ReadType,exc::ExceptionHandler& eh,Callback<const R*>*);
      };//DiscInfo

    



    template<typename R>
    class GroupCellFilterConnInfo:public Callback<const R*>,
				  public FilterConnInfo<R>{		
      SensorValList<R> vals;
      Cell::CellCallback* ccb;
      void operator()(const R*);
    public:
      GroupCellFilterConnInfo(const HwIdList&,sensor::ReadType,Cell::CellConnection&);
      
    };
    
    template<typename R>
    class SingleCellFilterConnInfo:public Callback<const R*>,
				   public FilterConnInfo<R>{		

	Cell::CellCallback* ccb;
	void operator()(const R*);
      public:
	SingleCellFilterConnInfo(int id,sensor::ReadType,Cell::CellConnection&);

      };

    
    template<typename R>
    class OnChangeGroupCellFilterConnInfo:public Callback<const R*>,
					  public FilterConnInfo<R>{		
      SensorValList<R> vals;
      Cell::CellCallback* ccb;
      bool toUpdate;
      void operator()(const R*);
    public:
      OnChangeGroupCellFilterConnInfo(const HwIdList&,sensor::ReadType,Cell::CellConnection&);
      
    };
    
    template<typename R>
    class OnChangeSingleCellFilterConnInfo:public Callback<const R*>,
					   public FilterConnInfo<R>{		
	Cell::CellCallback* ccb;
      R val;
      bool toUpdate;
      void operator()(const R*);
      public:
      OnChangeSingleCellFilterConnInfo(int id,sensor::ReadType,Cell::CellConnection&);

      };


    
   
      

      





    template<typename R,typename T>
    class GenericFilterComputation{
    public:
      virtual void compute(R* r,const T* s,const HwIdList& what)=0;
      virtual void getRequired(HwIdList& req,const HwIdList& id)=0;//transform from filter ids to src ids
      virtual void getGenerated(HwIdList& gen,const HwIdList& id)=0;//transform from ids to the data generated, to know who to call when problems occurr
    };


    template<typename R,typename T>
    class GenericFilter:
			public FilterInterface<R>,
			public TypedCell<SensorGroupCellSignature<R> >{
      
      Mutex rinfoAccess;     
      Mutex tmpAccess; 

      

      friend class ReadInfo;
      FilterInterface<T>& src;


      GenericFilterComputation<R,T>& comp;

     
      Array<R> res;      //tmp:data computed, should be initialized soon to have resSize() working
      Array<T> sdata;    //tmp:data read from the source 




      HwIdList rAll,sAll;



      class FilterSignal{
	std::list<FilterConnInfo<R>*> conns; //the ownwer.tmp lock is used to protect this
	GenericFilter& owner;
      public:
	FilterSignal(GenericFilter& owner);
	void operator()(const R*);
	void handle(const HwIdList& bad,exc::Exception& e,const R*);
	void connect(FilterConnInfo<R>* c);
	void disconnect(FilterConnInfo<R>* c);	
      };      
      
      class ReadInfo:public Callback<const T*>,
		     public exc::ExceptionHandler
      {     
	GenericFilter& owner;
	sensor::ReadType type;	
	HwIdList resList,srcList;
	EhPtrConnReference<const T*> ref;
	Array<int> rcnt;      		
	void update();
	FilterSignal toNotify;
	void mark(const HwIdList& sl);	
	void unmark(const HwIdList& sl);

      public:
	void operator()(const T* t);
	void handle(exc::Exception& e);
	ReadInfo(GenericFilter& powner,sensor::ReadType ptype);	
	~ReadInfo();
	//void mark(int x);	
	//void unmark(int x);
	void connect(FilterConnInfo<R>* cinfo);
	void disconnect(FilterConnInfo<R>* cinfo);

      };
      
      
      
      
      Array<ReadInfo,
	    sensor::READTYPES,
	    array::InternalAllocator<ParamFuncAllocator<ReadInfo,GenericFilter&,sensor::ReadType,sensor::getReadType> > > rinfo;

      
     
      
      void operator()(const T* t);
      void handle(exc::Exception& e);//when it partially fails
      
     
     

      void updateRes(const HwIdList& toComp,const HwIdList& toRead);


      void anyConnect(FilterConnInfo<R>& cinfo);
      void anyDisconnect(FilterConnInfo<R>* disc);

      /*Cell stuff*/

      std::string getName() const;
      
      void getData(SensorValList<R>& t);

      void getData(SensorValList<R>& t,const HwIdList& p);

      void connect(Cell::CellConnection& ref);
      
      void connect(Cell::CellConnection& ref,const std::pair<HwIdList, sensor::ReadType>& p);


    public:
      void connect(Cell::CellConnection& ref,sensor::ReadType type,int id);//"hook" for the creation of single cells

      void disconnect(Cell::CellConnection &ref);
      
           
      
      
      void connect(EhConnReference<const R*> &ref,const HwIdList& what, sensor::ReadType type);
      
      void connect(EhConnReference<const R*> &ref,const sensor::ReadType type);

      void disconnect(EhConnReference<const R*> &ref);
            
      R read(int idx);      
      void read(R* r);
      void read(R* r,const HwIdList& what);

      void read(SensorValList<R>& v,const HwIdList& what);
            
      GenericFilter(GenericFilterComputation<R,T>& pcomp,int rn,FilterInterface<T>& psrc,int sn);

      size_t  srcSize() const;
      size_t  resSize() const;
           
    };//GenericFilter


    
    template<typename R,typename T>
    class SingleCellAdapter{
      
      
      class SingleCell:public TypedCell<SensorCellSignature<R> >{
	GenericFilter<R,T>& owner;
	int idx;
	
	std::string getName() const {
	  return "filter single value";
	}
	
	void getData(R& t){
	  t=owner.read(idx);
	}
	
	void connect(Cell::CellConnection& ref){
	  owner.connect(ref,sensor::READ,idx);
	}

	void connect(Cell::CellConnection& ref,const sensor::ReadType& type){
	  owner.connect(ref,type,idx);
	}
	
	void disconnect(Cell::CellConnection& ref){

	  owner.disconnect(ref);
	}

      public:
	SingleCell(GenericFilter<R,T>& powner,int pidx):owner(powner),idx(pidx){}
      };
    public:
      Array<SingleCell,
	    array::RESIZABLE,
	    array::InternalAllocator<ParamIdxAllocator<
				       SingleCell,GenericFilter<R,T>& > > > cells;
      
      

      
      SingleCellAdapter(GenericFilter<R,T>& f):cells(f,f.resSize()){}
    };//SingleCellAdapter


          class MainWT;

      class SecondaryWT:public Deletable,public Callback<const HwStateNotification&>{
	MainWT& owner;
	
	HwInterface::WorkingToken original;
      protected:
	Callback<const HwStateNotification&>* cb;

      public:
	void setOriginal(HwInterface::WorkingToken& o){
	  original=o;
	}
	
	const HwInterface::WorkingToken& getOriginalToken() const{
	  return original;
	}

	SecondaryWT(MainWT& powner):owner(powner){
	}


	SecondaryWT(MainWT& powner,HwInterface::WorkingToken& poriginal):owner(powner),original(poriginal){
	}
	
	void operator()(const HwStateNotification& sn);
	void setCb(Callback<const HwStateNotification&>* pcb){
	  cb=pcb;
	}
      };

      class MainWT:public SecondaryWT{


      

	friend class SecondaryWT;

	void mark(const HwStateNotification& sn){
	  if (sn.second!=WORKINGHWSTATE){
	    const std::vector<int>& v=srcToGen[sn.first];
	    for (size_t i=0;i<v.size();++i){
	      state[v[i]]=GENERICFAULTYHWSTATE;
	    }
	  }
	}

      public:
	std::map<int,HwState> state;  
	std::map<int,std::vector<int> > srcToGen;
	
	MainWT():SecondaryWT(*this){
	}

	void operator()(const HwStateNotification& sn){
	  SecondaryWT::operator()(sn);
	  for (std::map<int,HwState>::iterator it=state.begin();
	       it!=state.end();
	       ++it){
	    (*cb)(*it);//a pair = a HwStateNotification
	  }
	 
	  for(std::map<int,HwState>::iterator it=state.begin();
	       it!=state.end();
	       ++it){
	  it->second=WORKINGHWSTATE;
	 	  
	  }

	  setAll(state,WORKINGHWSTATE);
	  //sn.first,sn.second
	}
      };

    inline void SecondaryWT::operator()(const HwStateNotification& sn){
      owner.mark(sn);
    }



    //Single because it will only consider 1 generic filter,and not "walk up" to the real hw(specified as the first param of the constructor)
    //the generic filter computation getRequired() is used to determine the correct ids to be check for knowning the working state
    template<typename R,typename T>
    class SingleGenFilterHardware:public HwInterface{
      HwInterface& hw;
      GenericFilterComputation<R,T>& idmap;

      
    public:
      SingleGenFilterHardware(HwInterface& phw,GenericFilterComputation<R,T>& comp):
      hw(phw),idmap(comp)
      {
      }

      HwState isWorking(int id){
	SingleHwIdList what(id);
	HwIdList req;
	idmap.getRequired(req,what);
	Array<HwState> state;
	hw.isWorking(state,req);	
	if (containsOtherThan(state,WORKINGHWSTATE)){
	  return GENERICFAULTYHWSTATE;
	}
	return WORKINGHWSTATE;
      }
      
      void isWorking(Array<HwState>& s,const HwIdList& l){
	std::map<int,std::vector<int> > affects; //the ids in the vector are indexes in l/s
                                                 //the map index is a sensor id
	
	for (size_t i=0;i<l.ids.size();++i){
	  HwIdList req;
	  idmap.getRequired(req,l.ids[i]);
	  for (size_t j=0;j<req.ids.size();++j){
	    affects[req.ids[j]].push_back(i);
	  }//for j
	}

	HwIdList toCheck;
	assignKeys(toCheck.ids,affects);
	Array<HwState> state(toCheck.ids.size());
	hw.isWorking(state,toCheck);
	s.resize(l.ids.size());
	setAll(s,WORKINGHWSTATE);
	for (size_t i=0;i<state.size();++i){
	  if (state[i]!=WORKINGHWSTATE){
	    const std::vector<int>& v=affects[toCheck.ids[i]];
	    for (size_t j=0;j<v.size();++j){
	      s[j]=GENERICFAULTYHWSTATE;
	    }
	  }	  
	}	
      }//isWorking array<hwstate>, hwidlist



      void getWorkingOptimizedSplit(Array <WorkingToken>& tk,const HwIdList &l) const{
	std::set<int> toCheckS;

	MainWT* mwt=new MainWT();	  

	for (size_t i=0;i<l.ids.size();++i){
	  HwIdList req;
	  int idx=l.ids[i];
	  idmap.getRequired(req,idx);
	  toCheckS.insert(req.ids.begin(),req.ids.end());
	  mwt->state[idx]=WORKINGHWSTATE;
	  for (size_t j=0;j<req.ids.size();++j){
	    mwt->srcToGen[req.ids[j]].push_back(idx);
	  }
	}
	HwIdList toCheck;
	assign(toCheck.ids,toCheckS);
	hw.getWorkingOptimizedSplit(tk,toCheck);

	if (tk.size()>0){	  
	  for (size_t i=0;i<tk.size()-1;++i){
	    tk[i]=new SecondaryWT(*mwt,tk[i]);
	  }//for i
	  tk[tk.size()-1]=mwt;
	  mwt->setOriginal(tk[tk.size()-1]);	  
	}else{
	  delete mwt;
	}//if tk.size(
      }

      void isWorking(Callback<const HwStateNotification&>* cb,const WorkingToken& tk){
	SecondaryWT* swt=trust_cast<SecondaryWT*>(tk.get());
	 swt->setCb(cb);
	hw.isWorking(swt,swt->getOriginalToken());
      }



    };//SingleGenFilterHarware



    
   
  }//sensor




}//helium

#include <helium/robot/genericFilter.hpp>

#endif


      /*class GroupCBAdapter:public DiscInfo,public Callback<const R*>,public EhPtrConnReference<const R*>{
	SensorValList<R> data;
	Cell::CellCallback* cb;
	
	void operator()(const R* r);
      public:
	GroupCBAdapter(const HwIdList& what,sensor::ReadType type,Cell::CellConnection& pcb);
      };//GroupCBAdapter
      */
