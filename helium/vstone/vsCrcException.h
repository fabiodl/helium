/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_VSCRCEXCEPTION
#define HE_VSCRCEXCEPTION

#include <helium/robot/hardwareException.h>
#include <helium/util/ioUtil.h>

namespace helium{
  
  namespace exc{

    class VsSerialChecksumError:public exc::HardwareException{ //this is for the ROM 
    public:
      VsSerialChecksumError(int id,int addr,int size);      
      void rethrow();      
      Exception* clone();
    };

    

    template<bool read,bool write>
    class VsSerialCRCError:public exc::HardwareException{
      static const char* doing;
      
    public:

      VsSerialCRCError(const std::string& s):HardwareException(
							       stringBegin()
							       <<"CRC error while "<<doing<<addDetail(s)<<stringEnd()){}
      
      VsSerialCRCError(int id,int addr,int size,unsigned char expected,unsigned char got ):
	HardwareException(
			  stringBegin()
			  <<"CRC error while "<<doing<<" "<<size<<" bytes, device "<<id<<" address 0x"<<std::hex<<addr<<" expected crc 0x"<<(int)expected<<" got 0x"<<(int)got<<std::dec<<((!read)&&(got&0x80)?"(command refused)":"")<<stringEnd() ){
       
      }
      
      void rethrow(){
	throw *this;
      }
      
    Exception* clone(){
      return new VsSerialCRCError(*this);
    }
      
    };





  }//ns exc

}//ns hel


#endif
