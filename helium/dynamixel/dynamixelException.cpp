/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#include <helium/dynamixel/dynamixelException.h>
#include <helium/dynamixel/dynamixelSerial.h>
#include <helium/util/ioUtil.h>
#include <helium/util/printHex.h>




namespace helium{


  std::string describeInstruction(dynamixel::InstructionType t){
    switch(t){
    case dynamixel::PING: return "Ping";
    case dynamixel::READ_DATA: return "Read";
    case dynamixel::WRITE_DATA: return "Write";
    case dynamixel::REG_WRITE: return "Reg Write";
    case dynamixel::ACTION: return "Action";
    case dynamixel::RESET: return "Reset";
    case dynamixel::SYNC_WRITE: return "Sync write";      
    }
    return "Uknown instruction";
  }

  std::string describePacket(const unsigned char* packet){
    return stringBegin()<<"ID="<<(int)packet[dynamixel::ID]
			 <<",length="<<(int)packet[dynamixel::LENGTH]
			 <<",instruction="<<describeInstruction((dynamixel::InstructionType)packet[dynamixel::INSTRUCTION])
			<<",params="<<asHexString(packet+dynamixel::INSTRUCTION,packet[dynamixel::LENGTH]-2)
			<<stringEnd();
     
    

  }

  std::string describeErrors(unsigned char e){
    return stringBegin()
      <<(e&dynamixel::INSTRUCTIONERROR?"Instruction ":"")
      <<(e&dynamixel::INSTRUCTIONERROR?"Overload ":"")
      <<(e&dynamixel::CHECKSUMERROR?"Checksum ":"")
      <<(e&dynamixel::RANGEERROR?"Range ":"")
      <<(e&dynamixel::OVERHEATINGERROR?"Overheat ":"")
      <<(e&dynamixel::ANGLELIMITERROR?"Anglelimit ":"")
      <<(e&dynamixel::INPUTVOLTAGEERROR?"InputVoltage ":"")
      <<stringEnd();      
  }


namespace exc{


  DynamixelException::DynamixelException(const unsigned char* packet,const std::string& operation):HardwareException(addDetail(operation,""," with ")+describePacket(packet)){
    
  } 

  void DynamixelException::rethrow(){
    throw *this;
  }

  Exception* DynamixelException::clone(){
    return new DynamixelException(*this);
  }
  


  DynamixelCsumException::DynamixelCsumException(const unsigned char* packet,unsigned char expected,unsigned char got):DynamixelException(packet,stringBegin()<<"Checksum mismatch: expected "<<(int)expected<<" got "<<(int)got<<stringEnd() ){
    
  } 

  void DynamixelCsumException::rethrow(){
    throw *this;
  }

  Exception* DynamixelCsumException::clone(){
    return new DynamixelCsumException(*this);
  }

  DynamixelStatusError::DynamixelStatusError(const unsigned char* packet,unsigned char errorflags): 
    DynamixelException(packet,"reported errors "+describeErrors(errorflags))
  {    
  }

   void DynamixelStatusError::rethrow(){
    throw *this;
   }


  Exception* DynamixelStatusError::clone(){
    return new DynamixelStatusError(*this);
  }


}

}
