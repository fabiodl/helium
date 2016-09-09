/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#include <helium/vstone/vServo.h>
#include <helium/util/mout.h>
#include <helium/math/interpolation.h>
#include <helium/robot/motorException.h>
#include <helium/math/tAbs.h>
#include <stdio.h>
namespace helium{

char Vservo::propSize[]={

  2, //0x00
  0, //0x01
  2, //0x02
  0,//0x03

  4, //0x04
  0,//0x05
  0,//0x06
  0,//0x07

  1,//0x08
  1,//0x09
  2,//0x0a
  0,//0x0b

  2,//0x0c
  0,//0x0d
  0,//0x0e
  0,//0x0f

  1,//0x10
  1,//0x11
  1,//0x12
  0,//0x13

  1,//0x14
  0,//0x15
  0,//0x16
  0,//0x17

  1,//0x18
  1,//0x19
  2,//0x1a
  0,//0x1b

  2,//0x1c
  0,//0x1d
  0,//0x1e
  0,//0x1f
  
  2,//0x20
  0,//0x21
  2,//0x22
  0,//0x23

  2,//0x24
  0,//0x25
  2,//0x26
  0,//0x27

  2,//0x28
  0,//0x29
  2,//0x2a
  0,//0x2b

  0,//0x2c
  0,//0x2d
  0,//0x2e
  0,//0x2f

  2,//0x30
  0,//0x31
  1,//0x32
  1,//0x33

  1,//0x34
  1,//0x35
  2,//0x36
  0,//0x37

  1,//0x38
  1,//0x39
  1,//0x3a
  1,//0x3b

  1,//0x3c
  1,//0x3d
  2,//0x3e
  0,//0x3f

  2,//0x40
  0,//0x41
  2,//0x42
  0,//0x43

  2,//0x44
  0,//0x45
  2,//0x46
  0,//0x47

  0,//0x48
  0,//0x49
  0,//0x4a
  0,//0x4b

  0,//0x4c
  1,//0x4d
  1,//0x4e
  1,//0x4f

  1,//0x50
  1,//0x51
  1,//0x52
  1,//0x53

  1,//0x54
  1,//0x55
  1,//0x56
  1,//0x57

  1,//0x58
  1,//0x59
  1,//0x5a
  1,//0x5b

  1,//0x5c
  1,//0x5d
  1,//0x5e
  1,//0x5f
  -1  //0x60

};

 
 static const int TEMPTABLESIZE=21;
  static const double MINTEMP=20;
  static const double TEMPDELTA=5;
  static const int tempTable[TEMPTABLESIZE]={0x073a,
				  0x0800,
				  0x08be,
				  0x0974,
				  0x0a1e,
				  0x0abc,
				  0x0b4c,
				  0x0bcf,
				  0x0c45,
				  0x0cae,
				  0x0d0c,
				  0x0d5f,
				  0x0da9,
				  0x0dea,
				  0x0e24,
				  0x0e56,
				  0x0e83,
				  0x0eaa,
				  0x0ecd,
				  0x0eec,
				  0x0f07};

static double getTemperature(int val){
  if (val>=tempTable[TEMPTABLESIZE-1]) return MINTEMP+(TEMPTABLESIZE-1)*TEMPDELTA;
  int i=0;
  for (;tempTable[i+1]<val;i++); 
  return linearInterpolation(tempTable[i],MINTEMP+TEMPDELTA*i,tempTable[i+1],MINTEMP+TEMPDELTA*(i+1),val);
}

  static int getAddr(const motor::PropertyRequest& r){    
    switch(r.id){
    case motor::TARGETPOSITION:
      return Vservo::FB_TPOS;
    case motor::POTENTIOPOSITION:
      return Vservo::M_POS;
    case motor::POWER:
      return Vservo::PWM_OFF;
    case motor::TEMP:
      return Vservo::M_TEMP;
    case motor::RAWPROPERTIES:
      return r.idx;
    default:
      throw exc::InvalidOperation("Vservo does not support property "+toString(r.id));
    }
	
  }



  motor::Interface::Property Vservo::PropNormalize::normProp(int value,const motor::PropertyRequest& r){
    int addr=getAddr(r);
    switch (addr){
    case M_TEMP:
      return getTemperature(value);
    case PWM_OFF:
      return value;
    case FB_TPOS:
    case M_POS:
      return posNorm.normPos(value,r.joint);
    default:
      throw exc::InvalidOperation("Conversion for property "+toString(addr)+" not available");
    } 

  }//normProp

