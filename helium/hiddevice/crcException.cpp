#include <helium/hiddevice/crcException.h>
#include <helium/math/crc.h>
#include <helium/util/ioUtil.h>
#include <helium/util/printHex.h>
namespace helium{

  namespace exc{

    CRCException::CRCException(unsigned char expected,unsigned char got):
	Exception(stringBegin()<<"CRC Error: expected "<<std::hex<<(int)expected<<" received "<<(int)got<<stringEnd()){
    }

    CRCException::CRCException(const unsigned char* c,int size):
      Exception(stringBegin()<<"CRC Error: expected "<<std::hex<<(int)crc8maxim::getCrc(c,size-1)<<" received "<<(int)c[size-1]<<stringEnd())
    {
      printHex(c,size);
      std::cout<<"is used for the exc"<<std::endl;
    }


    void CRCException::rethrow(){
      throw *this;
    }
    
    exc::Exception* CRCException::clone() const{
	return new CRCException(*this);
      }



  }//ms exc



}//ns hel
