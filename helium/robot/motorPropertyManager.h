/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#ifndef HE_MOTORPROPERTYMAGER
#define HE_MOTORPROPERTYMAGER

#include <map>
#include <helium/robot/motorSpooler.h>
#include <helium/signal/wrapInfo.h>
#include <helium/core/exception.h>
#include <helium/spooling/jobShouldRun.h>






namespace helium{

  namespace exc{
    class SettingPositionExc:public InvalidOperation{
    public:
      SettingPositionExc():InvalidOperation("To set the position, use a MotorMode"){
    }//ctor
      
    };//class SettingPositionExc
    
  }

  
 

  namespace motProp{
    enum ReadType {IDLE,READ,READTYPES};
    enum ReadFormat {RAWFORMAT,NORMFORMAT,FORMATS};
    
    template<typename MotorSign,ReadFormat F> struct Adapter{
    };




    struct ReadJointPropertyRequest{
      motor::JointPropertyRequest req;
      bool read; //for reading: force reading      

      ReadJointPropertyRequest(){}
      ReadJointPropertyRequest(motor::PropertyID pid,int idx,bool pread):req(pid,idx),read(pread){}
      

      bool operator<(const ReadJointPropertyRequest& b) const{
	if (req<b.req) return true;
	return read&!b.read;
      }
    };

    
    struct ReadPropertyRequest{	
      motor::PropertyRequest req;
      bool read; //for reading: force reading
      //for connection: and high priority 
      
      ReadPropertyRequest(const motor::PropertyRequest& preq,bool pread):req(preq),read(pread)
      {
      }

      ReadPropertyRequest(int j,const ReadJointPropertyRequest& r):req(j,r.req),read(r.read){	
      }

      
      ReadPropertyRequest(){}//for reading
      
      bool operator<(const ReadPropertyRequest& b) const{
	if (req<b.req) return true;
	return read&!b.read;
      }

    };//struct ReadPropertyRequest
    



    /*inline std::ostream& operator<<(std::ostream& o,const ReadPropertyRequest& r){
      return o<<r.req<<" "<<r.read;
    }
    inline std::istream& operator>>(std::istream& i,ReadPropertyRequest& r){
      return i>>r.req>>r.read;
      }*/


    template<typename T>
    struct SetJointPropertyRequest{
      T value;
      motor::JointPropertyRequest req;
    };




    template<typename T>
    struct SetPropertyRequest{
      T value;
      motor::PropertyRequest req;
      SetPropertyRequest(){}
      SetPropertyRequest(int j,const SetJointPropertyRequest<T>& s):value(s.value),req(j,s.req){}
    };


  }


  template<typename MotorSign>
  class MotorPropertyManager{
    //template<typename T> friend struct CellProps;

  public:
    typedef MotorSpooler<MotorSign> Spool;

    typedef typename motor::Raw<MotorSign> Raw;
    typedef typename motor::Interface Norm;
    typedef typename Raw::PropertyNotify PropertyNotify;
    typedef typename Raw::PropertyList PropertyList;
    typedef typename Spool::Motors Motors;

    typedef typename Spool::RawProperty RawProperty;
    typedef typename Spool::NormProperty NormProperty;
    
    typedef motProp::ReadType ReadType;
    typedef motProp::ReadFormat ReadFormat;

  private:
    Spool& ms;
    
    
    class PropertyData: public Callback<PropertyNotify*>{

      friend class MotorPropertyManager;
      RawProperty val;
      NormProperty normVal;
      EhSignal<Cell::CellSignalData> update[motProp::READTYPES][motProp::FORMATS]; 
      MotorPropertyManager *owner;
      

    public:
      
      PropertyData(const PropertyData& p)://we define this because the signal cannot be copied
	val(p.val),normVal(p.normVal),owner(p.owner)
	{
	  
	}
      
      
      PropertyData()://cell(NULL),ncell(NULL),
	val(MotorSign::INVALIDPROP),normVal(MotorSign::INVALIDPROP),owner(NULL)
	{
	  
	}
      void setOwner(MotorPropertyManager &powner){
	owner=&powner;
      }
      
