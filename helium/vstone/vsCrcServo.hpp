/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/math/tAbs.h>
namespace helium{


  template<typename VsSrl>
  motor::Raw<VServoSign>::PropNormalizer& VsCrcServo<VsSrl>::convProp(){
    return prNorm;
  }
  
  template<typename VsSrl>
  motor::Raw<VServoSign>::PosNormalizer& VsCrcServo<VsSrl>::convPos(){
  return psNorm;
  }
  

  template<typename VsSrl>
    void VsCrcServo<VsSrl>::manageError(int j){
      lastSent[j]=VServoSign::INVALIDPOS;
    }

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::throwError(int j,const exc::Exception& e){
    lastSent[j]=VServoSign::INVALIDPOS;
    throw exc::SingleMotorException<UNREACHABLEHWSTATE>(j,e.what());
  }
  

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::throwError(int j,const std::string& s){
    lastSent[j]=VServoSign::INVALIDPOS;
    throw exc::SingleMotorException<UNREACHABLEHWSTATE>(j,s);
  }

  template<typename VsSrl>
  motor::Raw<VServoSign>::Position VsCrcServo<VsSrl>::getPotentiometer(int joint,int checks){
    try{
      int c=0;
      int v=vserial.read<2,RETRIES>(idMap.rawId[joint],vservo::M_POS);
      while(c<checks){
	int nv=vserial.read<2,RETRIES>(idMap.rawId[joint],vservo::M_POS);
	if (tAbs(nv-v)<VServoSign::TORAW*3*M_PI/180){
	  c++;
	}else{
	  c=0;
	}
	v=nv;
      }
      //std::cout<<"potentiometer read, "<<std::hex<<v<<std::dec<<std::endl;
      return v;
    }catch(exc::Exception& e){
      throwError(joint,e);
      throw;
    }
  }

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::rawGetProperty(RawProperty& o, const motor::PropertyRequest & r){
    try{
      int addr=vservo::getAddr(r);
      //std::cout<<"reading addr"<<addr<<" of motor "<<r.joint<<std::endl;
      switch(vservo::Prop::size[addr]){
      case 1:
	o=vserial.read<1,RETRIES>(idMap.rawId[r.joint],addr);
	break;
      case 2:
	o=vserial.read<2,RETRIES>(idMap.rawId[r.joint],addr);
	break;
      case 4:
	o=vserial.read<4,RETRIES>(idMap.rawId[r.joint],addr);
	break;
      case -1:{
	std::cout<<"DUMPING TABLE"<<std::endl;
	unsigned char buff[33];
	for (int i=0;i<6;i++){	  
	  memset(buff,0,sizeof(char)*16);
	  vserial.recv(idMap.rawId[r.joint],i*16,buff,16);
	  for (int j=0;j<16;j++){
	    std::cout<<i*16+j<<"="<<buff[j]<<std::endl;
	  }
	  std::cout<<std::dec<<std::endl;
	}
      }
	break;
      default:
	throw exc::InvalidOperation("VsCrcServo:Unknown size for data at address "+toString(addr));
      }
    }catch(exc::Exception& e){
      throwError(r.joint,e);
    }
  }//rawGetProperty
  
  
  template<typename VsSrl>
  void VsCrcServo<VsSrl>::rawSetProperty(const RawProperty& v, const motor::PropertyRequest & r){
    try{
      int addr=vservo::getAddr(r);
      switch(vservo::Prop::size[addr]){
      case 1:
	vserial.write<1,RETRIES>(idMap.rawId[r.joint],addr,v);
	break;
      case 2:
	vserial.write<2,RETRIES>(idMap.rawId[r.joint],addr,v);
	break;
      case 4:
	vserial.write<4,RETRIES>(idMap.rawId[r.joint],addr,v);
	break;
    default:
      throw exc::InvalidOperation("VsCrcServo:Unknown size for data at address "+toString(addr));
      }
    }catch(exc::Exception& e){
      throwError(r.joint,e);
    }    
  }
  
  template<typename VsSrl>
  void VsCrcServo<VsSrl>::rawGetGetterToken(Token& tk,const PropertyList &info){
    tk.reset(new vservo::GetterToken(info,convProp()));  
  }

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::rawGetOptimizedSplit(Array <Token>& tk,const motor::Raw<VServoSign>::PropertyList &info){
    vservo::GetterToken::splitByJoint(tk,info,convProp());  
  }
  
