#include <helium/vstone/vsTouchSensor.h>
#include <helium/vector/operations.h>
#include <helium/robot/touchSensorException.h>
#include <helium/smartpointer/heapAutoCtor.h>
namespace helium{


  void  VStouchSensor::getReadDataCompleteMap(std::map<int,SensorToken::ReadData>& m,const HwIdList& what) const{
      for (size_t i=0;i<what.ids.size();i++){
	int idx=what.ids[i];
	m[idMap[idx].board].mark(idMap[idx].id,idx);      
	//std::cout<<"adding "<<idx<<"now "<<m[idMap[idx].board].minId<<"~"<<m[idMap[idx].board].minId<<std::endl;
      }
      for (std::map<int,SensorToken::ReadData>::iterator it=m.begin();it!=m.end();++it){
	it->second.bid=it->first;
      }
  }


    void  VStouchSensor::getReadDataSubMap(std::map<int,SensorToken::ReadData>& m,const HwIdList& what) const{
      for (size_t i=0;i<what.ids.size();i++){
	int idx=what.ids[i];
	m[idMap[idx].board].mark(idMap[idx].id,i);      
      }
      for (std::map<int,SensorToken::ReadData>::iterator it=m.begin();it!=m.end();++it){
	it->second.bid=it->first;
      }
  }
  


  VStouchSensor::SensorToken::ReadData::ReadData():minId(BOARDSENSORS),maxId(-1){
    setAll(gid,-1);
  }

  void VStouchSensor::SensorToken::ReadData::mark(int sid,int pgid){
      minId=(std::min)(sid,minId);
      maxId=(std::max)(sid,maxId);
      gid[sid]=pgid;
  }

  VStouchSensor::SensorToken::SensorToken(){    
  }

  VStouchSensor::SensorToken::SensorToken(const std::map<int,SensorToken::ReadData>& m){
    set(m);
  }

  void VStouchSensor::SensorToken::set(const std::map<int,SensorToken::ReadData>& m){
    toRead.resize(m.size());
    int c=0;
    for (std::map<int,ReadData>::const_iterator it=m.begin();it!=m.end();++it){
      toRead[c]=it->second;
      toRead[c].bid=it->first;
      ++c;
    }
  }

  
  VStouchSensor::VStouchSensor(VstoneSerial& ps,const RawIdMap& pidMap):s(ps),idMap(pidMap),validMin(1){
    HwIdList l(idMap.size());
    l.fillConsecutive();
    std::map<int,SensorToken::ReadData> m;
    getReadDataCompleteMap(m,l);
    wholeToken.set(m); 
    for (size_t i=0;i<idMap.size();i++){
      boardToSensors[idMap[i].board].ids.push_back(i);
    }
  }

  void VStouchSensor::setValidMin(int m){
    validMin=m;
  }
  

  void VStouchSensor::rawRead(int* t,const SensorToken& tk){    
    exc::MultipleExceptionHolder eh;
    for (size_t i=0;i<tk.toRead.size();i++){
      SensorToken::ReadData &r=tk.toRead[i];     
      try{
	//std::cout<<"reading board"<<r.bid<<"from id "<<r.minId<<"size "<<(r.maxId-r.minId+1)<<std::endl;
	s.recv(r.bid,M_VIN0+2*r.minId,buffer,2*(r.maxId-r.minId+1));
      }catch(exc::Exception& e){	
	eh.handle(exc::TouchSensorBoardException<UNREACHABLEHWSTATE>
		  (r.bid,boardToSensors[r.bid],toString(e))
		  );
      }
      for (int si=r.minId;si<=r.maxId;si++){
	int g=r.gid[si];
	//std::cout<<"minid"<<si<<" global id"<<g<<" "<<std::endl;
	if (g>=0){
	  from14(t[g],&buffer[(si-r.minId)*2]);
	  //std::cout<<"value "<<t[g]<<std::endl;
	  if (t[g]<validMin){
	    
	    eh.handle(exc::SingleTouchSensorException<FAULTYVALUEHWSTATE>(g));
	  }
	}//gid>=0
      }//for             
    }//for i
  }//rawRead






  void VStouchSensor::rawRead(SensorValList<int>& v,const SensorToken& tk){    
    exc::MultipleExceptionHolder eh;
    int c=0;
    for (size_t i=0;i<tk.toRead.size();i++){
      SensorToken::ReadData &r=tk.toRead[i];     
      try{
	s.recv(r.bid,M_VIN0+2*r.minId,buffer,2*(r.maxId-r.minId+1));
      }catch(exc::Exception& e){
	eh.handle(exc::TouchSensorBoardException<UNREACHABLEHWSTATE>(r.bid,boardToSensors[r.bid],toString(e)));
      }
      for (int si=r.minId;si<=r.maxId;si++){
	int g=r.gid[si];
	if (g>=0){
	  from14(v.vals[c],&buffer[(si-r.minId)*2]);
	  if (v.vals[c]<validMin){
	    eh.handle(exc::SingleTouchSensorException<FAULTYVALUEHWSTATE>(g));
	  }
	  c++;
	}//gid>=0
      }//for             
    }//for i
  }//rawRead


  int VStouchSensor::rawRead(int idx){
    int v;
    try{
      v=s.read2(idMap[idx].board,M_VIN0+2*idMap[idx].id);
      //std::cout<<"read "<<idx<<" with bid"<<idMap[idx].board<<" and id"<<idMap[idx].id<<std::endl;
    }catch(exc::Exception& e){
      int b=idMap[idx].board;
      throw exc::TouchSensorBoardException<UNREACHABLEHWSTATE>(b,boardToSensors[b],toString(e));
    }
    if (v<validMin){
      throw exc::SingleTouchSensorException<FAULTYVALUEHWSTATE>(idx);
    }
    return v;



}
  