      bool isUseless(){
	for (int i=0;i<motProp::READTYPES;i++){
	  for (int j=0;j<motProp::FORMATS;j++){
	    if (update[i][j].listenerNum()) return false;
	  }
	}
	return true;
      }
      
      bool hasReadType(ReadType type){
	for (int j=0;j<motProp::FORMATS;j++){
	  if (update[type][j].listenerNum()) return true;
	}
	return false;
      }
      
      bool hasFormat(ReadFormat f){
	for (int i=0;i<motProp::READTYPES;i++){
	  if (update[i][f].listenerNum()) return true;
	}
	return false;
      }
      
      
      void updateRaw(){
	for (int i=0;i<motProp::READTYPES;i++){
	  update[i][motProp::RAWFORMAT](&val);
	}
      }
      
      void updateNorm(){
	for (int i=0;i<motProp::READTYPES;i++){
	  update[i][motProp::NORMFORMAT](&normVal);
	}
	}
	
	void operator()(PropertyNotify* p){
	  /*if (p->second->id==motor::POTENTIOPOSITION){
	    std::cout<<"val is"<<val<<"new is "<<p->first<<std::endl;
	    }*/
	  if (val!=p->first){
	    val=p->first;
	    for (int i=0;i<motProp::READTYPES;i++){
	      update[i][motProp::RAWFORMAT](&val);
	    }
	    
	    if (hasFormat(motProp::NORMFORMAT)){	      
	      normVal=p->third->normProp(p->first,*p->second);
	      /*if (p->second->id==motor::POTENTIOPOSITION){
		std::cout<<"normval is"<<normVal<<std::endl;
		}*/

	      for (int i=0;i<motProp::READTYPES;i++){
		update[i][motProp::NORMFORMAT](&normVal);
	      }	   
	    }
	  }//val!=p->first	  
	}


	void connect(Cell::CellConnection& ref,ReadType type,ReadFormat f){	  	 
	  update[type][f].connect(ref);	  

	  //std::cout<<"property data,cell connection,"<<&ref<<" with disc pointer"<<&ref.disc<<" value="<<ref.disc<<std::endl;
	}

	void disconnect(Cell::CellConnection& ref,ReadType type,ReadFormat f){	
	  update[type][f].disconnect(ref);
	}

      
      void handle(exc::Exception& e){
	
	for (int i=0;i<motProp::READTYPES;i++){
	  for (int j=0;j<motProp::FORMATS;j++){
	    update[i][j].handle(e);
	  }
	}
      }

    };//PropertyData
    


    
    
  private:    



    typedef std::map<motor::PropertyRequest,PropertyData> PropertyMap;
    PropertyMap data;

    typedef triple<motor::PropertyRequest,ReadType,ReadFormat> ConnectionInfo;
    friend struct motProp::Adapter<MotorSign,motProp::RAWFORMAT>;
    friend struct motProp::Adapter<MotorSign,motProp::NORMFORMAT>;
    
    
    class PropertyJob:public Job<Motors>{
      
      ReadType myType;
      MotSpoolPriority::Priority myPriority;
      virtual void updateTokens(Motors& m,PropertyList& list)=0;
      PropertyMap& data;       
    public:
      JobShouldRun<Motors> jobState;
      const MotSpoolPriority::Priority& getPriority(){
	return myPriority;
      }
      
      PropertyJob(ReadType pmyType, MotSpoolPriority::Priority pmyPriority,PropertyMap& pdata):
	myType(pmyType),myPriority(pmyPriority),data(pdata){
      };
      

      void update(Motors& m){ //returns if the job should be added
	PropertyList list;
	for (typename PropertyMap::iterator it=data.begin();it!=data.end();++it){
	  if (it->second.hasReadType(myType)){
	    list.push_back(typename Raw::PropertyNotifyRequest(&it->second,&it->first));
	  }
	}
	bool hasWork=list.size()>0;
	jobState.setShouldRun(hasWork);
	if (hasWork){
	  updateTokens(m,list);
	}
      }

