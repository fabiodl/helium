/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


namespace helium{
  
  template<typename VsSrl>
  template<typename Data>
  void  VsCrcTouchSensor<VsSrl>::getReadDataSparseMap(std::map<int,Data>& m,const HwIdList& what) const{
    for (size_t i=0;i<what.ids.size();i++){
      int idx=what.ids[i];
      //std::cout<<"adding "<<idx<<"to board "<<sensMap[idx].board<<std::endl;
      m[sensMap[idx].board].markRead(sensMap[idx].chan,idx);      
      //std::cout<<"adding "<<idx<<"now "<<m[sensMap[idx].board].minId<<"~"<<m[sensMap[idx].board].minId<<std::endl;
    }    
  }
  
  template<typename VsSrl>
  template<typename Data>
  void  VsCrcTouchSensor<VsSrl>::getReadDataPackedMap(std::map<int,Data>& m,const HwIdList& what) const{
      for (size_t i=0;i<what.ids.size();i++){
	int idx=what.ids[i];
	m[sensMap[idx].board].markRead(sensMap[idx].chan,i);      
      }
  }
  



  template<typename VsSrl>
  template<typename Data>
  void  VsCrcTouchSensor<VsSrl>::getWriteDataSparseMap(std::map<int,Data>& m,const HwIdList& what) const{
    for (size_t i=0;i<what.ids.size();i++){
      int idx=what.ids[i];
      m[(*ledMap)[idx].board].markWrite((*ledMap)[idx].chan,idx);      
      //std::cout<<"adding "<<idx<<"now "<<m[idMap[idx].board].minId<<"~"<<m[idMap[idx].board].minId<<std::endl;
    }    
  }
  
  template<typename VsSrl>
  template<typename Data>
  void  VsCrcTouchSensor<VsSrl>::getWriteDataPackedMap(std::map<int,Data>& m,const HwIdList& what) const{
      for (size_t i=0;i<what.ids.size();i++){
	int idx=what.ids[i];
	m[(*ledMap)[idx].board].markWrite((*ledMap)[idx].chan,i);      
      }
  }

 
  

  //--BurstToken::BoardData
  
  
  //--BurstData
  
  
  
  template<typename VsSrl>
  void VsCrcTouchSensor<VsSrl>::assureBurstToken(const vsTs::BurstToken* t){
    if (burstData.activeToken!=t||!t->wasInit){
      burstData.activeToken=NULL; //we may end up in a  situation between old and new
      vsTs::BoardFlags f;
      
      for(size_t i=0;i<t->bdata.size();++i){
	t->bdata[i].getFlags(f);
	if (f!=burstData.curFlags[t->bdata[i].bid]){
	  try{
	    vs.send(t->bdata[i].bid,vsTs::CHAN_0_FLAGS,f.data,vsTs::CHANNELS);
	  }catch(exc::Exception& e){
	  meh.handle(e);
	  }
	}      
      }
      burstData.activeToken=t;
      t->wasInit=true;
    }//!=
  }
  
  template<typename VsSrl>
  void VsCrcTouchSensor<VsSrl>::burstBoard(const vsTs::BurstToken::BoardData& d){
    try{
      switch(d.type){
      case vsTs::BurstToken::BoardData::BURST_READ:
	vs.fastBurstRecv(d.bid,buffer,d.rSize,d.rCrc);
	break;
      case vsTs::BurstToken::BoardData::BURST_WRITE:
	vs.fastBurstSend(d.bid,rawDuty.data,d.wSize,d.wCrc);
	break;
      case vsTs::BurstToken::BoardData::BURST_READWRITE:
	vs.fastBurstSendRecv(d.bid,buffer,d.rSize,d.rCrc,
			     rawDuty.data,d.wSize,d.wCrc);
	break;
      }	
    }catch(exc::Exception& e){	
      if (d.type!=vsTs::BurstToken::BoardData::BURST_WRITE){
	meh.handle(exc::TouchSensorBoardException<UNREACHABLEHWSTATE>
		   (d.bid,boardToSensors[d.bid],toString(e))
		   );
      }else{
	HwIdList aff;
	for (size_t i=0;i<ledMap->size();i++){
	  if ((*ledMap)[i].board==d.bid){
	    aff.ids.push_back(i);
	  }
	}
	meh.handle(exc::TouchSensorBoardException<UNREACHABLEHWSTATE>
		   (d.bid,aff,toString(e))
		   );
      }
    }
    
  }
  
  
 