  motor::Raw<VServoSign>::Property Vservo::PropNormalize::unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r){
    throw exc::InvalidOperation("Conversion to raw for property "+toString(r)+" not available");
  }
  




typedef int RawProperty;
typedef int RawPosition;
  
motor::Raw<VServoSign>::PropNormalizer& Vservo::convProp(){
  return prNorm;
}

motor::Raw<VServoSign>::PosNormalizer& Vservo::convPos(){
  return psNorm;
}

  motor::Raw<VServoSign>::Position Vservo::getPotentiometer(int joint,int checks){
  try{
    int c=0;
    int v=serial.read2(idMap.rawId[joint],M_POS);
    while(c<checks){
      int nv=serial.read2(idMap.rawId[joint],M_POS);
      if (tAbs(nv-v)<VServoSign::TORAW*3*M_PI/180){
	c++;
      }else{
	c=0;
      }
      v=nv;
    }
    //std::cout<<"potentiometer read, "<<v<<std::endl;
    return v;
  }catch(exc::Exception& e){
    throw exc::SingleMotorException<UNREACHABLEHWSTATE>(joint,toString(e));
  }
}

 

  void Vservo::rawGetProperty(RawProperty& o, const motor::PropertyRequest & r){
    try{
      int addr=getAddr(r);
      //std::cout<<"reading addr"<<addr<<" of motor "<<r.joint<<std::endl;
      switch(propSize[addr]){
      case 1:
	o=serial.read1(idMap.rawId[r.joint],addr);
	break;
      case 2:
	o=serial.read2(idMap.rawId[r.joint],addr);
	break;
      case 4:
	o=serial.read4(idMap.rawId[r.joint],addr);
	break;
      case -1:
	std::cout<<"TEST"<<std::endl;

	for (int i=0;i<6;i++){
	  unsigned char buff[33];
	  memset(buff,0,sizeof(char)*16);
	  serial.recv(idMap.rawId[r.joint],i*16,buff,16);
	  for (int j=0;j<16;j++){
	    printf("%02x:%02x ",i*16+j,buff[j]);
	  }
	  std::cout<<std::dec<<std::endl;
	}
	break;
      default:
	throw exc::InvalidOperation("Vservo:Unknown size for data at address "+toString(addr));
      }
    }catch(exc::Exception& e){
      throw exc::SingleMotorException<UNREACHABLEHWSTATE>(r.joint,e.what());
    }
}



void Vservo::rawSetProperty(const RawProperty& v, const motor::PropertyRequest & r){
  try{
    int addr=getAddr(r);
    switch(propSize[addr]){
    case 1:
      serial.write1(idMap.rawId[r.joint],addr,v);
      break;
    case 2:
      serial.write2(idMap.rawId[r.joint],addr,v);
      break;
    case 4:
      serial.write4(idMap.rawId[r.joint],addr,v);
      break;
    default:
      throw exc::InvalidOperation("Vservo:Unknown size for data at address "+toString(addr));
    }
  }catch(exc::Exception& e){
    throw exc::SingleMotorException<UNREACHABLEHWSTATE>(r.joint,e.what());
  }
  
}

typedef motor::Raw<VServoSign>::PropertyList::const_iterator IT;

struct GetterToken:public Deletable{

  typedef motor::Raw<VServoSign>::PropertyNotify PropertyNotify;
  Array<PropertyNotify> props;
  Array<Callback<PropertyNotify*>*> to;

  void notify(int i){
    (*to[i])(&props[i]);
  }

  GetterToken(const motor::Raw<VServoSign>::PropertyList &info,motor::Raw<VServoSign>::PropNormalizer& pn):
    props(info.size()),to(info.size()){
    //std::cout<<"|created "<<this<<std::endl;
    int i=0;
    for (IT it=info.begin();it!=info.end();++it){
      //props[i].first
      props[i].second=it->second;
      props[i].third=&pn;
      to[i]=it->first;
      i++;
    }
  }

  ~GetterToken(){
    //std::cout<<"|deleted "<<this<<std::endl;
  }
  
};
				  

void Vservo::rawGetGetterToken(Token& tk,const PropertyList &info){
  tk=new GetterToken(info,convProp());  
}

void Vservo::rawGetOptimizedSplit(Array <Token>& tk,const motor::Raw<VServoSign>::PropertyList &info){
  std::map<int,motor::Raw<VServoSign>::PropertyList> lists;
  for (IT it=info.begin();it!=info.end();++it){
    lists[it->second->joint].push_back(*it);
  }
  typedef std::map<int,motor::Raw<VServoSign>::PropertyList>::iterator  Mit;
  tk.resize(lists.size());
  int i=0;
  for (Mit mit=lists.begin();mit!=lists.end();++mit){
    tk[i]= new GetterToken(mit->second,convProp());
    i++;
  }
}