      void handle(exc::Exception& e){
	for (typename PropertyMap::iterator it=data.begin();it!=data.end();++it){
	  if (it->second.hasReadType(myType)){
	    it->second.handle(e);
	  }
	}
      }
     
  
    };//PropertyJob

    
    class SingleTokenJob:public PropertyJob{
      typename Raw::Token readToken;
    
      void updateTokens(Motors& m,PropertyList& list){
	m.hw->rawGetGetterToken(readToken,list);	
      }
      
      bool operator()(Motors& m){
	if (this->jobState.mustExit()){
	  return true;
	}
	try{
	  //	  std::cout<<"reading fast"<<std::endl;
	  m.hw->rawGetProperty(readToken);
	}catch(exc::Exception& e){
	  this->handle(e); 
	  throw; //we need to inform the state monitor
	}
	return false;
      }

      bool isToCancel(){
	return this->jobState.mustExit();
      }

    public:
      SingleTokenJob(ReadType r,MotSpoolPriority::Priority p,PropertyMap& pdata):PropertyJob(r,p,pdata){	
      }

    };// SingleToken


    class MultipleTokenJob:public PropertyJob{
      Array<typename Raw::Token> readToken;
      int currToken;

          
      void updateTokens(Motors& m,PropertyList& list){
	m.hw->rawGetOptimizedSplit(readToken,list);	
	/*std::cout<<"got new optimized split";
	for (size_t i=0;i<readToken.size();i++){
	  std::cout<<&readToken[i]<<" ";
	  }*/
	//std::cout<<std::endl;
	
	currToken=(currToken)%(int)readToken.size();	  
	//std::cout<<"Currently reading "<<readToken.size()<<" tokens"<<std::endl;
      }
      
      bool operator()(Motors& m){
	if (this->jobState.mustExit()){
	  return true;
	}
	try{
	  //milliSleep(1000); //DEBUG
	  //std::cout<<"reading slow"<<std::endl;
	  m.hw->rawGetProperty(readToken[currToken]);
	  currToken=(currToken+1)%(int)readToken.size();	  
	}catch(exc::Exception& e){
	  currToken=(currToken+1)%(int)readToken.size();	  
	  this->handle(e);
	  throw; //we need to inform the state monitor
	}
	
	return false;
      }
    public:
      MultipleTokenJob(ReadType r,MotSpoolPriority::Priority p,PropertyMap& pdata):PropertyJob(r,p,pdata),currToken(0){	
      }

      bool isToCancel(){
	return this->jobState.mustExit();
      }

    };// MultipleToken



    PropertyJob* jobs[motProp::READTYPES];


    void do_connect(Motors& m,std::pair<Cell::CellConnection*,ConnectionInfo*>& p){
      Cell::CellConnection& ref=*p.first;
      ConnectionInfo& t=*p.second;


      //std::cout<<"connecting";
      //print(t)<<std::endl;
      data[t.first].connect(ref,t.second,t.third);//this also creates when non existing
      data[t.first].setOwner(*this);

      helium::wrapInfo<ConnectionInfo> (ref,t);	  	  
      for (int i=0;i<motProp::READTYPES;i++){
	jobs[i]->update(m);
	jobs[i]->jobState.startIfNecessary(ms,jobs[i],jobs[i]->getPriority());	       
      }//for i
    }

    void do_disconnect(Motors& m,Cell::CellConnection* ref){
      ConnectionInfo loc;	
      unwrapInfo<ConnectionInfo> (*ref,loc);	  
      typename PropertyMap::iterator it=data.find(loc.first);
      it->second.disconnect(*ref,loc.second,loc.third);
      if (it->second.isUseless()){
	data.erase(it);
      } 
      for (int i=0;i<motProp::READTYPES;i++){
	jobs[i]->update(m);
      }//for i
    }
           

