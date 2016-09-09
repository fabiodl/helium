/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2015.05.20
 * \version 0.1
 * Release_flags s
 */


#include <helium/math/crc.h>
#include <helium/vector/operations.h>
#include <helium/robot/motorException.h>
#include <helium/util/byteConversion.h>
#include <helium/android/androidMotor.h>
#include <helium/util/printHex.h>

#include <helium/os/time.h>


/***
    received data
    00   01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27  
   
    5A   B9   00   01  [00] [-1] [03] [12] [13] [-14] XX   7F   **   **   5A   B9   01   00   7F   7E   7E   7E   7F   7E   7E   7E   0A   A0

    XX is 7f or 80
    **  is the checksum
    [n] is joint #n


data sent are

bigPacket[n+ROTATEPACKETOVERHEAD-1]=0x5A; //replace the terminator with a new start marker
bigPacket[n+ROTATEPACKETOVERHEAD+0]=0x39;
bigPacket[n+ROTATEPACKETOVERHEAD+1]=0x00;
bigPacket[n+ROTATEPACKETOVERHEAD+2]=0x02;
bigPacket[n+ROTATEPACKETOVERHEAD+3]=0x51;
bigPacket[n+ROTATEPACKETOVERHEAD+4]=0xCC;
bigPacket[n+ROTATEPACKETOVERHEAD+5]=0xA5;

note:if you make a read packet it takes some time to be processed
 ***/




namespace helium{


  static const int ROTATEPACKETOVERHEAD=6;
  static const int ROTATEPACKETVALUEPOS=3;

  static const uint8_t HEADERAXIS = 0x5A; // header identifier
  static const  uint8_t  COMMAND =0x30;
  static const uint8_t FOOTER = 0xA5;
  

  static const int NUMRETRIES=1;

  
  using namespace android;

  AndroidMotor::PropertyNormalizer:: PropertyNormalizer(motor::Calib<MotorSign>& c):
    LinearPosNormalizer<android::MotorSign>(c)
  {
    
  }


  motor::Interface::Property AndroidMotor::PropertyNormalizer::normProp(motor::Raw<MotorSign>::Property value,const motor::PropertyRequest& r){
    switch (r.id){
    case motor::TARGETPOSITION:
    case motor::POTENTIOPOSITION:
      return this->normPos(value,r.joint);
    case motor::POWER:
    case motor::TEMP:       
      return value;	
    default:
      throw exc::InvalidOperation("Conversion for property "+toString(r.id)+" not available");
    }           
  }