  template<typename VsSrl>
  void VsCrcServo<VsSrl>::rawGetProperty(const Token& tk){
    vservo::GetterToken* gk=const_cast<vservo::GetterToken*>(trust_cast<const vservo::GetterToken*>(&tk));
  //std::cout<<"|using "<<gk<<std::endl;
  for (size_t i=0;i<gk->props.size();i++){
    try{
      rawGetProperty(gk->props[i].first,*gk->props[i].second); //this already throws exceptions of motor type
      gk->notify(i);
    }catch(exc::Exception& e){
      manageError(i);
      meh.handle(e);
    }
  }//for i
  meh.check();
}



template<typename VsSrl>
HwState VsCrcServo<VsSrl>::isWorking(int j){
  try{
    int val=vserial.read<1,1>(idMap.rawId[j],vservo::SYS_SID);
    return (val==idMap.rawId[j]?WORKINGHWSTATE:FAULTYVALUEHWSTATE);
  }catch(exc::Exception& ){
    return UNREACHABLEHWSTATE;
  }  
}

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::isWorking(Array<HwState>& s,const HwIdList& l){
    //it makes no sense to try to read all at once, if something times out we need to check individually anyway
    for (size_t i=0;i<l.ids.size();i++){
      s[i]=isWorking(l.ids[i]);
    }    
  }