    void getProperty(Motors& m,std::pair<RawProperty *,const motProp::ReadPropertyRequest*>& what){	
      typename PropertyMap::iterator it=data.find(what.second->req);
      if (!what.second->read&&it!=data.end()){
	*what.first=it->second.val;
	return; 
      }
      m.hw->rawGetProperty(*what.first,what.second->req);
      if (it!=data.end()) it->second.val=*what.first;
    }

    void getNormProperty(Motors& m,std::pair<NormProperty* ,const motProp::ReadPropertyRequest*>& what){
      typename PropertyMap::iterator it=data.find(what.second->req);
      static typename Raw::PropNormalizer& normalizer=m.hw->convProp();

      if (!what.second->read&&it!=data.end()){
	*what.first=it->second.normVal;
	return; 
      }
      RawProperty p;	
      m.hw->rawGetProperty(p,what.second->req);
      *what.first=normalizer.normProp(p,what.second->req);
      if (it!=data.end()){
	it->second.val=p;
	it->second.normVal=*what.first;
      }
    }



    void setProperty(Motors& m,const motProp::SetPropertyRequest<RawProperty> *s){      
      typename PropertyMap::iterator it=data.find(s->req);
      static typename Raw::PropNormalizer& normalizer=m.hw->convProp();

      if (it!=data.end()){
	it->second.val=s->value;
	if (it->second.hasFormat(motProp::RAWFORMAT)){
	  it->second.updateRaw();
	}//raw
	if (it->second.hasFormat(motProp::NORMFORMAT)){
	  it->second.normVal=normalizer.normProp(s->value,s->req);
	    it->second.updateNorm();
	}//norm
      }//it!=data.end()
      m.hw->rawSetProperty(s->value,s->req);
    }
    
    void setNormProperty(Motors& m,const motProp::SetPropertyRequest<NormProperty> *s){     
      typename PropertyMap::iterator it=data.find(s->req);
      static typename Raw::PropNormalizer& normalizer=m.hw->convProp();
      RawProperty p=normalizer.unnormProp(s->value,s->req);
      m.hw->rawSetProperty(p,s->req);     
      if (it!=data.end()){
	it->second.normVal=s->value;
	it->second.val=p;
	it->second.updateRaw();
	if (it->second.hasFormat(motProp::NORMFORMAT)){
	  it->second.updateNorm();
	}//norm
      }//it!=data.end

    }


  public:
    template<ReadFormat F> class PropertyCellSignature:public CellSignature{
    public:
      typedef typename  motProp::Adapter<MotorSign,F>::Property GT;
      typedef motProp::ReadPropertyRequest GP;
      typedef motProp::SetPropertyRequest<typename motProp::Adapter<MotorSign,F>::Property> ST;
      typedef StdReply SP;
      typedef typename motProp::Adapter<MotorSign,F>::Property UT;
      typedef motProp::ReadPropertyRequest UP;
      static const bool getRequiresParam=true;
      static const bool setRequiresParam=false;
      static const bool connectRequiresParam=true;
    };

    template<ReadFormat F> class JointPropertyCellSignature:public CellSignature{
    public:
      typedef typename motProp::Adapter<MotorSign,F>::Property GT;
      typedef motProp::ReadJointPropertyRequest GP;
      typedef motProp::SetJointPropertyRequest<typename motProp::Adapter<MotorSign,F>::Property> ST;
      typedef StdReply SP;
      typedef typename motProp::Adapter<MotorSign,F>::Property UT;
      typedef motProp::ReadJointPropertyRequest UP;
      static const bool getRequiresParam=true;
      static const bool setRequiresParam=false;
      static const bool connectRequiresParam=true;

    };


  


      private:
    template<ReadFormat F>
    class PropertyCell:public TypedCell<PropertyCellSignature<F> >{
      typedef motProp::Adapter<MotorSign,F> Adpt;
      
