/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_VSCRCTOUCHSENSOR
#define HE_VSCRCTOUCHSENSOR

#include <helium/robot/sensorInterface.h>
#include <helium/robot/sensorSpooler.h>
#include <helium/system/cellDescription.h>
#include <helium/vector/operations.h>
#include <helium/robot/touchSensorException.h>
#include <helium/math/bit.h>
#include <helium/vstone/vsByte.h>
#include <helium/vstone/vsTouchSensorTable.h>
namespace helium{


 
  namespace vsTs{


    struct RawId{
    int board;
    int chan;
  };
    typedef Array<vsTs::RawId> RawIdMap; //global sensor id to board and channel
    


    struct ReadData{
      int minId,maxId; //min/max sensor id (0..7)
      Array<int,vsTs::CHANNELS> gid;	//global id
      void markRead(int sid,int gid);      
      ReadData();
    };


    typedef std::map<int,ReadData> ReadSet;

    struct WorkToken:public Deletable,public ReadData{
      int bid;
      class Ref{
	WorkToken* w;
      public:	
	Ref():w(new WorkToken()){}
	WorkToken* get(){return w;}
	void markRead(int sid,int gid){w->markRead(sid,gid);}
	WorkToken* setBid(int b){
	  w->bid=b;
	  return w;
	}
      };
    };

    struct BoardFlags{
      static const unsigned char INVALIDFLAG;
      unsigned char data[vsTs::CHANNELS];
      bool operator!=(const BoardFlags& b);
      void reset();
      void invalidate();
    };
   
    typedef  Callback<unsigned char*> * WriterF;
    
    class WriteTokenMaker{
    public:
      virtual WriterF getWriterF() =0;
      virtual const HwIdList& getBurstWriteIds() =0;
    };
    

    
    class BurstToken:public Deletable{
    public:
      struct BoardData:public Deletable{
	int bid;//board id
	Array<int,vsTs::CHANNELS> rgid;	//global id
	Array<int,vsTs::CHANNELS> wgid;
	int rSize,wSize;
	unsigned char rCrc,wCrc;

	enum Type{BURST_READ,BURST_WRITE,BURST_READWRITE} type;
		
	void markRead(int sid,int gid); //chan id,desired "global" id 	
	void markWrite(int sid,int gid); //chan id,desired "global" id 	

	void finalize();
	
	void getFlags(BoardFlags& flags);

	BoardData();
      };
      
      Array<BoardData> bdata;
      mutable bool wasInit;
      WriterF writerf;
      
      void set(const std::map<int,BoardData>& m);//sets the array 
     
      //from the map, filling in bid
      BurstToken();
      BurstToken(const std::map<int,BoardData>& m);
      BurstToken(const std::map<int,BoardData>& m, WriterF writef);
    };//BurstToken 
      

    struct BurstData{      
      Array<BoardFlags> curFlags;                 
      const BurstToken* activeToken;      
      BurstData(int boardn);
      void invalidate();
    };
    
  
    class LocklessLedBurstSetter:
      public Callback<unsigned char*>,
      public WriteTokenMaker{
      HwIdList onBurst;
      SensorSpooler<int>& spool;
      Array<double> vals;
      void operator()(unsigned char* v);
      void do_setOnBurst(const HwIdList& h);
      void do_setBurstDuty(const double * d);    
    public:    

      WriterF getWriterF() ;
      const HwIdList& getBurstWriteIds() ;
      LocklessLedBurstSetter(SensorSpooler<int>& spool,int n);
      void setOnBurst(const HwIdList& h);
      void setBurstDuty(const double * d);    
    };




  }//ns vsTs




  template<typename VsSrl>
  class VsCrcTouchSensor:public sensor::Set<int>{
              
  private:

    static const int READRETRIES=2; 
    typedef std::map<int,HwIdList > BoardToSensor;  


     
    

    
    VsSrl& vs;
    const vsTs::RawIdMap& sensMap ;
    const vsTs::RawIdMap* ledMap ;
    vsTs::WriteTokenMaker* wtm;

