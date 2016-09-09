/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/vstone/vsCrcException.h>

namespace helium{

  
   namespace exc{

     VsSerialChecksumError::VsSerialChecksumError(int id,int addr,int size):
       HardwareException("Checksum error while writing "+toString(size)+" bytes from "+toString(addr)+" in motor "+toString(id)){       
     }
     
     void VsSerialChecksumError::rethrow(){
       throw *this;
     }
     
     Exception* VsSerialChecksumError::clone(){
       return new VsSerialChecksumError(*this);
     }


     template<>
     const char* VsSerialCRCError<true,true>::doing="reading and writing";
     
     template<>
     const char* VsSerialCRCError<true,false>::doing="reading";
     
     template<>
     const char* VsSerialCRCError<false,true>::doing="writing";
     
     

   }//ns exc


}//ns he