      MotorPropertyManager& owner;
      typedef typename motProp::Adapter<MotorSign,F>::Property T;
    public:
      
      std::string getName() const{
	return std::string(F==motProp::RAWFORMAT?"Raw":"Normalized")+" property cell";

      }
      void getData(T &p ,const motProp::ReadPropertyRequest& what){
	typename Adpt::GetterJob job(&owner,std::pair<T *,const motProp::ReadPropertyRequest*>(&p,&what));
	owner.ms.waitJob(&job,MotSpoolPriority::HIGH);
      }
      
      void setData(const motProp::SetPropertyRequest<T> &s){		
	Job<Motors> *job=new typename Adpt::FastSetterJob
	  (&owner,&s);
	owner.ms.addJob(job,MotSpoolPriority::HIGH);
      }
      void setData(StdReply& r,const motProp::SetPropertyRequest<T> &s){

	typename Adpt::SyncSetterJob job(&owner,&s);
	owner.ms.waitJob(&job,MotSpoolPriority::HIGH);
      }

      void connect(Cell::CellConnection& ref,const motProp::ReadPropertyRequest& what){
	ConnectionInfo ci(what.req,what.read?motProp::READ:motProp::IDLE,F);
			  
	//std::cout<<"cell connection,"<<&ref<<" with disc "<<ref.disc<<std::endl;
	StackJob<Motors,
	  MotorPropertyManager,
	  std::pair<Cell::CellConnection*,ConnectionInfo*>,
	  std::pair<Cell::CellConnection*,ConnectionInfo*>&,	  
		 &MotorPropertyManager::do_connect,true>
	job(&owner, std::pair<Cell::CellConnection*,ConnectionInfo*>(&ref,&ci));
	owner.ms.waitJob(&job,MotSpoolPriority::HIGH);
	  
      }
      void disconnect(Cell::CellConnection& ref){
	StackJob<Motors,
	  MotorPropertyManager,
	  Cell::CellConnection*,
	  Cell::CellConnection*,
		 &MotorPropertyManager::do_disconnect,true>
	  job(&owner,&ref);
	  owner.ms.waitJob(&job,MotSpoolPriority::HIGH);
      }

      PropertyCell(MotorPropertyManager& powner):owner(powner){
      }
      
    };


    template<ReadFormat F>
    class JointPropertyCell:public TypedCell<JointPropertyCellSignature<F> >{
      typedef motProp::Adapter<MotorSign,F> Adpt;
      
      PropertyCell<F>& owner;
      int j;
      typedef typename motProp::Adapter<MotorSign,F>::Property T;

    public:
      
      std::string getName() const{
	return "Joint "+toString(j)+" "+(F==motProp::RAWFORMAT?"Raw":"Normalized")+" property cell";

      }
      void getData(T &p ,const motProp::ReadJointPropertyRequest& what){
	owner.getData(p,motProp::ReadPropertyRequest(j,what));
      }
      
      void setData(const motProp::SetJointPropertyRequest<T> &s){		
	owner.setData(motProp::SetPropertyRequest<T>(j,s));
      }
      void setData(StdReply& r,const motProp::SetJointPropertyRequest<T> &s){
	owner.setData(r,motProp::SetPropertyRequest<T>(j,s));

      }

      void connect(Cell::CellConnection& ref,const motProp::ReadJointPropertyRequest& what){	  
	owner.connect(ref,motProp::ReadPropertyRequest(j,what));
      }

      void disconnect(Cell::CellConnection& ref){
	owner.disconnect(ref);
      }

      JointPropertyCell(PropertyCell<F>& powner,int pj):owner(powner),j(pj){
      }
      
    };



    PropertyCell<motProp::RAWFORMAT> rawPropCell;
    PropertyCell<motProp::NORMFORMAT> normPropCell;

