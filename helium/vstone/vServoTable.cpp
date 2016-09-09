/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/vstone/vServoTable.h>
#include <helium/math/interpolation.h>

namespace helium{

  namespace vservo{
  const char Prop::size[]={
    
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
  };//propSize

    static const double MINTEMP=20;
    static const double TEMPDELTA=5;
    static const int TEMPTABLESIZE=21;
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


    double getTemperature(int val){
      if (val>=tempTable[TEMPTABLESIZE-1]) return MINTEMP+(TEMPTABLESIZE-1)*TEMPDELTA;
      int i=0;
      for (;tempTable[i+1]<val;i++); 
      return linearInterpolation(tempTable[i],MINTEMP+TEMPDELTA*i,tempTable[i+1],MINTEMP+TEMPDELTA*(i+1),val);
    }
    
    
    int AddrTable[motor::PROPERTIESNUM]={vservo::FB_TPOS,vservo::M_POS,vservo::PWM_OFF,vservo::M_TEMP,-1};


    int getAddr(const motor::PropertyRequest& r){    
      if (r.id==motor::RAWPROPERTIES){
	return r.idx;
      }
      if (r.id>=motor::PROPERTIESNUM){
	throw exc::InvalidOperation("VServo does not support property "+toString(r.id));
      }
      return AddrTable[r.id];	
  }
    
    motor::Interface::Property PropNormalize::normProp(int value,const motor::PropertyRequest& r){
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


    motor::Raw<VServoSign>::Property PropNormalize::unnormProp(motor::Interface::Property value,const motor::PropertyRequest& r){
      throw exc::InvalidOperation("Conversion to raw for property "+toString(r)+" not available");
    }


  }//ns vservo
  

}//ns helium