  template<typename VsSrl>
  void VsCrcTouchSensor<VsSrl>::readset(int* t,const vsTs::ReadSet& s){
    for (vsTs::ReadSet::const_iterator it=s.begin();it!=s.end();++it){
      const vsTs::ReadData& r=it->second;      
      try{
	//std::cout<<"reading board"<<it->first<<"from id "<<r.minId<<"size "<<(r.maxId-r.minId+1)<<std::endl;
	vs.recv(it->first,vsTs::M_VIN0+2*r.minId,buffer,
	       2*(r.maxId-r.minId+1));
	
	for (int si=r.minId;si<=r.maxId;si++){
	  int g=it->second.gid[si];
	  if (g>=0){
	    int res;
	    from14(res,&buffer[(si-r.minId)*2]);	    
	    if (res<validMin){	      
	      meh.handle(exc::SingleTouchSensorException<FAULTYVALUEHWSTATE>(res));
	    }
	    t[g]=res;
	  }//gid>=0
	}//for si             	
      }catch(exc::Exception& e){
	std::cout<<"exception for board "<<it->first<<std::endl;
	meh.handle(exc::TouchSensorBoardException<UNREACHABLEHWSTATE>
		  (it->first,boardToSensors[it->first],toString(e))
		   );
      }			
    }//for it 
    meh.check();
  }
  
  
  
  template<typename VsSrl>
  typename VsCrcTouchSensor<VsSrl>::BoardToSensor& VsCrcTouchSensor<VsSrl>::initBoardToSensor(){
    for (size_t i=0;i<sensMap.size();i++){
      boardToSensors[sensMap[i].board].ids.push_back(i);
    }
    return boardToSensors;
  }
  
  

  template<typename VsSrl>
  VsCrcTouchSensor<VsSrl>::VsCrcTouchSensor(VsSrl& pvs,const vsTs::RawIdMap& psensMap,
					    const vsTs::RawIdMap* pledMap,
					    vsTs::WriteTokenMaker* pwtm
					    ):vs(pvs),sensMap(psensMap),ledMap(pledMap),
					      wtm(pwtm),
					      burstData(initBoardToSensor().rbegin()->first+1 ),
					      validMin(1),rawDuty(psensMap.size()) {
    HwIdList l(sensMap.size());
    l.fillConsecutive();
    //std::cout<<"whole token"<<l<<std::endl;
    getReadDataSparseMap<vsTs::ReadData>(wholeToken,l);
    //std::cout<<"b to sens size "<<boardToSensors.size()<<std::endl;
    
  }
  
  template<typename VsSrl>
  void VsCrcTouchSensor<VsSrl>::setValidMin(int m){
    validMin=m;
  }
  
  


  /*template<typename VsSrl>
void VsCrcTouchSensor<VsSrl>::rawRead(int* t,const SensorToken& tk){    
  
}//rawRead
  */

  /*  template<typename VsSrl>
  void VsCrcTouchSensor<VsSrl>::rawRead(SensorValList<int>& v,const SensorToken& tk){    
  
    int p=0;
    int d=0;
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
 
  */
  template<typename VsSrl>
  int VsCrcTouchSensor<VsSrl>::rawRead(int idx){
    int v;
    try{
      v=vs.read<2,READRETRIES>(sensMap[idx].board,vsTs::M_VIN0+2*sensMap[idx].chan);
    }catch(exc::Exception& e){
      int b=sensMap[idx].board;
      throw exc::TouchSensorBoardException<UNREACHABLEHWSTATE>(b,boardToSensors[b],toString(e));
    }
    if (v<validMin){
      throw exc::SingleTouchSensorException<FAULTYVALUEHWSTATE>(idx);
    }
    return v;       
  }
  
  
  template<typename VsSrl>
  void VsCrcTouchSensor<VsSrl>::rawRead(int* t){
    readset(t,wholeToken);
  }
  