    BoardToSensor boardToSensors; //used for the exceptions
    vsTs::BurstData burstData;
    int validMin;
    
    vsTs::ReadSet wholeToken;
    unsigned char buffer[256];
    Array<unsigned char> rawDuty;

    exc::MultipleExceptionHolder meh;

    

    void assureBurstToken(const vsTs::BurstToken* t); //exceptions stored in meh
    void burstBoard(const vsTs::BurstToken::BoardData& d);

    BoardToSensor& initBoardToSensor();

   
    template<typename Data>
    void getReadDataSparseMap(std::map<int,Data>& m,const HwIdList& what) const; //the ids are what.ids[i]
    
    template<typename Data>
    void getReadDataPackedMap(std::map<int,Data>& m,const HwIdList& what) const; //the ids are i (the position in what)



    template<typename Data>
    void getWriteDataSparseMap(std::map<int,Data>& m,const HwIdList& what) const; //the ids are what.ids[i]
    
    template<typename Data>
    void getWriteDataPackedMap(std::map<int,Data>& m,const HwIdList& what) const; //the ids are i (the position in what)

 
    void readset(int*,const vsTs::ReadSet& s);
    
   
    
    template<typename Param,void assigner(Param,int,HwState)>
    void isWorking(Param p,int bid,const vsTs::ReadData &r);
  
    
  public:    
    VsCrcTouchSensor(VsSrl& ps,const vsTs::RawIdMap& sensMap,
		     const vsTs::RawIdMap* pledMap=NULL,
		     vsTs::WriteTokenMaker* pwtm=NULL
		     );   

    void setValidMin(int m);

    int rawRead(int idx);    
    void rawRead(int*);  
    
    void rawRead(int*, const HwIdList&); 
    void rawRead(SensorValList<int>& , const HwIdList&); 
    
    void getToken(Token& t, const HwIdList& toRead);


    void rawRead(int*,const Token& t);//T* is for ALL sensors, not just the one in the token

    size_t size() const;

    HwState isWorking(int id);
    void isWorking(Array<HwState>& ,const HwIdList&);
    
    void getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList &) const;        
    void isWorking(Callback<const HwStateNotification&>*,const HwInterface::WorkingToken&);

    //fabio extensions
   
    void setMode(int idx,vsTs::Mode mode);
    
    void rawSetDuty(int idx,int val);
    void setDuty(int idx,double d);
  };//VStouchSensor


  template<typename VsSrl>
  inline std::ostream& operator<<(std::ostream& o,const typename VsCrcTouchSensor<VsSrl>:: RawId& rid){
    return o<<rid.board<<" "<<rid.id;
  }

   template<typename VsSrl>
  inline std::istream& operator>>(std::istream& i,typename VsCrcTouchSensor<VsSrl>:: RawId& rid){
    return i>>rid.board>>rid.id;
  }


  template<>
  class DefDescr<vsTs::RawId>:
    public Description<vsTs::RawId>{
  public:
    DefDescr(vsTs::RawId& s):Description<vsTs::RawId>(s){
    }

    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){      
      members.push_back(scalarMapping(obj.board,"board"));
      members.push_back(scalarMapping(obj.chan,"channel"));
    }
    
  };

  template<> 
    class DefDescr<vsTs::RawIdMap>:
    public Description<vsTs::RawIdMap>{
  public:    
      DefDescr(vsTs::RawIdMap& s):Description<vsTs::RawIdMap>(s){            
    }
    
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){      
      members.push_back(vectMapping(vd(obj),IDPath("sensor","."),"",IDPath("num",".")));
    }    
  };

  
  class LedDescr:
    public Description<vsTs::RawIdMap>{
  public:    
      LedDescr(vsTs::RawIdMap& s):Description<vsTs::RawIdMap>(s){            
    }
    
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){      

      members.push_back(fvectMapping<SILENTFAIL>(vd(obj),IDPath("led","."),"",IDPath("num",".")));
    }    
  };


}//helium


#include <helium/vstone/vsCrcTouchSensor.hpp>
#endif