    Array<JointPropertyCell<motProp::RAWFORMAT>,
	  array::RESIZABLE,
	  array::InternalAllocator<ParamIdxAllocator<JointPropertyCell<motProp::RAWFORMAT>,
						     PropertyCell<motProp::RAWFORMAT>&,
						     PropertyCell<motProp::RAWFORMAT>&> >
	  > jointRawPropCell;
  
  Array<JointPropertyCell<motProp::NORMFORMAT>,
	array::RESIZABLE,
	array::InternalAllocator<ParamIdxAllocator<JointPropertyCell<motProp::NORMFORMAT>,
						   PropertyCell<motProp::NORMFORMAT>&,
						   PropertyCell<motProp::NORMFORMAT>&> >
	> jointNormPropCell;
  
    
    
    /*inline void addJob(Job<Motors>* j,MotSpoolPriority::Priority p){
      ms.addJob(j,p);
      }*/

    friend class DefDescr<MotorPropertyManager<MotorSign> >;

    friend class StackJob<Motors,
			  MotorPropertyManager,
			  std::pair<RawProperty *,const typename motProp::ReadPropertyRequest*>,
			  std::pair<RawProperty *,const typename motProp::ReadPropertyRequest*>&,	  		       &
			  MotorPropertyManager::getProperty,true>;

    friend class AutoDeleteJob<Motors,
			     MotorPropertyManager,			  
			    const typename motProp::SetPropertyRequest<RawProperty> *,
			    const typename motProp::SetPropertyRequest<RawProperty> *,
			       &MotorPropertyManager::setProperty,true>;
    
    
    friend class StackJob<Motors,
		     MotorPropertyManager,
		     const typename motProp::SetPropertyRequest<RawProperty> *,
		     const typename motProp::SetPropertyRequest<RawProperty> *,
			  &MotorPropertyManager::setProperty,true>; 
    

    friend class StackJob<Motors,
			  MotorPropertyManager,
			  std::pair<NormProperty *,const typename motProp::ReadPropertyRequest*>,
			  std::pair<NormProperty *,const typename motProp::ReadPropertyRequest*>&,	  		       
			  &MotorPropertyManager::getNormProperty,true>;
  
    friend class AutoDeleteJob<Motors,
			     MotorPropertyManager,			  
			    const typename motProp::SetPropertyRequest<NormProperty> *,
			    const typename motProp::SetPropertyRequest<NormProperty> *,
			       &MotorPropertyManager::setNormProperty,true>;
    
    
    friend class StackJob<Motors,
		      MotorPropertyManager,
		      const typename motProp::SetPropertyRequest<NormProperty> *,
		       const typename motProp::SetPropertyRequest<NormProperty> *,
			  &MotorPropertyManager::setNormProperty,true>; 

    
    
    friend class StackJob<Motors,
			  MotorPropertyManager,
			  std::pair<Cell::CellConnection*,ConnectionInfo*>,
			  std::pair<Cell::CellConnection*,ConnectionInfo*>&,	  
			  &MotorPropertyManager::do_connect,true>;
 
    
    friend class StackJob<Motors,
	  MotorPropertyManager,
	  Cell::CellConnection*,
	  Cell::CellConnection*,
			  &MotorPropertyManager::do_disconnect,true>;
    
  public:
    MotorPropertyManager(Spool& pms):ms(pms),rawPropCell(*this),normPropCell(*this),
				     jointRawPropCell(rawPropCell,pms.getMotorNum()),
				     jointNormPropCell(normPropCell,pms.getMotorNum())
					   
