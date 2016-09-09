#ifndef HE_VSTOUCHSENSOR
#define HE_VSTOUCHSENSOR

#include <helium/robot/sensorInterface.h>
#include <helium/vstone/vstoneSerial.h>
#include <helium/system/cellDescription.h>

namespace helium{


  class VStouchSensor:public sensor::Set<int>{
  public:
    static const int M_VIN0 =0x20; // 2 R		measured vin0
    static const int M_VIN1 =0x22; // 2 R		measured vin1
    static const int M_VIN2 =0x24; // 2 R		measured vin2
    static const int M_VIN3 =0x26; // 2 R		measured vin3
    static const int M_VIN4 =0x28; // 2 R		measured vin4
    static const int M_VIN5 =0x2a; // 2 R		measured vin5
    static const int M_VIN6 =0x2c; // 2 R		measured vin6
    static const int M_VIN7 =0x2e; // 2 R		measured vin7

    /*fabio extensions*/

    static const int LED_MODE0 =0x30;
    static const int LED_MODE1 =0x31;
    static const int LED_MODE2 =0x32;
    static const int LED_MODE3 =0x33;
    static const int LED_MODE4 =0x34;
    static const int LED_MODE5 =0x35;
    static const int LED_MODE6 =0x36;
    static const int LED_MODE7 =0x37;
    
    static const int LED_DUTY0 =0x38;
    static const int LED_DUTY1 =0x39;
    static const int LED_DUTY2 =0x3a;
    static const int LED_DUTY3 =0x3b;
    static const int LED_DUTY4 =0x3c;
    static const int LED_DUTY5 =0x3d;
    static const int LED_DUTY6 =0x3e;
    static const int LED_DUTY7 =0x3f;
   

    static const int BOARDSENSORS=8;

   
    
    struct RawId{
      int board;
      int id;
    };
    typedef Array<RawId> RawIdMap;
    
  private:
    unsigned char buffer[256];
    
    VstoneSerial& s;

    const RawIdMap& idMap;
    std::map<int,HwIdList > boardToSensors; //used for the exceptions

    class SensorToken:public Deletable{
    public:
      struct ReadData:public Deletable{
	int bid,//board id
	  minId,maxId; //min/max sensor id (0..7)
	Array<int,BOARDSENSORS> gid;	//global id
	void mark(int sid,int gid);
	
	ReadData();
      };
      Array<ReadData> toRead;
      void set(const std::map<int,SensorToken::ReadData>& m);
      SensorToken();
      SensorToken(const std::map<int,SensorToken::ReadData>& m);
    };//SensorToken
    
    
    SensorToken wholeToken;
    
    

    void rawRead(int*,const SensorToken& t); //SensorToken, not Token
    void rawRead(SensorValList<int>& , const SensorToken& t); 


    void getReadDataCompleteMap(std::map<int,SensorToken::ReadData>& m,const HwIdList& what) const;
    
    void getReadDataSubMap(std::map<int,SensorToken::ReadData>& m,const HwIdList& what) const;

    

    
    template<typename Param, void (*updaterf)(Param,int,HwState)>
    void isWorking(Param target,const SensorToken::ReadData &r);
  
    int validMin;
  public:    
    VStouchSensor(VstoneSerial& ps,const RawIdMap& pidMap);   

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

    void setMode(int idx,int val);
    void rawSetDuty(int idx,int val);
    void setDuty(int idx,double d);
  };//VStouchSensor
  
  inline std::ostream& operator<<(std::ostream& o,const VStouchSensor:: RawId& rid){
    return o<<rid.board<<" "<<rid.id;
  }

  inline std::istream& operator>>(std::istream& i,VStouchSensor:: RawId& rid){
    return i>>rid.board>>rid.id;
  }


  template<> 
  class DefDescr<VStouchSensor::RawId>:public Description<VStouchSensor::RawId>{
  public:
    DefDescr(VStouchSensor::RawId& s):Description<VStouchSensor::RawId>(s){
    }

    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){      
      members.push_back(scalarMapping(obj.board,"board"));
      members.push_back(scalarMapping(obj.id,"channel"));
    }
    
  };

  template<> 
  class DefDescr<VStouchSensor::RawIdMap>:public Description<VStouchSensor::RawIdMap>{
  public:    
    DefDescr(VStouchSensor::RawIdMap& s):Description<VStouchSensor::RawIdMap>(s){            
    }
    
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){      
      members.push_back(vectMapping(vd(obj),IDPath("sensors.sensor","."),"",IDPath("sensors.num",".")));
    }    
  };


}//helium


#include <helium/vstone/vsTouchSensor.hpp>
#endif
