/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/vstone/usbVsRc003.h>
#include <helium/robot/deletableList.h>
#include <helium/vector/operations.h>

#include <fstream>

typedef unsigned char RawPacket [64];
static const RawPacket SynchyHeader[4]={
  {
0x55,0x01,0xe0,0x00,0x80,0x04,0x00,0x00,0x00,0x00,0x80,0x00,0x04,0x00,0x00,0x00,0x00,0x80,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x02,0x0e,0x04,0xc0,0x08,0xe0,0x10,0xe0,0x00,0x3c,0x01,0x01,0x01,0x80,0x00,0x01,0xe0,0x00,0x3d,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
  }
  ,
  {0x55,0x03,0xe0,0x01,0x00,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00    
  },
  {
    0x55,0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
  },
  {
    0x55,0x23,0x00,0x00,0x00,0x00,0x80,0x00,0x04,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
  }
};
    
namespace helium{



 

  void UsbVsRc003::synchyInit(){
    UsbMessage<64> p;
    for (size_t i=0;i<4;i++){

      p=SynchyHeader[i];
      usb.sendAll(p);
    }
    
  }



  const int UsbVsRc003::VENDOR_ID=0x1962;
  const int UsbVsRc003::PRODUCT_ID=0x1000;

  UsbVsRc003::Protocol::Protocol(){
    zeron(readPacket.data,readPacket.completeSize());
    readPacket[0]=0x55;
    readPacket[1]=0x01;
    readPacket[2]=0xe0;
    //readPacket[3]=0x00;
    readPacket[4]=0x10;
    readPacket[5]=0x04;
    //readPacket[6]=0x00;
    readPacket[7]=0x0b;
    readPacket[8]=0x20;
    //readPacket[9]=0x00;
    readPacket[10]=0x8a;
    readPacket[11]=0xec;
    readPacket[12]=0x06;
    readPacket[13]=0x09;//id returned in the reply 
    readPacket[14]=0xe0;
    //readPacket[15]=0x09;//0x08+high part of the address  
    //readPacket[16]=0x02; //low part of the address
    //readPacket[17]=0x02; //bytes to read

    zeron(writePacket.data,writePacket.completeSize());
      writePacket[0]=0x55;
      writePacket[1]=1; //numpackets
      writePacket[2]=0xE0;      
  }

  
  UsbVsRc003::UsbVsRc003(UsbHid& pusb):usb(pusb),gyro(*this){}
  
  

  void UsbVsRc003::request(int addr,int length){
    addr=addr<<1;
    protocol.readPacket[15]=0x08+((addr>>8)&0xFF);
    protocol.readPacket[16]=addr&0xFF;
    protocol.readPacket[17]=length;
    usb.sendAll(protocol.readPacket);    
  }

  



  void UsbVsRc003::set(int addr,const int* what,int numInts){
    addr=addr<<1;
    protocol.writePacket[3]=0x08|((addr&0xFF00)>>8);
    protocol.writePacket[4]=addr&0xFF;
    protocol.writePacket[5]=numInts*2;
    for (int i=0;i<numInts;i++){
      protocol.writePacket[6+i*2]=what[i]&0xFF;
      protocol.writePacket[6+i*2+1]=(what[i]&0xFF00)>>8;
    }
    usb.sendAll(protocol.writePacket);
    memset(&protocol.writePacket[6],0,numInts*2); //clean up what you make dirty (assuming send returns after sending...) ?)

  }



  enum MemLocations{
    CN1_1,CN1_2,CN1_3,CN1_4,CN1_5,CN1_6,
    CN2_1,CN2_2,CN2_3,CN2_4,CN2_5,CN2_6,
    CN3_1,CN3_2,CN3_3,CN3_4,CN3_5,CN3_6,
    CN4_1,CN4_2,CN4_3,CN4_4,CN4_5,CN4_6,
    CN5_1,CN5_2,CN5_3,CN5_4,CN5_5,CN5_6,
    VOICE,
    GYRO=129,
    SERVO_ENABLE=252
  };


 