    {
      jobs[motProp::IDLE]=new MultipleTokenJob(motProp::IDLE,MotSpoolPriority::IDLE,data);
      jobs[motProp::READ]=new SingleTokenJob(motProp::READ,MotSpoolPriority::NORMAL,data);            
    }
    ~MotorPropertyManager(){
      delete jobs[motProp::IDLE];
      delete jobs[motProp::READ];     
    }


  };//MotorPropertyManager


  
  template<typename MotorSign>
  class DefDescr<MotorPropertyManager<MotorSign> >:public Description<MotorPropertyManager<MotorSign> >{
  public:
    DefDescr(MotorPropertyManager<MotorSign>& ms):Description<MotorPropertyManager<MotorSign> >(ms){
    }

    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(scalarMapping(this->obj.rawPropCell,IDPath("motors.properties.raw",".")));
      members.push_back(scalarMapping(this->obj.normPropCell,IDPath("motors.properties.norm",".")));
      members.push_back(vectMapping(this->obj.jointRawPropCell,IDPath("motors.motor","."),IDPath("properties.raw","."),IDPath("motors.num",".")));
      members.push_back(vectMapping(this->obj.jointNormPropCell,IDPath("motors.motor","."),IDPath("properties.norm","."),IDPath("motors.num",".")));
    }

    
  };


  


  namespace motProp{


    template<typename MotorSign> struct Adapter<MotorSign,RAWFORMAT>{    
      typedef MotorPropertyManager<MotorSign> PropertyManager;
      typedef typename PropertyManager::RawProperty Property;
      typedef typename PropertyManager::Motors Motors;
      
      typedef StackJob<Motors,
		       PropertyManager,
		       std::pair<Property *,const typename motProp::ReadPropertyRequest*>,
		       std::pair<Property *,const typename motProp::ReadPropertyRequest*>&,	  
		       &PropertyManager::getProperty,true> GetterJob;
    
      typedef AutoDeleteJob<Motors,
			    PropertyManager,			  
			    const typename motProp::SetPropertyRequest<Property> *,
			    const typename motProp::SetPropertyRequest<Property> *,
			    &PropertyManager::setProperty,true> FastSetterJob; 
    
      typedef StackJob<Motors,
		       PropertyManager,
		       const typename motProp::SetPropertyRequest<Property> *,
		       const typename motProp::SetPropertyRequest<Property> *,
		       &PropertyManager::setProperty,true> SyncSetterJob; 
    
      
    };

    
    template<typename MotorSign> struct Adapter<MotorSign,NORMFORMAT>{  
      typedef MotorPropertyManager<MotorSign> PropertyManager;
      typedef typename PropertyManager::NormProperty Property;
      typedef typename PropertyManager::Motors Motors;
      
      typedef StackJob<Motors,
		       PropertyManager,
		       std::pair<Property *,const typename motProp::ReadPropertyRequest*>,
		       std::pair<Property *,const typename motProp::ReadPropertyRequest*>&,	  
		       &PropertyManager::getNormProperty,true> GetterJob;
    
      typedef AutoDeleteJob<Motors,
			    PropertyManager,			  
			    const typename motProp::SetPropertyRequest<Property> *,
			    const typename motProp::SetPropertyRequest<Property> *,
			    &PropertyManager::setNormProperty,true> FastSetterJob; 
    
      typedef StackJob<Motors,
		       PropertyManager,
		       const typename motProp::SetPropertyRequest<Property> *,
		       const typename motProp::SetPropertyRequest<Property> *,
		       &PropertyManager::setNormProperty,true> SyncSetterJob; 


    };


  }
  
  namespace motProp{

    inline std::ostream& operator<<(std::ostream& o,const ReadPropertyRequest &p){
      return o<<p.req<<" "<<(p.read?"H":"L");
    }
    
    inline  std::istream& operator>>(std::istream& i,ReadPropertyRequest &p){
      i>>p.req;
      char r;
      i>>r;
      p.read=r!='L';
      return i;
    }

    inline std::ostream& operator<<(std::ostream& o,const ReadJointPropertyRequest &p){
      return o<<p.req<<" "<<(p.read?"H":"L");
    }
    
    inline  std::istream& operator>>(std::istream& i,ReadJointPropertyRequest &p){
      i>>p.req;
      char r;
      i>>r;
      p.read=r!='L';
      return i;
    }

    


    
    template<typename T> std::ostream& operator<<(std::ostream& o,const SetPropertyRequest<T> &p){
      return o<<p.req<<" "<<p.value;
    }
    
    template<typename T> std::istream& operator>>(std::istream& i,SetPropertyRequest<T> &p){
      return i>>p.req>>p.value;
    }
    
    template<typename T> std::ostream& operator<<(std::ostream& o,const SetJointPropertyRequest<T> &p){
      return o<<p.req<<" "<<p.value;
    }
    
    template<typename T> std::istream& operator>>(std::istream& i,SetJointPropertyRequest<T> &p){
      return i>>p.req>>p.value;
    }


  }



}