  void VStouchSensor::rawRead(int* t){
    rawRead(t,wholeToken);
  }



  
  void VStouchSensor::rawRead(int* t, const HwIdList& toRead){
    std::map<int,SensorToken::ReadData> m;
    getReadDataCompleteMap(m,toRead);
    SensorToken tk(m);
    rawRead(t,tk);
  }
  
  void VStouchSensor::rawRead(SensorValList<int>& v, const HwIdList& toRead){
    std::map<int,SensorToken::ReadData> m;
    getReadDataCompleteMap(m,toRead);
    SensorToken tk(m);
    rawRead(v,tk);
  }
  
  void VStouchSensor::getToken(Token& t, const HwIdList& toRead){
    std::map<int,SensorToken::ReadData> m;
    getReadDataCompleteMap(m,toRead);
    t=new SensorToken(m);
  }

  void VStouchSensor::rawRead(int* t,const Token& tk){
    rawRead(t,*trust_cast<const SensorToken*>(&tk));
  }

  size_t VStouchSensor::size() const{
    return idMap.size();
  }


 

  
  HwState  VStouchSensor::isWorking(int idx){   
    try{
      return s.read2(idMap[idx].board,M_VIN0+2*idMap[idx].id)<validMin?
	FAULTYVALUEHWSTATE:UNREACHABLEHWSTATE;    
    }catch(exc::Exception& ){
      return UNREACHABLEHWSTATE;
    }
  }

  
  inline void assigner(HwState* state,int idx,HwState value){
    state[idx]=value;
  }
  
  inline void callbackCaller(Callback<const HwStateNotification&>*cb,int idx,HwState value){
    (*cb)(std::make_pair(idx,value));
  }
  
  
  
  void VStouchSensor::isWorking(Array<HwState>& state,const HwIdList& toTest){
    std::map<int,SensorToken::ReadData> toRead; 
    getReadDataSubMap(toRead,toTest);
    for (std::map<int,SensorToken::ReadData>::iterator it=toRead.begin();it!=toRead.end(
);++it){
      isWorking<HwState*,assigner> (state.data,it->second);     
    }//for it
  }//isWorking


  void VStouchSensor::getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList & toTest) const{
    std::map<int,SensorToken::ReadData> toRead; 
    getReadDataCompleteMap(toRead,toTest);
    //std::cout<<"size is "<<toRead.size()<<std::endl;
    tk.reset(toRead.size());

    int c=0;
    for (std::map<int,SensorToken::ReadData>::iterator it=toRead.begin();it!=toRead.end(
);++it){
      tk[c++]=new SensorToken::ReadData(it->second);
    }//for it
    
    /*std::cout<<"The token is"<<std::endl;
    for (size_t i=0;i<tk.size();i++){
      std::cout<<"["<<i<<"]"<<((SensorToken::ReadData*)&tk[i])->minId<<"~"<<((SensorToken::ReadData*)&tk[i])->maxId<<std::endl;
      }*/
    
  }//getWorkingOptimizedSplit


void VStouchSensor::isWorking(Callback<const HwStateNotification&>* cb,const HwInterface::WorkingToken& tk){
  const SensorToken::ReadData& r= *trust_cast<const SensorToken::ReadData*>(&tk);
  isWorking<Callback<const HwStateNotification&>*,callbackCaller>(cb,r);
}
  
/*void VStouchSensor::isWorking(Callback<const HwStateNotification&>* cb,const HwInterface::WorkingToken& tk){
  const SensorToken::ReadData& r= *trust_cast<const SensorToken::ReadData*>(&tk);
    try{
      s.recv(r.bid,M_VIN0+r.minId,buffer,2*(r.maxId-r.minId+1));      
      for (int si=r.minId;si<=r.maxId;si++){
	int g=r.gid[si];
	if (g>=0){
	  int v;
	  VstoneSerial::from14(v,&buffer[(si-r.minId)*2]);
	  if (v==0){
	    (*cb)(std::make_pair(g,FAULTYVALUEHWSTATE));
	  }else{
	    (*cb)(std::make_pair(g,WORKINGHWSTATE));
	  }
	}//g>=0	
      }//for
    }catch(exc::Exception& e){
      for (int si=r.minId;si<=r.maxId;si++){
	int g=r.gid[si];
	if (g>=0){
	  (*cb)(std::make_pair(g,UNREACHABLEHWSTATE));
	}//g>=0	
      }//for si
    }//catch
    
  }//isWorking
  */
 
  void VStouchSensor::setMode(int idx,int val){
    s.write1(idMap[idx].board,LED_MODE0+idMap[idx].id,val);
  }


  void VStouchSensor::rawSetDuty(int idx,int val){
    //std::cout<<"idx"<<idx<<"of size"<<idMap.size()<<std::endl;
    //std::cout<<"writing to "<<LED_DUTY0+idMap[idx].id<<" of "<<idMap[idx].board<<" the val "<<val<<std::endl;
     s.write1(idMap[idx].board,LED_DUTY0+idMap[idx].id,val);
  }

  void VStouchSensor::setDuty(int idx,double d){
    //std::cout<<"setting duty "<<d<<std::endl;
    rawSetDuty(idx,(int)round(d*0x7f));
  }

}//helium