  typename motor::Raw<MotorSign>::Property AndroidMotor::PropertyNormalizer::unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r){
    switch (r.id){
    case motor::TARGETPOSITION:
    case motor::POTENTIOPOSITION:
      return this->unnormPos(value,r.joint);
    case motor::POWER:
    case motor::TEMP:       
      return value;	
    default:
      throw exc::InvalidOperation("Conversion to raw for property "+toString(r.id)+" not available");
    }       
  }
  
  typename motor::Raw<MotorSign>::PropNormalizer& AndroidMotor::convProp(){
    return pnorm;
  }

  typename motor::Raw<MotorSign>::PosNormalizer& AndroidMotor::convPos(){
    return pnorm;
  }

  /*
  bool c(unsigned char* buff,int idx,unsigned char val){
    if (buff[idx]!=val){
      std::cout<<std::dec<<idx<<" is not "<<std::hex<<(int)val<<" but "<<(int)buff[idx]<<std::dec<<std::endl;
    }
  }


  //5a b9 0 1 [0] [-1] [3] [12] [13] [-14] 7f 7f crc crc

  unsigned char tail[14]={0x5A,0xB9,0x01,0x00,0x7F,0x7E,0x7E,0x7E,0x7F,0x7E,0x7E,0x7E,0x0A,0xA0};
  
class SerReader:public Thread{
  Serial& s;
  
public:
  void run(){
    unsigned char b[256];


    while(true){
      int n=s.recv(b,256);
      if (n!=28) continue;
      //printHex(b,n);      
      //      std::cout<<std::hex<<"{"<<crc16ibm::getCrc(buffer+1,11)<<"}"<<std::endl;

      //      printHex(buffer+2,10);
      //std::cout<<std::endl;
      c(b,0,0x5A);
      c(b,1,0xB9);
      c(b,2,0x00);
      c(b,3,0x01);

      c(b,10,0x7F);
      c(b,11,0x7F);
      c(b,14,0x5A);
      c(b,15,0xB9);
      for (int i=14;i<28;i++){
	if (tail[i-14]!=b[i]) std::cout<<"i="<<i<<std::hex<<":"<<(int)b[i]<<"vs"<<(int)tail[i-14]<<std::dec<<std::endl;
      }

    }
  }
  SerReader(  Serial& _s):s(_s){
    start();
  }
};

  */

  AndroidMotor::AndroidMotor(Serial& _srl,IdMapper& _idMap,motor::Calib<android::MotorSign>& _calib):
    srl(_srl),idMap(_idMap),pnorm(_calib),limiter(_calib),
    lastSent(_calib.data.size()),
    rotatePacket(_calib.data.size()+ROTATEPACKETOVERHEAD),
    onlyOneSelector(_calib.data.size()),
    onlyOneValue(_calib.data.size()),
    limitedPos(_calib.data.size()),
    globalPowerState(true)
  {
    //new SerReader(srl);
    for (size_t j=0;j<lastSent.size();++j){
      lastSent[j]=pnorm.unnormPos(0,j);
      //std::cout<<"init "<<j<<"@"<<lastSent[j]<<std::endl;
    }
    //setAll(lastSent,Sign::INVALIDPOS);
    rotatePacket[0]=HEADERAXIS;
    rotatePacket[1]=COMMAND;
    rotatePacket[2]=lastSent.size();    
    //here goes the positions
    int n=lastSent.size();
    for (int j=0;j<n;j++){
      rotatePacket[ROTATEPACKETVALUEPOS+idMap.rawId[j]]=pnorm.unnormPos(0,j);
    }
    uint16_t crc=crc16ibm::getCrc(rotatePacket+1,n+2);
    //here goes the crc
    rotatePacket[n+3]=crc&0xFF;
    rotatePacket[n+4]=(crc>>8)&0xFF;
    //terminator
    rotatePacket[lastSent.size()+5]=FOOTER;

  }

  
  AndroidMotor::RawPosition AndroidMotor::getPotentiometer(int joint,int checks){
    //std::cout<<"@getpotentiomenter ";printHex(lastSent);
    return lastSent[joint];
  }


  void AndroidMotor::rawGetProperty(RawProperty& o, const motor::PropertyRequest & r){
    switch(r.id){

    case motor::TARGETPOSITION:
      o=rawGetPosition(r.joint);
      return;

    case motor::POTENTIOPOSITION:
      o=getPotentiometer(r.joint,NUMRETRIES);
      return;
      
    case motor::POWER:
      o=getPowerState(r.joint);
      return;      
    case motor::TEMP:
    case motor::RAWPROPERTIES:    
      o=Sign::INVALIDPROP;
      return;        
    case motor::PROPERTIESNUM:
      return;
    }    
  }
  
  void AndroidMotor::rawSetProperty(const RawProperty&  o, const motor::PropertyRequest & r){
    switch(r.id){
    case motor::TARGETPOSITION:
      rotate(r.joint,o);
      return;
    case motor::POWER:
      setPowerState(r.joint,o);
      return; 
    default:
      throw exc::InvalidOperation("Unable to set property "+toString(r.id)+" for android motor");
      }
    }  


  
  void AndroidMotor::rawGetGetterToken(typename AndroidMotor::Token& tk,const PropertyList &info){
    tk.reset(new MotorGetterToken<Sign>(info,pnorm)  );
  }


  void AndroidMotor::rawGetOptimizedSplit(Array<Token>& tk,const PropertyList &info){
    tk.resize(1);
    rawGetGetterToken(tk[0],info);//all at once
  }

  void AndroidMotor::rawGetProperty(const Token& tk){
    MotorGetterToken<AndroidMotor::Sign>* gk=static_cast< MotorGetterToken<AndroidMotor::Sign>* >(tk.get());
    
    for (size_t i=0;i<gk->props.size();i++){
      try{
	rawGetProperty(gk->props[i].first,*gk->props[i].second); //this already throws exceptions of motor type
	gk->notify(i);
      }catch(exc::Exception& e){
	//manageError(i);
	meh.handle(e);
      }
    }//for i
    meh.check();    
  }
  
  void AndroidMotor::setPowerState(int j,bool state){
    if (!state){    
      uint8_t offPacket[5]={0x5A,0x15,0xC1,0xCF,0xA5};      
      srl.send(offPacket,5);
    }else{
      srl.send(rotatePacket,lastSent.size()+ROTATEPACKETOVERHEAD);     
    }
    globalPowerState=state;
  }

  bool AndroidMotor::getPowerState(int j){
    return globalPowerState;
  }


  void AndroidMotor::getPowerState(bool* b){
    setAlln(b,true,lastSent.size());
  }
  
  

  
  

  void AndroidMotor::hwRotate(const bool * b,const AndroidMotor::RawPosition* p){
    int n=lastSent.size();
    for (int j=0;j<n;j++){
      int idx=ROTATEPACKETVALUEPOS+idMap.rawId[j];
      if (b[j]){
	rotatePacket[idx]=p[j];
      }else{
	rotatePacket[idx]=(lastSent[j]==Sign::INVALIDPOS)?pnorm.unnormPos(0,j):lastSent[j];
      }

    }
    uint16_t crc=crc16ibm::getCrc(rotatePacket+1,n+2);
    rotatePacket[n+3]=crc&0xFF;
    rotatePacket[n+4]=(crc>>8)&0xFF;


    srl.send(rotatePacket,n+ROTATEPACKETOVERHEAD);

    for (int j=0;j<n;j++){
      lastSent[j]=rotatePacket[ROTATEPACKETVALUEPOS+idMap.rawId[j]];
    }
    
  }


  
  void AndroidMotor::hwRotate(int j, AndroidMotor::RawPosition p){
    onlyOneSelector[j]=true;
    onlyOneValue[j]=p;
    hwRotate(onlyOneSelector,onlyOneValue);
    onlyOneSelector[j]=false;    
  }



  void AndroidMotor::hwRotate(const AndroidMotor::RawPosition* p){
    int n=lastSent.size();
    for (size_t j=0;j<lastSent.size();j++){
      rotatePacket[ROTATEPACKETVALUEPOS+idMap.rawId[j]]=p[j];
    }
    uint16_t crc=crc16ibm::getCrc(rotatePacket+1,n+2);
    rotatePacket[n+3]=crc&0xFF;
    rotatePacket[n+4]=(crc>>8)&0xFF;
    srl.send(rotatePacket,n+ROTATEPACKETOVERHEAD);   
    assign(lastSent,p);
  }
  

  void AndroidMotor::rawRotate(int j, AndroidMotor::RawPosition pos){
    AndroidMotor::RawPosition lp=limiter.limit(pos,j);
    if (lp==lastSent[j]) return;
    hwRotate(j,lp);
  }
  
  void AndroidMotor::rawRotate(const RawPosition* v){
    bool toRotate=false;
    for (size_t j=0;j<lastSent.size();j++){
      AndroidMotor::RawPosition  lp=limiter.limit(v[j],j);
      toRotate|=(lp!=lastSent[j]);
      limitedPos[j]=lp;
    }
    if (toRotate) hwRotate(limitedPos);    
  }

  void AndroidMotor::rawRotate(const bool* b,const RawPosition* v){
    bool toRotate=false;
    for (size_t j=0;j<lastSent.size();j++){
      if (b[j]){
	AndroidMotor::RawPosition  lp=limiter.limit(v[j],j);
	toRotate|=(lp!=lastSent[j]);
	limitedPos[j]=lp;
      }
    }
    if (toRotate) hwRotate(b,limitedPos);        
  }
  
  static const char* NOTIMPLEMENTED="AndroidMotor disables connection for efficiency";

  void AndroidMotor::connect(ConnReference<NormPosition>&, int){
    throw exc::InvalidOperation(NOTIMPLEMENTED);
  }
  void AndroidMotor::disconnect(ConnReference<NormPosition>&,int j){
    throw exc::InvalidOperation(NOTIMPLEMENTED);
  }
  void AndroidMotor::rawConnect(ConnReference<RawPosition>& conn,int j){
    throw exc::InvalidOperation(NOTIMPLEMENTED);
  }
  void AndroidMotor::rawDisconnect(ConnReference<RawPosition>& conn,int j){
    throw exc::InvalidOperation(NOTIMPLEMENTED);
  }
  
  AndroidMotor::RawPosition  AndroidMotor::rawGetPosition(int j){
    if (lastSent[j]==Sign::INVALIDPOS){
      return getPotentiometer(j,NUMRETRIES);
    }else{
      return lastSent[j];
    }
  }
  
 void AndroidMotor::rawGetPosition(RawPosition* p){
    for (size_t j=0;j<lastSent.size();j++){
      p[j]=rawGetPosition(j);
    }
 }


  void AndroidMotor::rotate(int j, NormPosition p){
    //    std::cout<<"rotating iface "<<p<<std::endl;
    rawRotate(j,pnorm.unnormPos(p,j));
  }
  
  void AndroidMotor::rotate(const NormPosition* v){
    for (size_t j=0;j<lastSent.size();j++){
      limitedPos[j]=pnorm.unnormPos(v[j],j);
    }
    rawRotate(limitedPos);
  }

  void AndroidMotor::rotate(const bool* b,const NormPosition* v){
    for (size_t j=0;j<lastSent.size();j++){
      if (b[j]){
	limitedPos[j]=pnorm.unnormPos(v[j],j);
      }
    }
    rawRotate(b,limitedPos);
  }

  

  AndroidMotor::NormPosition AndroidMotor::getPosition(int j){
    return pnorm.normPos(rawGetPosition(j),j);
  }

  void AndroidMotor::getPosition(NormPosition* p){
    rawGetPosition(limitedPos);
    
    for (size_t j=0;j<lastSent.size();j++){
      p[j]=pnorm.normPos(limitedPos[j],j);
    }
  }

 

  motor::IfaceId  AndroidMotor::getIfaceId(){
    return "android ibm crc";
  }
  
  HwState  AndroidMotor::isWorking(int id){
    return WORKINGHWSTATE;
  }

  void AndroidMotor::isWorking(Array<HwState>& state,const HwIdList& l){
    for (size_t i=0;i<l.ids.size();i++){
      state[i]=isWorking(l.ids[i]);
    }
  }

  void AndroidMotor::getWorkingOptimizedSplit(Array <HwInterface::WorkingToken>& tk,const HwIdList &l) const{
    tk.resize(1);
    tk[0].reset(new DeletableData<HwIdList>(l));
  }
  
  
    
  void AndroidMotor::isWorking(Callback<const HwStateNotification&>* cb,const HwInterface::WorkingToken& tk){
    HwIdList& l=static_cast<DeletableData<HwIdList>*>(tk.get())->data;
    for (size_t i=0;i<l.ids.size();++i){
      int idx=l.ids[i];
      (*cb)(HwStateNotification(idx,isWorking(idx)));
    }    
  }

  int AndroidMotor::getMotorNum(){
    return lastSent.size();
  }


}//ns helium