#endif
  /*template<> struct CellProps<MotorPropertyManager::RawProperty>{
    typedef StackJob<Motors,
		     MotorPropertyManager,
		     std::pair<Motors::Property *,const MotorPropertyManager::ReadPropertyRequest*>,
		     std::pair<Motors::Property *,const MotorPropertyManager::ReadPropertyRequest*>&,	  
		     &MotorPropertyManager::getProperty> GetterJob;
  
   typedef AutoDeleteJob<Motors,
			  MotorPropertyManager,			  
			 const MotorPropertyManager::SetPropertyRequest<Motors::Property> *,
			  const MotorPropertyManager::SetPropertyRequest<Motors::Property> *,
			  &MotorPropertyManager::setProperty> FastSetterJob; 

    typedef StackJob<Motors,
		     MotorPropertyManager,
		     const MotorPropertyManager::SetPropertyRequest<Motors::Property> *,
		     const MotorPropertyManager::SetPropertyRequest<Motors::Property> *,
		     &MotorPropertyManager::setProperty> SyncSetterJob; 
    
    
  };
    
    
    
  template<> struct CellProps<Motors::NormProperty>{
    typedef StackJob<Motors,
		     MotorPropertyManager,
		     std::pair<Motors::NormProperty *,const MotorPropertyManager::ReadPropertyRequest*>,
		     std::pair<Motors::NormProperty *,const MotorPropertyManager::ReadPropertyRequest*>&,	  
    &MotorPropertyManager::getNormProperty> GetterJob;

    typedef AutoDeleteJob<Motors,
			  MotorPropertyManager,
			  const MotorPropertyManager::SetPropertyRequest<Motors::NormProperty> *,
			  const MotorPropertyManager::SetPropertyRequest<Motors::NormProperty> *,
    &MotorPropertyManager::setNormProperty> FastSetterJob; 

    typedef StackJob<Motors,
    MotorPropertyManager,
		     const MotorPropertyManager::SetPropertyRequest<Motors::NormProperty> *,
		     const MotorPropertyManager::SetPropertyRequest<Motors::NormProperty> *,		     
		     &MotorPropertyManager::setNormProperty> SyncSetterJob;
    
  };
  */



  /*template <typename T> void MotorPropertyManager::PropertyCell<T>::getData(T &p ,const MotorPropertyManager::ReadPropertyRequest& what){
    typename CellProps<T>::GetterJob job(&owner,std::pair<T *,const MotorPropertyManager::ReadPropertyRequest*>(&p,&what));
	owner.ms.waitJob(&job,MotorSpooler::HIGH);
  }

  template <typename T> void MotorPropertyManager::PropertyCell<T>::setData(const MotorPropertyManager::SetPropertyRequest<T> &s){
    if (s.req.id==Motors::POSITION){
      throw exc::SettingPositionExc();
    }	
    Job<Motors> *job=new typename CellProps<T>::FastSetterJob
      (&owner,&s);
    owner.ms.addJob(job,MotorSpooler::HIGH);
  }


   template <typename T> void MotorPropertyManager::PropertyCell<T>::setData(StdReply& r,const MotorPropertyManager::SetPropertyRequest<T> &s){
    if (s.req.id==Motors::POSITION){
      throw exc::SettingPositionExc();
    }	
    typename CellProps<T>::SyncSetterJob job(&owner,&s);
      owner.ms.waitJob(&job,MotorSpooler::HIGH);
  }
  */

