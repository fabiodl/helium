namespace helium{

    namespace supp{
    
    

    
    template<typename Sign> struct motOpT<Sign,IFACEMOTOP>{
      typedef motor::Interface::Position D;
    typedef motor::Interface Mot;
    };
    
    template<typename Sign> struct motOpT<Sign,IFACEBOOLMOTOP>{
      typedef bool D;
      typedef motor::Interface Mot;
    };

    template<typename Sign> struct motOpT<Sign,RAWMOTOP>{
      typedef typename Sign::Pos D;
      typedef typename motor::Raw<Sign>::Set Mot;
    };
    
    
    template<typename Sign> struct motOpT<Sign,HWMOTOP>{
      typedef typename Sign::Pos D;
      typedef typename motor::Raw<Sign>::Hardware Mot;
    };

    template<typename Sign> struct motOpT<Sign,HWBOOLMOTOP>{
      typedef bool D;
      typedef typename motor::Raw<Sign>::Hardware Mot;
    };
    


    }//sn supp

  
  template<typename Sign>
  motor::Interface::Property MultiMotorMap<Sign>::normProp(typename Sign::Prop value,const motor::PropertyRequest& r){
      return hw(r.joint)->convProp().normProp(value,propReq(r));
    }


  template<typename Sign>
    typename Sign::Prop MultiMotorMap<Sign>::unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r){
      return hw(r.joint)->convProp().unnormProp(value,propReq(r));
    }
    
  template<typename Sign>
    motor::Interface::Position MultiMotorMap<Sign>::normPos(typename Sign::Pos value,int joint){
      return hw(joint)->convPos().normPos(value,id(joint));
    }
   
  template<typename Sign>
    typename Sign::Pos MultiMotorMap<Sign>::unnormPos(motor::Interface::Position value,int joint){
      return hw(joint)->convPos().unnormPos(value,id(joint));
    }
    
  template<typename Sign>
    typename motor::Raw<Sign>::PropNormalizer& MultiMotorMap<Sign>::convProp(){
      return *this;
    }

  template<typename Sign>
    typename motor::Raw<Sign>::PosNormalizer& MultiMotorMap<Sign>::convPos(){
      return *this;
    }

  template<typename Sign>
    void MultiMotorMap<Sign>::addMapping(int virtId,typename motor::Raw<Sign>::Hardware* hw,int hwId){
      motMap[virtId]=std::make_pair(hw,hwId);
      std::vector<int>& hwIdMap=hardwares[hw];
      if ((int)hwIdMap.size()<hwId+1){
	hwIdMap.insert(hwIdMap.end(),hwId+1-hwIdMap.size(),INVALIDID);
      }
      hwIdMap[hwId]=virtId;
    }
   

  template<typename Sign>
    typename Sign::Pos MultiMotorMap<Sign>::getPotentiometer(int j,int checks){
      return hw(j)->getPotentiometer(id(j),checks);
    }


  template<typename Sign>
    void MultiMotorMap<Sign>::rawGetProperty(typename Sign::Prop& o, const motor::PropertyRequest & r){
      hw(r.joint)->rawGetProperty(o,propReq(r));
    }


  template<typename Sign>
    void MultiMotorMap<Sign>::rawSetProperty(const typename Sign::Prop& i, const motor::PropertyRequest & r){
      hw(r.joint)->rawSetProperty(i,propReq(r));
    }






  template<typename Sign>
    void MultiMotorMap<Sign>::rawGetGetterToken(typename motor::Raw<Sign>::Token& tk,const typename motor::Raw<Sign>::PropertyList &info){
      CbHolderGetterToken* gt=new CbHolderGetterToken();
      tk.reset(gt);
      PropertyListMap infos;
      splitPropertyList(infos,info,gt);
      gt->tokens.reset(infos.size());
      int c=0;
      
      for (typename PropertyListMap::iterator it=infos.begin();it!=infos.end();++it){
	it->first->rawGetGetterToken(gt->tokens[c].second,it->second); 
	c++;
      }	
      meh.check();
    }

  template<typename Sign>
    void MultiMotorMap<Sign>::rawGetOptimizedSplit(Array<typename motor::Raw<Sign>::Token>& tk,const typename motor::Raw<Sign>::PropertyList & info){
      CbHolderGetterToken* gt=new CbHolderGetterToken();
      PropertyListMap infos;
      splitPropertyList(infos,info,gt);
      int c=0;


      int totalSize=0;
      Array<Array<typename motor::Raw<Sign>::Token> > hwTokens(infos.size());
      
      for (typename PropertyListMap::iterator it=infos.begin();it!=infos.end();++it){
	it->first->rawGetOptimizedSplit(hwTokens[c],it->second); 
	totalSize+=hwTokens[c].size();
	c++;
      }	
      tk.resize(totalSize);
      
      typename PropertyListMap::iterator it=infos.begin();
      gt->setSingle(it->first,hwTokens[0][0]);      
      tk[0].reset(gt); //the first holds the callbacks
      c=1;      
      int srcHw=0;
      int srcIdx=1;
      for (;it!=infos.end();++it){
	for (;srcIdx<(int)hwTokens[srcHw].size();++srcIdx){
	  tk[c].reset(new GetterToken
		      (it->first,hwTokens[srcHw][srcIdx])
		      );
	}
	srcHw++;
	srcIdx=0;//reset here because we exclude element[0][0]	
      }//for infos
           
    }//rawGetOptimizedSplit


  template<typename Sign>
    void MultiMotorMap<Sign>::rawGetProperty(const typename motor::Raw<Sign>::Token& tk){
      const GetterToken* gt=trust_cast<const GetterToken*>(tk.get());
      for (size_t i=0;i<gt->tokens.size();i++){
	try{
	  gt->tokens[i].first->rawGetProperty(gt->tokens[i].second);
	}catch(exc::Exception& e){
	  meh.handle(e);
	}
      }            
      meh.check();
    }


  template<typename Sign>
    void MultiMotorMap<Sign>::setPowerState(int j,bool state){
      hw(j)->setPowerState(id(j),state);
    }


  template<typename Sign>
    bool MultiMotorMap<Sign>::getPowerState(int j){
      return hw(j)->getPowerState(id(j));
    }

  template<typename Sign>
    void MultiMotorMap<Sign>::getPowerState(bool* b){
      getFromHw<supp::HWBOOLMOTOP,&Hw::getPowerState>(tmpPower.data,b);
    }

  template<typename Sign>
    void MultiMotorMap<Sign>::hwRotate(int j, typename Sign::Pos p){
      hw(j)->hwRotate(id(j),p);      
    }

  template<typename Sign>
    void MultiMotorMap<Sign>::hwRotate(const typename Sign::Pos* p){
      setToHw<supp::HWMOTOP,&Hw::hwRotate>(tmpRaw.data,p);
    }

  template<typename Sign>
    void MultiMotorMap<Sign>::hwRotate(const bool* b,const typename Sign::Pos* p){
      setToHwConditional<supp::HWMOTOP,&Hw::hwRotate>(tmpRaw.data,p,b);
    }

  template<typename Sign>
    void MultiMotorMap<Sign>::rawRotate(int j, typename Sign::Pos p){
      hw(j)->rawRotate(id(j),p);      
    }

      template<typename Sign>
    void MultiMotorMap<Sign>::rawRotate(const typename Sign::Pos* p){
      setToHw<supp::RAWMOTOP,&RawMotor::Set::rawRotate>(tmpRaw.data,p);
    }

  template<typename Sign>
    void MultiMotorMap<Sign>::rawRotate(const bool* b,const typename Sign::Pos* p){
      setToHwConditional<supp::RAWMOTOP,&RawMotor::Set::rawRotate>(tmpRaw.data,p,b);
    }

        template<typename Sign>
    void MultiMotorMap<Sign>::connect(ConnReference<NormPosition>& conn, int j) {
      hw(j)->connect(conn,id(j));
    }
    
  template<typename Sign>
    void MultiMotorMap<Sign>::disconnect(ConnReference<NormPosition>& conn,int j){
      hw(j)->disconnect(conn,id(j));
    }
    
  template<typename Sign>
    void MultiMotorMap<Sign>::rawConnect(ConnReference<typename Sign::Pos>& conn,int j){
      hw(j)->rawConnect(conn,id(j));
    }
    
  template<typename Sign>
    void MultiMotorMap<Sign>::rawDisconnect(ConnReference<typename Sign::Pos>& conn,int j){
      hw(j)->rawDisconnect(conn,id(j));
    }

  template<typename Sign>
    typename Sign::Pos MultiMotorMap<Sign>::rawGetPosition(int j){
      return hw(j)->rawGetPosition(id(j));
    }

  template<typename Sign>
    void MultiMotorMap<Sign>::rawGetPosition(typename Sign::Pos* p){
      getFromHw<supp::RAWMOTOP,&RawMotor::Set::rawGetPosition>(tmpRaw.data,p);
    }


      template<typename Sign>
    void MultiMotorMap<Sign>::rotate(int j, NormPosition p){
      hw(j)->rotate(id(j),p);
    }

  template<typename Sign>
    void MultiMotorMap<Sign>::rotate(const NormPosition* p){
      setToHw<supp::IFACEMOTOP,&motor::Interface::rotate>(tmpNorm.data,p);
    }
    
  template<typename Sign>
    void MultiMotorMap<Sign>::rotate(const bool* b,const NormPosition* p){
      setToHwConditional<supp::IFACEMOTOP,&motor::Interface::rotate>(tmpNorm.data,p,b);
    }

  template<typename Sign>
  motor::Interface::Position MultiMotorMap<Sign>::getPosition(int j){
      return hw(j)->getPosition(id(j));
    }

  template<typename Sign>
    void MultiMotorMap<Sign>::getPosition(NormPosition* p){
      getFromHw<supp::IFACEMOTOP,&motor::Interface::getPosition>(tmpNorm.data,p);
    }

  template<typename Sign>
    motor::IfaceId MultiMotorMap<Sign>::getIfaceId(){
      return "multimotor";
    }

  template<typename Sign>
    HwState MultiMotorMap<Sign>::isWorking(int j){
      return hw(j)->isWorking(id(j));
    }

    
  template<typename Sign>
    void MultiMotorMap<Sign>::isWorking(Array<HwState>& s,const HwIdList& idList){
      for (typename HwMap::iterator it=hardwares.begin();
	   it!=hardwares.end();++it){	
	HwIdList idInList,thisHwReal;
	for (size_t i=0;i<idList.ids.size();++i){
	  int idx=idList.ids[i];
	  if (hw(idx)==it->first){
	    idInList.ids.push_back(i);
	    thisHwReal.ids.push_back(id(idx));
	  }//if
	}//for 	  
	try{
	  it->first->isWorking(tmpHwState,thisHwReal);
	}catch(exc::Exception& e){
	  meh.handle(e);
	}	 
	for (size_t i=0;i<idInList.ids.size();i++){
	  s[idInList.ids[i]]=tmpHwState[i];
	}	 
      }//for it
      meh.check();

    }



     template<typename Sign>
    void MultiMotorMap<Sign>::getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList &idList) const{
      std::vector<GetWorkingToken*> gwt;
      
      for (typename HwMap::const_iterator it=hardwares.begin();
	   it!=hardwares.end();++it){	
	HwIdList thisHwReal;
	for (size_t i=0;i<idList.ids.size();++i){
	  int idx=idList.ids[i];
	  if (hw(idx)==it->first){
	    thisHwReal.ids.push_back(id(idx));
	  }//if	  
	}
	Array<HwInterface::WorkingToken> hwTk;
	it->first->getWorkingOptimizedSplit(hwTk,thisHwReal);
	for (size_t i=0;i<hwTk.size();i++){
	  gwt.push_back(new GetWorkingToken(it->first,hwTk[i]));
	}	
      }//for it
      tk.resize(gwt.size());
      for (size_t i=0;i<gwt.size();i++){
	tk[i].reset(gwt[i]);
      }      
    }

  
  template<typename Sign>
  void MultiMotorMap<Sign>::isWorking(Callback<const HwStateNotification&>* cb,const HwInterface::WorkingToken& tk){

      const GetWorkingToken* gwt=
	trust_cast<const GetWorkingToken*>(tk.get());
      IdSwapCallback isc(hardwares[gwt->hw],cb);      
      gwt->hw->isWorking(&isc,gwt->tok);
    }
  

  template<typename Sign>
  MultiMotorMap<Sign>::MultiMotorMap(int virtMotorNum,int maxHwMotorNum):
      motMap(virtMotorNum),
      tmpPower(maxHwMotorNum), 
      tmpNorm(maxHwMotorNum), 
      tmpRaw(maxHwMotorNum),      
      tmpHwState(maxHwMotorNum)
    {
    }
  
  template<typename Sign>
  int MultiMotorMap<Sign>::getMotorNum(){
      return motMap.size();
    }


  template<typename Sign>
  void MultiMotorMap<Sign>::splitPropertyList(PropertyListMap& infos, const typename motor::Raw<Sign>::PropertyList &info,CbHolderGetterToken* gt){
      for (typename RawMotor::PropertyList::const_iterator it=info.begin();it!=info.end();++it){
	gt->callbacks.push_back(PropCallback(*it,id(it->second->joint),this));
	
	infos[hw(it->second->joint)].push_back
	  (
	   //callback*,request*
	   typename RawMotor::PropertyNotifyRequest(&gt->callbacks.back(),&gt->callbacks.back().hwReq)		 
	   );	
      }
  }
  


  template<typename Sign>
  MultiMotorMap<Sign>::IdSwapCallback::IdSwapCallback( std::vector<int>& pidMap,Callback<const HwStateNotification&>* pcb):
	idMap(pidMap),cb(pcb)
      {
	
      }