  template<typename VsSrl>
  void VsCrcServo<VsSrl>::getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList & l) const{
    tk.resize(l.ids.size());
    for(size_t i=0;i<l.ids.size();i++){
      tk[i].reset(new DeletableData<int>(l.ids[i]));
    }
  }

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::isWorking(Callback<const HwStateNotification&>* cb,const WorkingToken& tk){
    int id=trust_cast<const DeletableData<int>*>(&tk)->data;
    (*cb)(std::make_pair(id,isWorking(id)));
  }


  template<typename VsSrl>
  void VsCrcServo<VsSrl>::setPowerState(int j,bool state){  
    lastSent[j]=VServoSign::INVALIDPOS;      
    try{
      if (state){      
	//unlock is done in rotate when necessary.	
	unsigned char buff[4];
	int calibMin,calibMax;
	int trials=0;
	do{
	  vserial.recv(idMap.rawId[j],vservo::CALIBMIN,buff,4);
	  calibMin=from14(buff);
	  calibMax=from14(buff+2);	
	  if (calibMin==limiter.getMin(j)&&calibMax==limiter.getMax(j)){
	    break;
	  }	
	  std::cout<<"retrieved limits "<<std::hex<<calibMin<<" ~ "<<std::hex<<calibMax<<std::endl;
	  if (trials++==3){
	    throwError(j,"motor cannot be set its limits");	   
	  }
	  to14(buff,limiter.getMin(j));
	  to14(buff+2,limiter.getMax(j));	  
	  vserial.send(idMap.rawId[j],vservo::CALIBMIN,buff,4);//we send both	  
	}while(calibMin!=limiter.getMin(j)||calibMax!=limiter.getMax(j));
	
      }//state 
      vserial.write<1,RETRIES>(idMap.rawId[j],vservo::PWM_OFF,state?1:0);    
    }catch(exc::Exception& e){
      throwError(j,e);
    }
  }


  template<typename VsSrl>
  bool VsCrcServo<VsSrl>::getPowerState(int j){
    try{
      return vserial.read<1,RETRIES>(idMap.rawId[j],vservo::PWM_OFF);
    }catch(exc::Exception& e){
      throwError(j,e);
      throw;//for compiler warnings
    }
  }

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::getPowerState(bool* b){
    //std::cout<<"COULD optimize here..."<<std::endl;
    for (size_t j=0;j<lastSent.size();j++){
      try{
	b[j]=getPowerState(j);
      }catch(exc::Exception& e){
	manageError(j);
	meh.handle(e);
      }
    }
    meh.check();    
  }

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::hwRotate(int j,motor::Raw<VServoSign>::Position pos){    
    try{
      if (lastSent[j]==VServoSign::INVALIDPOS){
	vserial.unlock(idMap.rawId[j]);
      }
      vserial.write<2,RETRIES>(idMap.rawId[j],vservo::FB_TPOS,pos);
      lastSent[j]=pos;
      //std::cout<<"set lastSent["<<j<<"]="<<std::hex<<lastSent[j]<<"@"<<&lastSent[j] <<std::dec<<std::endl;
    }catch(exc::Exception& e){
      throwError(j,e);
    }
  }

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::hwRotate(const motor::Raw<VServoSign>::Position* v){    
    try{ //try total sel, if it fails try one by one
      unsigned char data[2];
      for (size_t j=0;j<lastSent.size();j++){
	to14(data,v[j]);
	expectedCrc[j]=vserial.initSend(idMap.rawId[j],vservo::FB_TPOS,data,2);
      }
      vserial.finalizeSend(receivedCrc.data,expectedCrc.data,lastSent.size());      
      for (size_t j=0;j<lastSent.size();j++){
	lastSent[j]=v[j];
	//std::cout<<"gset lastSent["<<j<<"]="<<v[j]<<std::endl;
      }
    }catch(exc::Exception& ){      
      for (size_t j=0;j<lastSent.size();j++){
	try{
	  hwRotate(j,v[j]);
	}catch(exc::Exception& e){
	  manageError(j);
	  meh.handle(e);
	}
      }
      meh.check();
    }//catch
  }//hwRotate pos*


  template<typename VsSrl>
  void VsCrcServo<VsSrl>::hwRotate(const bool* b,const motor::Raw<VServoSign>::Position* v){

    try{
      unsigned char data[2];
      int c=0;
      for (size_t j=0;j<lastSent.size();j++){	
	if (b[j]){
	  to14(data,v[j]);
	  expectedCrc[c]=vserial.initSend(idMap.rawId[j],vservo::FB_TPOS,data,2);	  
	  c++;
	}
      }
      vserial.finalizeSend(receivedCrc.data,expectedCrc.data,c);                 
      for (size_t j=0;j<lastSent.size();j++){
	if (b[j]){
	  lastSent[j]=v[j];
	  //std::cout<<"bset lastSent["<<j<<"]="<<v[j]<<std::endl;
	}
      }
    }catch(exc::Exception& ){
      for (size_t j=0;j<lastSent.size();j++){
	try{
	  if (b[j]){	
	    hwRotate(j,v[j]);
	  }
	}catch(exc::Exception& e){
	  manageError(j);
	  meh.handle(e);
	}
      }
      meh.check();
    }//catch
  }//hwRotate bool* pos*


    template<typename VsSrl>
      void VsCrcServo<VsSrl>::rawRotate(int j,motor::Raw<VServoSign>::Position pos){      
      VServoSign::Pos lp=limiter.limit(pos,j);
      if (lp==lastSent[j]) return;
      hwRotate(j,lp);
    }

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::rawRotate(const motor::Raw<VServoSign>::Position* v){
    for (size_t j=0;j<lastSent.size();j++){
      VServoSign::Pos lp=limiter.limit(v[j],j);
      toRotate[j]=(lp!=lastSent[j]);
      limitedPos[j]=lp;
    }
    hwRotate(toRotate.data,limitedPos.data);
  }

  
  template<typename VsSrl>
  void VsCrcServo<VsSrl>::rawRotate(const bool* b,const motor::Raw<VServoSign>::Position* v){
    setAll(toRotate,false);
    for (size_t j=0;j<lastSent.size();j++){
      if (b[j]){
	VServoSign::Pos lp=limiter.limit(v[j],j);
	toRotate[j]=(lp!=lastSent[j]);
	limitedPos[j]=lp;
      }
    }//for
    hwRotate(toRotate.data,limitedPos.data);
  }

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::rotate(int j, motor::Interface::Position p){
    rawRotate(j,psNorm.unnormPos(p,j));
  }
  
  template<typename VsSrl>
  void VsCrcServo<VsSrl>::rotate(const motor::Interface::Position * v){
    for (size_t j=0;j<idMap.rawId.size();j++){
      limitedPos[j]=psNorm.unnormPos(v[j],j);
    }
    rawRotate(limitedPos.data);
  }

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::rotate(const bool* b,const motor::Interface::Position * v){
    for (size_t j=0;j<idMap.rawId.size();j++){
      if (b[j]){
	limitedPos[j]=psNorm.unnormPos(v[j],j);
      }
    }
    rawRotate(b,limitedPos.data);
  }

  template<typename VsSrl>
  motor::IfaceId VsCrcServo<VsSrl>::getIfaceId(){
    return "vservo";
  }


  template<typename VsSrl>
  void VsCrcServo<VsSrl>::unlock(int id){
    //std::cout<<"unlocking" <<id<<" = "<<idMap.rawId[id]<<std::endl;
    vserial.unlock(idMap.rawId[id]);
  }


  template<typename VsSrl>
  void VsCrcServo<VsSrl>::connect(ConnReference<NormPosition>&, int){
    throw exc::InvalidOperation("VsCrcServo disables connection for efficiency");
  }
  
  template<typename VsSrl>
  void VsCrcServo<VsSrl>::disconnect(ConnReference<NormPosition>&,int j){
    throw exc::InvalidOperation("VsCrcServo disables connection for efficiency");
  }
  
  template<typename VsSrl>
  void VsCrcServo<VsSrl>::rawConnect(ConnReference<RawPosition>& conn,int j){
    throw exc::InvalidOperation("VsCrcServo disables connection for efficiency");
  }
  
  template<typename VsSrl>
  void VsCrcServo<VsSrl>::rawDisconnect(ConnReference<RawPosition>& conn,int j){
    throw exc::InvalidOperation("VsCrcServo disables connection for efficiency");
  }
  

  template<typename VsSrl>
  motor::Raw<VServoSign>::Position VsCrcServo<VsSrl>::rawGetPosition(int j){
    //std::cout<<"asked position:"<<j<<std::endl;
    if (lastSent[j]!=VServoSign::INVALIDPOS){
      //std::cout<<"returning cache" <<std::hex<<lastSent[j]<<std::dec<<std::endl;
      return lastSent[j];
    }
    try{      
      int p=vserial.read<2,RETRIES>(idMap.rawId[j],vservo::FB_TPOS);
      //std::cout<<"p is "<<std::hex<<p<<std::dec<<std::endl;
      return lastSent[j]=p;
    }catch (exc::Exception& e){
      throwError(j,e);
      throw; //for compiler warning
    }
  }

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::rawGetPosition( motor::Raw<VServoSign>::Position* p){
    try{
      int c=0;
      //std::cout<<"asked global position:"<<std::endl;
      for (size_t j=0;j<lastSent.size();j++){
	//std::cout<<"LAST"<<std::hex<<"@"<<&lastSent[j]<<"=="<<lastSent[j]<<std::dec<<std::endl;
	if (lastSent[j]==VServoSign::INVALIDPOS){
	  expectedCrc[c]=vserial.initRecv(idMap.rawId[j],vservo::FB_TPOS,2);
	  c++;
	}else{
	  p[j]=lastSent[j];
	  //std::cout<<j<<"is old "<<std::hex<<p[j]<<"lastsent "<<lastSent[j] <<std::dec<<std::endl;
	}
      }
      if (c){
	vserial.finalizeRecv(receivedData.data,expectedCrc.data,3,c);
	c=0;
	for (size_t j=0;j<lastSent.size();j++){
	  if (lastSent[j]==VServoSign::INVALIDPOS){
	    from14(p[j],receivedData.data+c*3);
	    //std::cout<<"j is new "<<std::hex<<p[j]<<std::dec<<std::endl;
	    c++;
	  }
	}
      }
    }catch(exc::Exception& ){
      for (size_t j=0;j<lastSent.size();j++){
	try{
	  p[j]=rawGetPosition(j);
	}catch(exc::Exception& e){
	  meh.handle(e);
	}
      }
      meh.check();      
    }
    /*for (size_t j=0;j<lastSent.size();j++){
      std::cout<<" returning "<<j<<" "<<std::hex<<p[j]<<std::dec<<std::endl;
      }*/
  }



  template<typename VsSrl>
  motor::Interface::Position VsCrcServo<VsSrl>::getPosition(int j){
    return psNorm.normPos(rawGetPosition(j),j);
  }

  template<typename VsSrl>
  void VsCrcServo<VsSrl>::getPosition(motor::Interface::Position* p){
    rawGetPosition(limitedPos.data);
    for (size_t j=0;j<lastSent.size();j++){
      p[j]=psNorm.normPos(limitedPos[j],j);
    }
  }


template<typename VsSrl>

int  VsCrcServo<VsSrl>::getMotorNum(){
  return lastSent.size();
}


  template<typename VsSrl>
  VsCrcServo<VsSrl>::VsCrcServo(VsSrl& s, IdMapper& pidMap,motor::Calib<VServoSign>& calib):
    vserial(s),idMap(pidMap),psNorm(calib),prNorm(psNorm),limiter(calib),lastSent(calib.data.size()),expectedCrc(calib.data.size()),receivedCrc(calib.data.size()),receivedData(calib.data.size()*3),toRotate(calib.data.size()),limitedPos(calib.data.size())
  {
    setAll(lastSent,VServoSign::INVALIDPOS);
    /*for (size_t i=0;i<calib.data.size();i++){
      std::cout<<lastSent[i]<<std::endl;
      }*/

    for (size_t j=0;j<calib.data.size();j++){
      unlock(j);
    }
  }
    
}//ns helium
