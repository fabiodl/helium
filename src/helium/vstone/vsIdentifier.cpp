#include <helium/vstone/vsIdentifier.h>
#include <helium/vstone/vsByte.h>
#include <helium/thread/timer.h>
#include <helium/math/crc.h>
#include <helium/util/ioUtil.h>





using helium::crc8maxim::crcPush;
using helium::crc8maxim::getCrc;

static const int SYS_PN=0x00;
static const int SYS_VER=0x02;

static const unsigned char RDF=0x20;



namespace helium{


  const char* EnumStrings<VsIdentifier::HwType>::strings[5]={"Vs sv410","Vs ss110","Vs sv3310","=UNSPECIFIED=","Vs ss110kindy"};

  const char* EnumStrings<VsIdentifier::Protocol>::strings[2]={"Vs","Vs+crc"};





VsIdentifier::VsIdentifier(Serial& ps):s(ps){    
    
  }



  VsIdentifier::Info VsIdentifier::getInfo(int id){
    
    static const int size=2;
    unsigned char header[5];
    header[0]=0x80|id;
    header[1]=RDF|size;
    header[2]=SYS_PN;
    header[3]=header[4]=0;
    s.send(header,3);
    milliSleep(50);
    unsigned char reply[3];
    int n;
    n=s.recv(reply,3);//with the crc      
    bool isCrc=true;
    
    if (n<=0){
      isCrc=false;     
      s.send(header+3,2);
      milliSleep(50);
      n=s.recv(reply,2);
      if (n!=2){
	throw exc::InvalidDataValue(stringBegin()<<"When retrieving the Product number without CRC received "<<n<<" bytes"<<stringEnd(),"expected 2");
      }
    }else{
      if (n!=3){
	throw exc::InvalidDataValue(stringBegin()<<"When retrieving the Product number with CRC received "<<n<<" bytes"<<stringEnd(),"expected 3");
      }
      unsigned char crc=getCrc(reply,2, getCrc(header,3));
      if (crc!=reply[2]){
	throw exc::InvalidDataValue("When retrieving the Product number with CRC",stringBegin()<<"received checksum "<<std::hex<<(int)reply[2]<<" instead of "<<(int)crc<<stringEnd());
      }
      
    }
  
    int sys_pn=from14(reply);    
    header[2]=SYS_VER;
    s.send(header,isCrc?3:5);
    s.recvAll(reply,isCrc?3:2,300);
    int sys_ver=from14(reply);    

    VsIdentifier::Info  r;
    r.fwVersion=sys_ver;

    switch(sys_pn){
    case 0:
      r.hw=UNSPECIFIED;
      break;
    case 1000:
      r.hw=VSSV410;
      break;
    case 1100:
      r.hw=VSSV3310;
      break;
    case 2000:
      r.hw=VSSS110;
      break;
    case 1920:
      r.hw=VSSS110Kindy;
      break;
    default:
      throw exc::InvalidDataValue(toString(sys_pn),"unknown product");
    }
    r.protocol=sys_ver>=100?VSDEFAULT:VSCRC;
    return r;    
  }
    
}