template<typename Sign>
template<typename T> 
void MultiMotorMap<Sign>::hwToVirt(T* virtData,const T* hwData,Hw* hw){
      std::vector<int>& dsts=hardwares[hw];
      for (size_t i=0;i<dsts.size();++i){
	if (dsts[i]!=INVALIDID){
	  virtData[dsts[i]]=hwData[i];
	}
      }//for i
    }


  template<typename Sign>
  template<typename T> 
  void MultiMotorMap<Sign>::virtToHw(T* hwData,const T* virtData,Hw* hw){
    for (size_t i=0;i<motMap.size();++i){
      if (motMap[i].first==hw){
	hwData[motMap[i].second]=virtData[i];
      }//if 
    }//for i
  }
  

  template<typename Sign>
  
    template<supp::MotorOperationLevel l,	     
	     void (supp::motOpT<Sign,l>::Mot::*m)(typename supp::motOpT<Sign,l>::D*)> 
  void MultiMotorMap<Sign>::getFromHw(typename supp::motOpT<Sign,l>::D* tmp,typename supp::motOpT<Sign,l>::D* data){
      for (typename HwMap::iterator it=hardwares.begin();
	   it!=hardwares.end();++it){	
	try{
	  (it->first->*m)(tmp);
	}catch(exc::Exception& e){
	  meh.handle(e);
	}
	hwToVirt(data,tmp,it->first);
      }//for it
      meh.check();
    }


  template<typename Sign>
      template<supp::MotorOperationLevel l,	     
	     void (supp::motOpT<Sign,l>::Mot::*m)(const typename supp::motOpT<Sign,l>::D*)
	     > 
  void MultiMotorMap<Sign>::setToHw(typename supp::motOpT<Sign,l>::D* tmp,const typename supp::motOpT<Sign,l>::D* data){
      for (typename HwMap::iterator it=hardwares.begin();
	   it!=hardwares.end();++it){	
	virtToHw(tmp,data,it->first);	
	try{
	  (it->first->*m)(tmp);
	}catch(exc::Exception& e){
	  meh.handle(e);
	}
      }//for
      meh.check();
    }


  template<typename Sign>
  template<supp::MotorOperationLevel l,	     
	     void (supp::motOpT<Sign,l>::Mot::*m)(const bool*,const typename supp::motOpT<Sign,l>::D*)      
	     > 
  void MultiMotorMap<Sign>::setToHwConditional(typename supp::motOpT<Sign,l>::D* tmp,const typename supp::motOpT<Sign,l>::D* data,const bool* b){
      for (typename HwMap::iterator it=hardwares.begin();
	   it!=hardwares.end();++it){	
	virtToHw(tmp,data,it->first);	
	virtToHw(tmpPower.data,b,it->first);
	try{
	  (it->first->*m)(tmpPower.data,tmp);
	}catch(exc::Exception& e){
	  meh.handle(e);
	}
      }//for
      meh.check();
    }



  template<typename Sign>
  void MultiMotorMap<Sign>::IdSwapCallback::operator()(const HwStateNotification& n){
	int virtid=idMap[n.first];
	if (virtid!=INVALIDID){
	  (*cb)(HwStateNotification(virtid,n.second));
	}
      }