  template<typename VsSrl>
  void VsCrcTouchSensor<VsSrl>::rawRead(int* t, const HwIdList& toRead){
    vsTs::ReadSet m;
    getReadDataSparseMap<vsTs::ReadData>(m,toRead);
    readset(t,m);
  }
  
  
  template<typename VsSrl>
  void VsCrcTouchSensor<VsSrl>::rawRead(SensorValList<int>& v, const HwIdList& toRead){
    vsTs::ReadSet m;
    getReadDataPackedMap<vsTs::ReadData>(m,toRead);
    readset(v.vals.data,m);
  }
  
  
  template<typename VsSrl>
  void VsCrcTouchSensor<VsSrl>::getToken(Token& t, const HwIdList& toRead){
    std::map<int,vsTs::BurstToken::BoardData> m;
    getReadDataSparseMap<vsTs::BurstToken::BoardData>(m,toRead);
    if (wtm){
      getWriteDataSparseMap<vsTs::BurstToken::BoardData>(m,wtm->getBurstWriteIds());
      t=new vsTs::BurstToken(m,wtm->getWriterF());
    }else{
      t=new vsTs::BurstToken(m);
    }   
  }
  
  template<typename VsSrl>
  void VsCrcTouchSensor<VsSrl>::rawRead(int* t,const Token& tok){
    const vsTs::BurstToken& tk=*trust_cast<const vsTs::BurstToken*>(tok.get());
    
    if (tk.writerf){
      (*tk.writerf)(rawDuty.data);
    }
    assureBurstToken(&tk);
    
    for (size_t i=0;i<tk.bdata.size();i++){
      burstBoard(tk.bdata[i]);    //we bust the single boar on the buffer
      unsigned char* p=buffer;
      for (int c=0;c<vsTs::CHANNELS;c++){
	int g=tk.bdata[i].rgid[c];
	//std::cout<<"board "<<tk.bdata[i]<<" chan "<<c<<" id "<<g<<

	if (g>=0){
	  from14(t[g],p);	
	  p+=2;//the 2 bytes read. checksm already managed by vserial
	  if (t[g]<validMin){	  
	    meh.handle(exc::SingleTouchSensorException<FAULTYVALUEHWSTATE>(g));
	}
	}
      }
    }
    meh.check();  
  }

  template<typename VsSrl>
  size_t VsCrcTouchSensor<VsSrl>::size() const{
    return sensMap.size();
  }
  

  
  
  template<typename VsSrl>
  HwState  VsCrcTouchSensor<VsSrl>::isWorking(int idx){   
    try{
      return vs.read<2,READRETRIES>(sensMap[idx].board,vsTs::M_VIN0+2*sensMap[idx].chan)<validMin?
	FAULTYVALUEHWSTATE:UNREACHABLEHWSTATE;    
    }catch(exc::Exception& ){
      return UNREACHABLEHWSTATE;
    }
  }
  
  
  
  
  inline void vsCrcTsVectAssign(HwState* state,int idx,HwState value){
    state[idx]=value;
  }
  
  
  