void Vservo::rawGetProperty(const Token& tk){
  GetterToken* gk=const_cast<GetterToken*>(trust_cast<const GetterToken*>(&tk));
  //std::cout<<"|using "<<gk<<std::endl;
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


HwState Vservo::isWorking(int j){
  motor::PropertyRequest testProperty(j,motor::RAWPROPERTIES,Vservo::SYS_SID);
  try{
    int val;
    rawGetProperty(val,testProperty);    
    //std::cout<<"expected "<<idMap.rawId[j]<<" got "<<val<<std::endl;
    return (val==idMap.rawId[j]?WORKINGHWSTATE:FAULTYVALUEHWSTATE);
  }catch(exc::Exception& ){
    return UNREACHABLEHWSTATE;
  }  
}

  void Vservo::isWorking(Array<HwState>& s,const HwIdList& l){
    for (size_t i=0;i<l.ids.size();i++){
      s[i]=isWorking(l.ids[i]);
    }
  }

  
  struct IdToken:public Deletable{
    int id;
    IdToken(int pid):id(pid){
    }
  };

  void Vservo::getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList & l) const{
    tk.resize(l.ids.size());
    for(size_t i=0;i<l.ids.size();i++){
      tk[i]=new IdToken(l.ids[i]);
    }
  }

  void Vservo::isWorking(Callback<const HwStateNotification&>* cb,const WorkingToken& tk){
    int id=trust_cast<const IdToken*>(&tk)->id;
    (*cb)(std::make_pair(id,isWorking(id)));
  }
  

void Vservo::setPowerState(int j,bool state){
  try{
    if (!state){
      lastSent[j]=INVALIDPOS;      
    }else{
      int firm=serial.read4(idMap.rawId[j],SYS_UID);
      switch(firm){
      case 0x1234567:
	//std::cout<<"motor "<<j<<" old firmware"<<std::endl;
	break;
      case 0x7654321:
	//std::cout<<"motor "<<j<<" new firmware"<<std::endl;	
	unlock(j);
	break;
      case 0xFAB10:
	//std::cout<<"motor "<<j<<" safe firmware"<<std::endl;	
	unlock(j);
	serial.write2(idMap.rawId[j],CALIBMIN,limiter.getMin(j));
	serial.write2(idMap.rawId[j],CALIBMAX,limiter.getMax(j));
	if (
	    serial.read2(idMap.rawId[j],CALIBMIN)!=limiter.getMin(j)
	    ||
	    serial.read2(idMap.rawId[j],CALIBMAX)!=limiter.getMax(j)
	    ){
	  throw exc::SingleMotorException<UNREACHABLEHWSTATE>(j,"motor cannot be set its limits");
	}		
	break;
      default:
	mout(msg::WARNING)(stringBegin()<<"motor "<<j<<" has an unknown firmware:"<<firm<<stringEnd());

      }
    }
    //DEBUG
    //return;
    
    serial.write1(idMap.rawId[j],PWM_OFF,state?1:0);    
  }catch(exc::Exception& e){
    throw exc::SingleMotorException<UNREACHABLEHWSTATE>(j,toString(e));
  }
}



bool Vservo::getPowerState(int j){
  try{
    return serial.read1(idMap.rawId[j],PWM_OFF)?true:false;
  }catch(exc::Exception& e){
    throw exc::SingleMotorException<UNREACHABLEHWSTATE>(j,toString(e));
  }
}

  void Vservo::getPowerState(bool* b){
    for (size_t j=0;j<lastSent.size();j++){
      try{
	b[j]=getPowerState(j);
      }catch(exc::Exception& e){
	meh.handle(e);
      }
    }
    meh.check();    
  }