template<typename Sign>
MultiMotorMap<Sign>::PropCallback::PropCallback(const PropCallback& pc):cb(pc.cb),req(pc.req),notification(pc.notification),
									hwReq(pc.hwReq){
  notification.second=&req;
}
  
template<typename Sign>
MultiMotorMap<Sign>::PropCallback::PropCallback(const typename motor::Raw<Sign>::PropertyNotifyRequest& pnr,int hardId,typename motor::Raw<Sign>::PropNormalizer* pnorm):
	cb(pnr.first),req(*pnr.second),notification(-1,&req,pnorm),
	hwReq(hardId,req.id,req.idx)
      {	
      }	

template<typename Sign>
void MultiMotorMap<Sign>::PropCallback::operator()(typename motor::Raw<Sign>::PropertyNotify* pn){
	notification.first=pn->first;
	(*cb)(&notification);
      }


  template<typename Sign>
  void MultiMotorMap<Sign>::GetterToken::setSingle(Hw* hw,typename motor::Raw<Sign>::Token& hwToken){
	tokens.reset(1);
	tokens[0].first=hw;
	tokens[0].second=hwToken;
      }
  
  template<typename Sign>
  MultiMotorMap<Sign>::GetterToken::GetterToken(Hw* hw,typename motor::Raw<Sign>::Token& hwToken):tokens(1){
    tokens[0].first=hw;
    tokens[0].second=hwToken;
  }

}//ns he