  inline void vsCrcTsCallCb(Callback<const HwStateNotification&>*cb,int idx,HwState value){
    (*cb)(std::make_pair(idx,value));
  }
  
  

  
  template<typename VsSrl>
  template<typename Param,void (assigner)(Param,int,HwState)>
  void VsCrcTouchSensor<VsSrl>::isWorking(Param target,int bid,const vsTs::ReadData &r){
    try{
      //std::cout<<"maxid="<<r.maxId<<",mind="<<r.minId<<std::endl;
      vs.recv(bid,vsTs::M_VIN0+r.minId,buffer,2*(r.maxId-r.minId+1));	
      for (int si=r.minId;si<=r.maxId;si++){
	int g=r.gid[si];
	//std::cout<<"checking sensor "<<g<<std::endl;
	if (g>=0){
	  int v;	    	  
	  from14(v,&buffer[(si-r.minId)*2]);	  
	  assigner(target,g,v==0?FAULTYVALUEHWSTATE:WORKINGHWSTATE);
	}//g>=0
      }//for si	
    }catch(exc::Exception& ){
      //std::cout<<"caugth!"<<std::endl;
      for (int si=r.minId;si<=r.maxId;si++){
	int g=r.gid[si];
	if (g>=0){
	  assigner(target,g,UNREACHABLEHWSTATE);
	}
      }//for si
    }//catch
  }
  


  
  template<typename VsSrl>
  void VsCrcTouchSensor<VsSrl>::isWorking(Array<HwState>& state,const HwIdList& toTest){
    vsTs::ReadSet toRead;
    getReadDataPackedMap<vsTs::ReadData>(toRead,toTest);
    for (vsTs::ReadSet::iterator it=toRead.begin();it!=toRead.end(
);++it){
      isWorking<HwState*,vsCrcTsVectAssign>(state.data,it->first,it->second);     
    }//for it
  }//isWorking
  
  template<typename VsSrl>
  void VsCrcTouchSensor<VsSrl>::getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList & toTest) const{
    std::map<int,vsTs::WorkToken::Ref > toRead; 
    getReadDataSparseMap<vsTs::WorkToken::Ref > (toRead,toTest);
  
  tk.reset(toRead.size());
  
  int c=0;
  for (typename std::map<int,vsTs::WorkToken::Ref >::iterator it=toRead.begin();it!=toRead.end();++it){
      
      tk[c++].reset(it->second.setBid(it->first));
  }//for it
  
  
  }//getWorkingOptimizedSplit

template<typename VsSrl>
void VsCrcTouchSensor<VsSrl>::isWorking(Callback<const HwStateNotification&>* cb,const HwInterface::WorkingToken& tk){
  const vsTs::WorkToken& r= *trust_cast<const vsTs::WorkToken*>(tk.get());
  isWorking<Callback<const HwStateNotification&>*,vsCrcTsCallCb>(cb,r.bid,r);
}
  
 
template<typename VsSrl>
void VsCrcTouchSensor<VsSrl>::setMode(int idx,vsTs::Mode mode){
  unsigned char flags=
    vs.read<1,READRETRIES>((*ledMap)[idx].board,vsTs::CHAN_0_FLAGS+(*ledMap)[idx].chan);
  switch (mode){
  case vsTs::LEDMODE:
    if ((flags&vsTs::IS_LED)==0){
      flags|=vsTs::IS_LED;
      vs.write<1,READRETRIES>((*ledMap)[idx].board,vsTs::CHAN_0_FLAGS+(*ledMap)[idx].chan,flags);
    }
    break;
  case vsTs::SENSORMODE:
    if ((flags&vsTs::IS_LED)==0){
      flags&=~vsTs::IS_LED;
      vs.write<1,READRETRIES>((*ledMap)[idx].board,vsTs::CHAN_0_FLAGS+(*ledMap)[idx].chan,flags);
    }
    break;
  }
}

template<typename VsSrl>
void VsCrcTouchSensor<VsSrl>::rawSetDuty(int idx,int val){  
  vs.write<1,READRETRIES>((*ledMap)[idx].board,vsTs::LED_0_DUTY+(*ledMap)[idx].chan,val);
}

template<typename VsSrl>
  void VsCrcTouchSensor<VsSrl>::setDuty(int idx,double d){
    //std::cout<<"setting duty "<<d<<std::endl;
  rawSetDuty(idx,(int)round(d*0x7f));
}

}//helium




 /*template<ResWriterType>
  class ResWriter;
  

  template<>
  class ResWriter<VsCrcTouchSensor<VsSrl>::SPARSEWRITER>{
  public:
    int g;
    inline int operator()(){
      return g;
    }
  };
  
  
  template<>
  class ResWriter<VsCrcTouchSensor<VsSrl>::PACKEDWRITER>{
    int p;
  public:
    int g;
    ResWriter():p(0);

    inline int operator()(){
      return p++;
    }
    };*/