  int UsbVsRc003::Gyro::rawRead(int idx){
    owner.usb.recvAll(owner.protocol.tmp);
    return owner.getLastTmp(idx);
  }

  
  void UsbVsRc003::Gyro::rawRead(int* rawVals){
    owner.usb.recvAll(owner.protocol.tmp);
    for (int i=0;i<5;i++){
     rawVals[i]=owner.getLastTmp(i);
    }
  }


  void UsbVsRc003::Gyro::rawRead(int* rawVals,const HwIdList& l){
    owner.usb.recvAll(owner.protocol.tmp);
    for (size_t i=0;i<l.ids.size();i++){
      int idx=l.ids[i];
      rawVals[idx]=owner.getLastTmp(idx);
    }
  }

  void UsbVsRc003::Gyro::rawRead(SensorValList<int>& v, const HwIdList& l){
    owner.usb.recvAll(owner.protocol.tmp);
    for (size_t i=0;i<l.ids.size();i++){
     v.vals[i]=owner.getLastTmp(l.ids[i]);
    }
  }


  void UsbVsRc003::Gyro::getToken(Token& t, const HwIdList& toRead){
    t=new DeletableList(toRead);
  }

  void UsbVsRc003::Gyro::rawRead(int* v,const Token& t){
    rawRead(v,*trust_cast<const DeletableList*>(&t));
  }


  size_t UsbVsRc003::Gyro::size() const{
    return 5;
  }

  HwState UsbVsRc003::Gyro::isWorking(int id){
    try{
      rawRead(id);
      return WORKINGHWSTATE;
    }catch(exc::Exception& ){
      return UNREACHABLEHWSTATE;
    }
  }

  void UsbVsRc003::Gyro::isWorking(Array<HwState>& s,const HwIdList& l){
    try{
       owner.usb.recvAll(owner.protocol.tmp);
       setAll(s,WORKINGHWSTATE);
    }catch(exc::Exception& ){
      setAll(s,UNREACHABLEHWSTATE);
    }
  }


  void UsbVsRc003::Gyro::getWorkingOptimizedSplit(Array <WorkingToken>& tk,const HwIdList &l) const{
    tk.resize(1);
    tk[0]=new DeletableList(l);
  }

  void UsbVsRc003::Gyro::isWorking(Callback<const HwStateNotification&>* cb,const WorkingToken& t){
    const HwIdList& l=* trust_cast<const DeletableList*>(&t);
     try{
       owner.usb.recvAll(owner.protocol.tmp);
       for (size_t i=0;i<l.ids.size();i++){
	 (*cb)(std::make_pair(l.ids[i],WORKINGHWSTATE));
       }
     }catch(exc::Exception& ){
       for (size_t i=0;i<l.ids.size();i++){
	 (*cb)(std::make_pair(l.ids[i],UNREACHABLEHWSTATE));
       }
     }
  }


  void UsbVsRc003::setMainPower(bool b){
    int v=b;
    set(SERVO_ENABLE,&v,1);  
  }

   UsbVsRc003::Gyro::Gyro(UsbVsRc003& powner):owner(powner){
     owner.request(GYRO,10);
  }
  


  void UsbVsRc003::rotate(int * pos){
    static UsbMessage<64> d;
    zeron(d.data,d.completeSize());
    d[0]=0x55;
    d[1]=0x03;
    d[2]=0xE0;
    d[3]=0x01;
    d[4]=0x00;
    d[5]=0x80;
    for (int i=0;i<29;i++){
      d[6+i*2]=pos[i]&0xff;
      d[7+i*2]=(pos[i]&0xff00)>>8; 
    }
    usb.sendAll(d);
    
    d[0]=0x55;
    d[1]=0x13;
    for (int i=29;i<60;i++){
      d[2+(i-29)*2]=pos[i]&0xff;
      d[3+(i-29)*2]=(pos[i]&0xff00)>>8; 
    }
    usb.sendAll(d);


    d[01]=0x55;
    d[1]=0x23;
    for (int i=60;i<64;i++){
      d[2+(i-60)*2]=pos[i]&0xff;
      d[3+(i-60)*2]=(pos[i]&0xff00)>>8; 
    }
    d[10]=0x80;
    d[12]=0x02;
    d[13]=0x12;
    usb.sendAll(d);
    

  }

}
