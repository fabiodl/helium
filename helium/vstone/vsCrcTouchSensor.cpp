/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/vstone/vsCrcTouchSensor.h>
#include <helium/math/crc.h>
#include <helium/vector/operations.h>
namespace helium{

  namespace vsTs{

    const unsigned char BoardFlags::INVALIDFLAG=0xFF;

    ReadData::ReadData():minId(CHANNELS),maxId(-1){
      setAll(gid,-1);
    }
    
    void ReadData::markRead(int sid,int pgid){
      minId=(std::min)(sid,minId);
      maxId=(std::max)(sid,maxId);
      //std::cout<<"marked "<<sid<<"now"<<minId<<"~"<<maxId<<std::endl;
      gid[sid]=pgid;
    }
    
    
    void BoardFlags::reset(){
      zeron<vsTs::CHANNELS>(data);
    }
    
    bool BoardFlags::operator!=(const BoardFlags& b){
      return differsn<vsTs::CHANNELS>(data,b.data);
    }

    void BoardFlags::invalidate(){
      setAlln<vsTs::CHANNELS>(data,INVALIDFLAG);
    }
    
    
      BurstToken::BoardData::BoardData(){
	setAll(rgid,-1);
	setAll(wgid,-1);
      }
    
    void BurstToken::BoardData::markRead(int sid,int pgid){
      rgid[sid]=pgid;
    }
    
    void BurstToken::BoardData::markWrite(int sid,int pgid){
      wgid[sid]=pgid;
    }
    
    void BurstToken::BoardData::finalize(){
      unsigned char rburst=0,wburst=0;
      rSize=wSize=0;
      for (int i=0;i<vsTs::CHANNELS;i++){
	if (rgid[i]!=-1){
	  rburst|=1<<i;
	  rSize+=2;//2 bytes
	}
	if (wgid[i]!=-1){
	  wburst|=1<<i;
	  wSize++;
	}            
      }    
      unsigned char header=0xC0 | bid;    
      rCrc=wCrc=crc8maxim::getCrc(header);
      crc8maxim::crcPush(rCrc,rburst);
      crc8maxim::crcPush(wCrc,wburst);
      type=rSize?(!wSize?BURST_READ:BURST_READWRITE):BURST_WRITE;
    }
    
    void BurstToken::BoardData::getFlags(BoardFlags& flags) {
      for (int i=0;i<vsTs::CHANNELS;i++){
	flags.data[i]= (rgid[i]==-1?0:vsTs::IS_BURST_READ)
	  | (wgid[i]==-1?0:(vsTs::IS_BURST_WRITE|vsTs::IS_LED) );
      }      
    }
    
  //--BurstToken
  
  
    void BurstToken::set(const std::map<int,BurstToken::BoardData>& m){
      bdata.resize(m.size());
      int c=0;
      for (std::map<int,BurstToken::BoardData>::const_iterator it=m.begin();it!=m.end();++it){
	bdata[c]=it->second;
	bdata[c].bid=it->first;
	bdata[c].finalize();
	++c;
      }
    }
  

  
    BurstToken::BurstToken():wasInit(false),writerf(NULL){    
    }
    
   
    BurstToken::BurstToken(const std::map<int,BurstToken::BoardData>& m):wasInit(false),writerf(NULL){
      set(m);
    }

    BurstToken::BurstToken(const std::map<int,BurstToken::BoardData>& m,WriterF wf):wasInit(false),writerf(wf){
      set(m);
    }

   
   
    void BurstData::invalidate(){
      for (size_t i=0;i<curFlags.size();i++){
	curFlags[i].invalidate();
      }
      activeToken=NULL;
    }
    
    
    
    BurstData::BurstData(int boardn):
      curFlags(boardn){
      invalidate();
    }
    
  

    void LocklessLedBurstSetter::operator()(unsigned char* c){
      for (size_t i=0;i<vals.size();++i){
	c[i]=vals[i]*0x7F;
      }
    }
    
     WriterF LocklessLedBurstSetter::getWriterF()  {
      return this;
    }

    const HwIdList& LocklessLedBurstSetter::getBurstWriteIds(){
      return onBurst;
    }


    LocklessLedBurstSetter::LocklessLedBurstSetter(SensorSpooler<int>& pspool,int n):spool(pspool),vals(n){
      zero(vals);
    }
    
    void LocklessLedBurstSetter::do_setOnBurst(const HwIdList& h){
      onBurst=h;
    }

    void LocklessLedBurstSetter::do_setBurstDuty(const double * d){
      vals=d;
    }
    
    void LocklessLedBurstSetter::setOnBurst(const HwIdList& h){
      StackJob<sensor::Set<int>,
	       LocklessLedBurstSetter,
	       const HwIdList&,
	       onlyParam<const HwIdList&>,
	       &LocklessLedBurstSetter::do_setOnBurst,true>
    job(this,h);	
    spool.waitJob(&job,SensSpoolPriority::HIGH);
  }
    
  void LocklessLedBurstSetter::setBurstDuty(const double* d){
    StackJob<sensor::Set<int>,
	     LocklessLedBurstSetter,
	     const double*,
	     onlyParam<const double*>,
	     &LocklessLedBurstSetter::do_setBurstDuty,true>
      job(this,d);	
    spool.waitJob(&job,SensSpoolPriority::HIGH);
  }
  

    

  }
}
