/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/vstone/serialVsRc003.h>
#include <helium/robot/deletableList.h>
#include <helium/vector/operations.h>
#include <helium/vstone/vsByte.h>
#include <stdio.h>

static const int _BASE_	=0x200800;
static const int ACCX	=_BASE_ + 0x102;	       
static const int ACCY	=_BASE_ + 0x104;		
static const int ACCZ	=_BASE_ + 0x106;		

static const int GYROX	=_BASE_ + 0x108;		
static const int GYROY	=_BASE_ + 0x10a;		


static const int RPACKET=0;
static const int WPACKET=1;

static const int TIMEOUT=300;

namespace helium{


  SerialVsRc003::SerialVsRc003(Serial& ps):s(ps),gyro(*this){}  


  


  void SerialVsRc003::readToBuffer(int address,int length){        
    sprintf((char*)buffer, "r %06x %02x\x0d\x0a", address,2*length);
    s.send(buffer,strlen((char*)buffer));
    s.recvAll(buffer,2*length+2,TIMEOUT);//+2 is for CR/LF
  }

  void SerialVsRc003::read(int* d,int address,int length){    
    readToBuffer(address,length);
    for (int i=0;i<length;i++){
      fromLE(d[i],buffer+i*2);
    }
  }


  
  int SerialVsRc003::Gyro::rawRead(int idx){
    int r;
    owner.read(&r,ACCX+idx,1);
    return r;
  }

  void SerialVsRc003::Gyro::rawRead(int* d){
    owner.read(d,ACCX,5);
  }

  void SerialVsRc003::Gyro::rawRead(int* d,const HwIdList& l){
    owner.readToBuffer(ACCX,5);
    for (size_t i=0;i<l.ids.size();i++){
      int idx=l.ids[i];      
      fromLE(d[idx],owner.buffer+idx*2);
    }
  }

  void SerialVsRc003::Gyro::rawRead(SensorValList<int>& v, const HwIdList& l){
    v.vals.resize(l.ids.size());
    owner.readToBuffer(ACCX,5);
    for (size_t i=0;i<l.ids.size();i++){
      int idx=l.ids[i];
      fromLE(v.vals[i],owner.buffer+idx*2);
    }      
  }

  void SerialVsRc003::Gyro::getToken(Token& t, const HwIdList& toRead){
    t=new DeletableList(toRead);
  }
  
  void SerialVsRc003::Gyro::rawRead(int* d,const Token& t){
    rawRead(d,*trust_cast<const DeletableList*>(&t));
  }
  
  size_t SerialVsRc003::Gyro::size() const{
    return 5;
  }
  
  SerialVsRc003::Gyro::Gyro(SerialVsRc003& powner):owner(powner){
  }




  HwState SerialVsRc003::Gyro::isWorking(int id){
    try{
      rawRead(id);
      return WORKINGHWSTATE;
    }catch(exc::Exception& ){
      return GENERICFAULTYHWSTATE;
    }
  }

  void SerialVsRc003::Gyro::isWorking(Array<HwState>& v,const HwIdList& l){
    int all[5];
    try{
      rawRead(all,l);
      setAll(v,WORKINGHWSTATE);
    }catch(exc::Exception& ){
      setAll(v,GENERICFAULTYHWSTATE);
    }    
  }

  void  
SerialVsRc003::Gyro::getWorkingOptimizedSplit(Array <WorkingToken>& tk,const HwIdList& l) const{
    tk.resize(1);
    tk[0]=new DeletableList(l);
  }
  

  void  SerialVsRc003::Gyro::isWorking(Callback<const HwStateNotification&>* cb,const WorkingToken& tk){
    const DeletableList& l= *trust_cast<const DeletableList*>(&tk);
    
    try{
      int all[5];
      rawRead(all,l);
      for (size_t i=0;i<l.ids.size();++i){
	(*cb)(std::make_pair(l.ids[i],WORKINGHWSTATE));
      }
    }catch(exc::Exception& ){
      for (size_t i=0;i<l.ids.size();++i){
	(*cb)(std::make_pair(l.ids[i],GENERICFAULTYHWSTATE));
      }
    }
    
  }

  SerialVsRc003::Probe::Probe(const std::string& name):SerialProbe::Prober(name){
  }

  bool SerialVsRc003::Probe::probe(Serial* s){
    SerialVsRc003 test(*s);
    try{
      test.gyro.rawRead(0);      
    }catch(exc::Exception& ){
      return false;
    }
    return true;
  }

  
  


}//ns helium