void Vservo:: hwRotate(int j,motor::Raw<VServoSign>::Position pos){
  try{
    serial.write2(idMap.rawId[j],FB_TPOS,pos);
    lastSent[j]=pos;

    //std::cout<<"R "<<j<<" "<<pos<<std::endl;
  }catch(exc::Exception& e){
    throw exc::SingleMotorException<UNREACHABLEHWSTATE>(j,toString(e));
  }
}

  void Vservo::hwRotate(const motor::Raw<VServoSign>::Position* v){    
    for (size_t i=0;i<lastSent.size();i++){
      try{
	hwRotate(i,v[i]);
      }catch(exc::Exception& e){
	meh.handle(e);
      }
    }
    meh.check();
  }

  void Vservo::hwRotate(const bool* b,const motor::Raw<VServoSign>::Position* v){
    for (size_t i=0;i<lastSent.size();i++){
      try{
	if (b[i]){	
	  hwRotate(i,v[i]);
	}
      }catch(exc::Exception& e){
	meh.handle(e);
      }
    }
    meh.check();
  }


  
  void Vservo:: rawRotate(int j,motor::Raw<VServoSign>::Position pos){
    //std::cout<<"limit"<<pos<<"@"<<j<<std::endl;
    hwRotate(j,limiter.limit(pos,j));
  }

  void Vservo::rawRotate(const motor::Raw<VServoSign>::Position* v){
    for (size_t i=0;i<lastSent.size();i++){
      try{
	rawRotate(i,v[i]);
      }catch(exc::Exception& e){
	meh.handle(e);
      }
    }
    meh.check();
  }
  
  void Vservo::rawRotate(const bool* b,const motor::Raw<VServoSign>::Position* v){
    for (size_t i=0;i<lastSent.size();i++){
      try{
	if (b[i]){
	  rawRotate(i,v[i]);
	}//if
      }catch(exc::Exception& e){
	meh.handle(e);
      }
    }//for
  }
  
  


  
void Vservo::connect(ConnReference<NormPosition>&, int){
  throw exc::InvalidOperation("Vservo disables connection for efficiency");
}
void Vservo::disconnect(ConnReference<NormPosition>&,int j){
  throw exc::InvalidOperation("Vservo disables connection for efficiency");
}
void Vservo::rawConnect(ConnReference<RawPosition>& conn,int j){
  throw exc::InvalidOperation("Vservo disables connection for efficiency");
}
void Vservo::rawDisconnect(ConnReference<RawPosition>& conn,int j){
  throw exc::InvalidOperation("Vservo disables connection for efficiency");
}


  motor::Interface::Position Vservo::getPosition(int j){
    return psNorm.normPos(rawGetPosition(j),j);
  }

  void Vservo::getPosition(motor::Interface::Position* p){
    for (size_t j=0;j<lastSent.size();j++){
      try{
	p[j]=psNorm.normPos(rawGetPosition(j),j);
      }catch(exc::Exception& e){
	meh.handle(e);
      }
    }
    meh.check();
  }


  motor::Raw<VServoSign>::Position Vservo::rawGetPosition(int j){
    if (lastSent[j]!=INVALIDPOS)
      return lastSent[j];
    try{
      return serial.read2(idMap.rawId[j],FB_TPOS);
    }catch (exc::Exception& e){
      throw exc::SingleMotorException<UNREACHABLEHWSTATE>(j,toString(e));
    }
  }

  void Vservo::rawGetPosition( motor::Raw<VServoSign>::Position* p){
    for (size_t i=0;i<lastSent.size();i++){
      try{
	p[i]=rawGetPosition(i);
      }catch(exc::Exception& e){
	meh.handle(e);
      }
    }
    meh.check();
  }


void Vservo::rotate(int j, motor::Interface::Position p){
  //std::cout<<"rotate "<<p<<"@"<<j<<std::endl;
  rawRotate(j,psNorm.unnormPos(p,j));
}

void Vservo::rotate(const motor::Interface::Position * v){
  for (size_t i=0;i<idMap.rawId.size();i++){
    try{
      rotate(i,v[i]);
    }catch(exc::Exception& e){
	meh.handle(e);
    }
  }
  meh.check();
}

  void Vservo::rotate(const bool* b,const motor::Interface::Position * v){
    for (size_t i=0;i<idMap.rawId.size();i++){
      try{
	if (b[i]){
	  rotate(i,v[i]);
	}
      }catch(exc::Exception& e){
	meh.handle(e);
      }
    }
    meh.check();
  }
  

  motor::IfaceId Vservo::getIfaceId(){
    return "vservo";
  }

  void Vservo::unlock(int id){
    serial.unlock(idMap.rawId[id]);
  }


  int Vservo::getMotorNum(){
    return lastSent.size();
  }

  Vservo::Vservo(VstoneSerial& s, IdMapper& pidMap,Vservo::VservoCalib& calib):
    serial(s),idMap(pidMap),psNorm(calib),prNorm(psNorm),limiter(calib),lastSent(calib.data.size())
  {
    setAll(lastSent,INVALIDPOS);
    for (size_t j=0;j<calib.data.size();j++){
      unlock(j);
    }
  }





}//helium
