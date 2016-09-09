/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/math/tAbs.h>
#include <helium/vector/operations.h>
#include <helium/dynamixel/dynamixelMotor.h>
#include <helium/dynamixel/dynamixelTable.h>
#include <helium/robot/motorException.h>
#include <helium/util/byteConversion.h>
namespace helium{

  





  template<typename Dsrl,typename Sign>
  
  motor::Interface::Property DynamixelMotor<Dsrl,Sign>::PropNormalize::normProp(int value,const motor::PropertyRequest& r){
      switch (r.id){
      case motor::TARGETPOSITION:
      case motor::POTENTIOPOSITION:
	return posNorm.normPos(value,r.joint);
      case motor::POWER:
      case motor::TEMP:       
	return value;	
      default:
	throw exc::InvalidOperation("Conversion for property "+toString(r.id)+" not available");
      }       
    }

    
  template<typename Dsrl,typename Sign>
  typename motor::Raw<Sign>::Property DynamixelMotor<Dsrl,Sign>::PropNormalize::unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r){
    switch (r.id){
    case motor::TARGETPOSITION:
    case motor::POTENTIOPOSITION:
      return posNorm.unnormPos(value,r.joint);
    case motor::POWER:
    case motor::TEMP:       
      return value;	
    default:
      throw exc::InvalidOperation("Conversion to raw for property "+toString(r.id)+" not available");
    }       
    
  }





  template<typename Dsrl,typename Sign>
    typename motor::Raw<Sign>::PropNormalizer& DynamixelMotor<Dsrl,Sign>::convProp(){
    return prNorm;
  }


  template<typename Dsrl,typename Sign>
  typename motor::Raw<Sign>::PosNormalizer& DynamixelMotor<Dsrl,Sign>::convPos(){
    return psNorm;
  }

  
  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::manageError(int j){
    lastSent[j]=Sign::INVALIDPOS;
  }


  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::throwError(int j,const exc::Exception& e){
    manageError(j);
    throw exc::SingleMotorException<UNREACHABLEHWSTATE>(j,e.what());
  }

  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::assureCalib(int j){
    unsigned char buffer[4];
    unsigned char expected[4];
    toLE(expected,limiter.getMin(j));
    toLE(expected+2,limiter.getMax(j));
    
    try{
    do{
      dsrl.recv(idMap.rawId[j],dynamixel::CWANGLELIMITL,buffer,4);     
      if (equalsn<4>(buffer,expected)){
	break;
      }
      dsrl.send(idMap.rawId[j],dynamixel::CWANGLELIMITL,expected,4);     
    }while(true);
    }catch(exc::Exception& e){
      throwError(j,e);
    }
  }


  template<typename Dsrl,typename Sign>
  typename Sign::Pos DynamixelMotor<Dsrl,Sign>::getPotentiometer(int j,int checks){
    try{
      int c=0;
      typename Sign::Pos  v=dsrl.read<2>(idMap.rawId[j],dynamixel::M_POS);
      while(c<checks){
	typename Sign::Pos  nv=dsrl.read<2>(idMap.rawId[j],dynamixel::M_POS);
	if (tAbs(nv-v)<Sign::TORAW*3*M_PI/180){
	  c++;
	}else{
	  c=0;
	}
	v=nv;
      }
      //std::cout<<"potentiometer read, "<<std::hex<<v<<std::dec<<std::endl;
      return v;
    }catch(exc::Exception& e){
      throwError(j,e);
      throw;//compiler warning that the function does not return
    }
  }  
 
  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::rawGetProperty(RawProperty& o, const motor::PropertyRequest & r){
    try{
      int addr=dynamixel::getAddr(r);
      //std::cout<<"reading addr"<<addr<<" of motor "<<r.joint<<std::endl;
      switch(dynamixel::Prop::size[addr]){
      case 1:
	o=dsrl.read<1>(idMap.rawId[r.joint],addr);
	break;
      case 2:
	o=dsrl.read<2>(idMap.rawId[r.joint],addr);
	break;	
      case -2:{
	std::cout<<"DUMPING TABLE"<<std::endl;
	unsigned char buff[33];
	for (int i=0;i<6;i++){	  
	  memset(buff,0,sizeof(char)*16);
	  dsrl.recv(idMap.rawId[r.joint],i*16,buff,16);
	  for (int j=0;j<16;j++){
	    std::cout<<i*16+j<<"="<<buff[j]<<std::endl;
	  }
	  std::cout<<std::dec<<std::endl;
	}
      }
	break;
      case 0:
	throw exc::InvalidOperation("Dynamixel:The address "+toString(addr)+" is the high part of a data");
      case -1:
      default:
	throw exc::InvalidOperation("Dynamixel:Unknown size for data at address "+toString(addr));
      }
    }catch(exc::Exception& e){
      throwError(r.joint,e);

    }
  }//rawGetProperty

  

  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::rawSetProperty(const RawProperty& v, const motor::PropertyRequest & r){
    try{
      int addr=dynamixel::getAddr(r);
      switch(dynamixel::Prop::size[addr]){
      case 1:
	dsrl.write<1>(idMap.rawId[r.joint],addr,v);
	break;
      case 2:
	dsrl.write<2>(idMap.rawId[r.joint],addr,v);
	break;
      case 0:
	throw exc::InvalidOperation("Dynamixel:The address "+toString(addr)+" is the high part of a data");
      case -1:
      default:
      throw exc::InvalidOperation("Dynamixel:Unknown size for data at address "+toString(addr));
      }
    }catch(exc::Exception& e){
      throwError(r.joint,e);
    }    
  }


  template<typename Dsrl,typename Sign>  
  void DynamixelMotor<Dsrl,Sign>::rawGetGetterToken(typename DynamixelMotor<Dsrl,Sign>::Token& tk,const PropertyList &info){
    tk.reset(new GetterToken(info,convProp()));
  }
  
  template<typename Dsrl,typename Sign>  
  void DynamixelMotor<Dsrl,Sign>::rawGetOptimizedSplit(Array<typename DynamixelMotor<Dsrl,Sign>::Token>& tk,const PropertyList & info){
    GetterToken::splitByJoint(tk,info,convProp());
  }



  template<typename Dsrl,typename Sign>  
  void DynamixelMotor<Dsrl,Sign>::rawGetProperty(const typename DynamixelMotor<Dsrl,Sign>::Token& tk){
    GetterToken* gk=const_cast<GetterToken*>(trust_cast<const GetterToken*>(&tk));
  for (size_t i=0;i<gk->props.size();i++){
    try{
      rawGetProperty(gk->props[i].first,*gk->props[i].second); //this already throws exceptions of motor type
      gk->notify(i);
    }catch(exc::Exception& e){
      meh.handle(e);
    }
  }//for i
  meh.check();
}

  template<typename Dsrl,typename Sign>  
  void DynamixelMotor<Dsrl,Sign>::setPowerState(int j,bool state){
    lastSent[j]=Sign::INVALIDPOS;
    try{
      if (state){
	assureCalib(j);
      }
      dsrl.write<1>(idMap.rawId[j],dynamixel::TORQUEENABLE,state?1:0);
    }catch(exc::Exception& e){
      throwError(j,e);
    }
  }


  template<typename Dsrl,typename Sign>  
  bool DynamixelMotor<Dsrl,Sign>::getPowerState(int j){
    try{
      return dsrl.read<1>(idMap.rawId[j],dynamixel::TORQUEENABLE);
    }catch(exc::Exception& e){
      throwError(j,e);
      throw; //compiler warning that the func does not return
    }
  }


  template<typename Dsrl,typename Sign>  
  void DynamixelMotor<Dsrl,Sign>::getPowerState(bool* b){
    for(size_t j=0;j<lastSent.size();j++){
      try{      
	b[j]=getPowerState(j);//already manages the error
      }catch(exc::Exception& e){
	meh.handle(e);
      }
    }//for
    meh.check();
  }


  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::hwRotate(int j,typename Sign::Pos pos){ 
    try{
      dsrl.write<2>(idMap.rawId[j],dynamixel::TARGETPOS,pos);
      lastSent[j]=pos;
    }catch(exc::Exception& e){
      throwError(j,e);
    }
  }



  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::hwRotate(const typename Sign::Pos* v){    
    try{
      unsigned char* buff=dsrl.getSyncWriteBuffer();
      for (size_t j=0;j<lastSent.size();j++){
	buff[j*3]=(unsigned char)idMap.rawId[j];
	toLE(buff+j*3+1,v[j]);
      }
      dsrl.syncWrite(dynamixel::TARGETPOS,lastSent.size(),2);
      for (size_t j=0;j<lastSent.size();j++){
	lastSent[j]=v[j];
      }
    }catch(exc::Exception& e){
      for (size_t j=0;j<lastSent.size();j++){
	manageError(j);
      }
      HwIdList all(lastSent.size());
      all.fillConsecutive();
      throw exc::MultipleMotorException<UNREACHABLEHWSTATE>(all,e.what());
    }

  }//hwRotate int*
    

  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::hwRotate(const bool* b,const typename Sign::Pos* v){
    try{
      unsigned char* buff=dsrl.getSyncWriteBuffer();
      int c=0;
      for (size_t j=0;j<lastSent.size();j++){
	if (b[j]){
	  buff[c*3]=(unsigned char)idMap.rawId[j];
	  toLE(buff+c*3+1,v[j]);
	  c++;
	}
      }
      dsrl.syncWrite(dynamixel::TARGETPOS,c,2);      
      for (size_t j=0;j<lastSent.size();j++){
	if (b[j]){
	  lastSent[j]=v[j];
	}
      }
    }catch(exc::Exception& e){
      HwIdList sent;
      for (size_t j=0;j<lastSent.size();j++){
	if (b[j]){
	  manageError(j);
	  sent.ids.push_back(j);
	}
      }
      throw exc::MultipleMotorException<UNREACHABLEHWSTATE>(sent,e.what());;
    }    
  }//hwRotate bool* int*
  
  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::rawRotate(int j,typename Sign::Pos pos){       
    //std::cout<<"rawRotate pos"<<pos<<std::endl;
    typename Sign::Pos lp=limiter.limit(pos,j);    
    if (lp==lastSent[j]) return;
    //std::cout<<"rawRotate limpos"<<lp<<std::endl;
    hwRotate(j,lp);
  }
  
  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::rawRotate(const typename Sign::Pos* v){
    for (size_t j=0;j<lastSent.size();j++){
      limitedPos[j]=limiter.limit(v[j],j);
      toRotate[j]=(limitedPos[j]!=lastSent[j]);
    }
    hwRotate(toRotate.data,limitedPos.data);
  }


  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::rawRotate(const bool* b,const typename Sign::Pos* v){
    setAll(toRotate,false);
    for (size_t j=0;j<lastSent.size();j++){
      if (b[j]){
	limitedPos[j]=limiter.limit(v[j],j);
	toRotate[j]=(limitedPos[j]!=lastSent[j]);	
      }
    }//for
    hwRotate(toRotate.data,limitedPos.data);
  }
  
  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::rotate(int j, typename motor::Interface::Position p){

    //std::cout<<"rotate "<<j<<" p deg"<<p*180/M_PI<<std::endl;
    rawRotate(j,psNorm.unnormPos(p,j));
  }
  
  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::rotate(const motor::Interface::Position * v){
    for (size_t j=0;j<lastSent.size();j++){
      limitedPos[j]=psNorm.unnormPos(v[j],j);
    }
    rawRotate(limitedPos.data);
  }

  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::rotate(const bool* b,const motor::Interface::Position * v){
    for (size_t j=0;j<idMap.rawId.size();j++){
      if (b[j]){
	limitedPos[j]=psNorm.unnormPos(v[j],j);
      }
    }
    rawRotate(b,limitedPos.data);
  }


  template<typename Dsrl,typename Sign>
  motor::IfaceId DynamixelMotor<Dsrl,Sign>::getIfaceId(){
    return "dynamixel";
  }

 

  
  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::connect(ConnReference<typename motor::Interface::Position>&, int){
    throw exc::InvalidOperation("DynamixelServo disables connection for efficiency");
  }
  
  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::disconnect(ConnReference<typename motor::Interface::Position>&,int j){
    throw exc::InvalidOperation("DynamixelServo disables connection for efficiency");
  }

  
  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::rawConnect(ConnReference<typename Sign::Pos>& conn,int j){
    throw exc::InvalidOperation("DynamixelServo disables connection for efficiency");
  }
  

  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::rawDisconnect(ConnReference<typename Sign::Pos>& conn,int j){
    throw exc::InvalidOperation("DynamixelServo disables connection for efficiency");
  }
  

  template<typename Dsrl,typename Sign>
  typename Sign::Pos DynamixelMotor<Dsrl,Sign>::rawGetPosition(int j){
    //std::cout<<"asked position:"<<j<<std::endl;
    if (lastSent[j]!=Sign::INVALIDPOS){
      return lastSent[j];
    }
    try{      
      typename Sign::Pos p=dsrl.read<2>(idMap.rawId[j],dynamixel::TARGETPOS);
      lastSent[j]=p;
      return p;
    }catch (exc::Exception& e){
      throwError(j,e);
      throw;//compiler warning function not returning
    }
  }

  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::rawGetPosition(typename Sign::Pos* p){
    for (size_t j=0;j<lastSent.size();j++){
      try{
	p[j]=rawGetPosition(j);
      }catch(exc::Exception& e){
	meh.handle(e);
      }
    }
    meh.check();
  }
  
  template<typename Dsrl,typename Sign>
  typename motor::Interface::Position DynamixelMotor<Dsrl,Sign>::getPosition(int j){
    return psNorm.normPos(rawGetPosition(j),j);
  }


  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::getPosition(motor::Interface::Position* p){
    rawGetPosition(limitedPos.data);
    for (size_t j=0;j<lastSent.size();j++){
      p[j]=psNorm.normPos(limitedPos[j],j);
    }
  }

  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList &l) const{
    tk.resize(l.ids.size());
    for(size_t i=0;i<l.ids.size();i++){
      tk[i].reset(new DeletableData<int>(l.ids[i]));
    }
  }

  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::isWorking(Callback<const HwStateNotification&>* cb,const HwInterface::WorkingToken& tk){
    int id=trust_cast<const DeletableData<int>*>(&tk)->data;
    (*cb)(std::make_pair(id,isWorking(id)));
  }

  
  template<typename Dsrl,typename Sign>
  HwState DynamixelMotor<Dsrl,Sign>::isWorking(int j){
    try{
      dsrl.ping(idMap.rawId[j]);
      return WORKINGHWSTATE;
    }catch(exc::Exception& ){
      return UNREACHABLEHWSTATE;
    }  
  }
  template<typename Dsrl,typename Sign>
  void DynamixelMotor<Dsrl,Sign>::isWorking(Array<HwState>& s,const HwIdList& l){
    //it makes no sense to try to read all at once, if something times out we need to check individually anyway
    for (size_t i=0;i<l.ids.size();i++){
      s[i]=isWorking(l.ids[i]);
    }    
  }

  template<typename Dsrl,typename Sign>
  int DynamixelMotor<Dsrl,Sign>::getMotorNum(){
    return lastSent.size();
  }

  template<typename Dsrl,typename Sign>
  DynamixelMotor<Dsrl,Sign>::DynamixelMotor(Dsrl& pdsrl,IdMapper& pidMap,motor::Calib<Sign>& calib):
    dsrl(pdsrl),idMap(pidMap),psNorm(calib),prNorm(psNorm),limiter(calib),lastSent(calib.data.size()),
    toRotate(calib.data.size()),limitedPos(calib.data.size()){
    setAll(lastSent,Sign::INVALIDPOS);
  }
    

}//ns helium


