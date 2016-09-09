/*!
 * \brief   libhelium
 * \details http://sourceforge.net/projects/libhelium/
 * \author  Fabio Dalla Libera
 * \date    2013.02.18
 * \version 0.1
 * Release_flags s
 */


#include <helium/vector/operations.h>
#include <helium/dynamixel/dynamixelException.h>
#include <helium/util/printHex.h>
#include <helium/util/byteConversion.h>


namespace helium{
  
   template<typename Srl>
   void DynamixelSerial<Srl>::communicate(int id,dynamixel::InstructionType inst,int params){
      //4: 2*head,id,length,instruction,csum
      int totalLength=params+6;      
      outbuff[dynamixel::ID]=(unsigned char)id;
      outbuff[dynamixel::LENGTH]=params+2;
      outbuff[dynamixel::INSTRUCTION]=(unsigned char)inst;      
      //3: 2+head, csum      
      outbuff[totalLength-1]=~sumn(outbuff+dynamixel::ID,totalLength-3);
      s.send(outbuff,totalLength);
      s.recvAll(inbuff,totalLength,ADDITIONALTIME);
      if (differsn(outbuff,inbuff,totalLength)){
	std::cout<<"out"<<std::endl;
	printHex(outbuff,totalLength);
	std::cout<<"in"<<std::endl;
	printHex(inbuff,totalLength);
	throw exc::DynamixelException(outbuff,"Readback failed");

      }



      unsigned char srl=(id==dynamixel::BROADCASTID?
			 dynamixel::NEVERRETURNSTATUS:statusReturnLevel[id]);
      
      if (
	  (srl==dynamixel::ALWAYSRETURNSTATUS)||
	  (
	   (srl==dynamixel::ONLYONREADSTATUS)&&(outbuff[dynamixel::INSTRUCTION]==dynamixel::READ_DATA)
	   )
	   
	  )
	{	  
	  int length=6+(outbuff[dynamixel::INSTRUCTION]==dynamixel::READ_DATA?
			outbuff[dynamixel::READLENGTH]:0);		  	
	  //6: 2*head,id,length,csum
	  
	  try{
	    s.recvAll(inbuff,length,ADDITIONALTIME);
	  }catch(exc::Exception& e){
	    throw exc::DynamixelException(outbuff,"receive error"+toString(e));
	  }
	  //3: 2*head,csum
	  unsigned char csum=~sumn(inbuff+dynamixel::ID,length-3);
	  if (csum!=inbuff[length-1]){
	    throw exc::DynamixelCsumException(outbuff,csum,inbuff[length-1]);
	  }	
	  if (inbuff[dynamixel::STATUSERRORFLAGS]){
	    throw exc::DynamixelStatusError(outbuff,inbuff[dynamixel::STATUSERRORFLAGS]);
	  }     
	}//hasstatus
   }//communicate
  
  template<typename Srl>
     DynamixelSerial<Srl>::DynamixelSerial(Srl& ps,int maxId):s(ps),statusReturnLevel(maxId+1){
      outbuff[0]=outbuff[1]=0xFF;
      setAll(statusReturnLevel,dynamixel::ALWAYSRETURNSTATUS);
    }
  

  template<typename Srl>
  void DynamixelSerial<Srl>::setStatusReturnLevel(int id,dynamixel::StatusReturnLevel l){
    statusReturnLevel[id]=l;
  }

  
  template<typename Srl>
  void DynamixelSerial<Srl>::ping(int id){
    communicate(id,dynamixel::PING,0);
  }
  
  
  template<typename Srl>
  void DynamixelSerial<Srl>::action(int id){
    communicate(id,dynamixel::ACTION,0);
  }
  

    template<typename Srl>
    void DynamixelSerial<Srl>::reset(int id){
      communicate(id,dynamixel::RESET,0);
    }
    
  template<typename Srl>
    void DynamixelSerial<Srl>::recv(int id,int addr, unsigned char* data, size_t n){
      outbuff[dynamixel::PARAMETERSTART]=(unsigned char)addr;
      outbuff[dynamixel::PARAMETERSTART+1]=n;
      communicate(id,dynamixel::READ_DATA,2);
      memcpy(data,inbuff+dynamixel::PARAMETERSTART,n);
    }

  template<typename Srl>
  void DynamixelSerial<Srl>::send(int id,int addr,const unsigned char* data, size_t n){
      outbuff[dynamixel::PARAMETERSTART]=(unsigned char)addr;
      memcpy(outbuff+dynamixel::PARAMETERSTART+1,data,n);
      communicate(id,dynamixel::WRITE_DATA,n+1);
    }


template<typename Srl>
void DynamixelSerial<Srl>::regWrite(int id,int addr,const unsigned char* data, size_t n){
      outbuff[dynamixel::PARAMETERSTART]=(unsigned char)addr;
      memcpy(outbuff+dynamixel::PARAMETERSTART+1,data,n);
      communicate(id,dynamixel::REG_WRITE,n+1);
    }


  template<typename Srl>
  unsigned char* DynamixelSerial<Srl>::getSyncWriteBuffer(){
    return outbuff+(dynamixel::PARAMETERSTART+2);
  }

  template<typename Srl>
  void DynamixelSerial<Srl>::syncWrite(int addr,int nmotors,int singleMotordataLength){
      outbuff[dynamixel::PARAMETERSTART]=(unsigned char)addr;
      outbuff[dynamixel::PARAMETERSTART+1]=(unsigned char)singleMotordataLength;
      communicate(dynamixel::BROADCASTID,dynamixel::SYNC_WRITE,2+(singleMotordataLength+1)*nmotors);
    }
  

  namespace dynamixel{
    template<int bytes> struct rwCaller;
    
    template<> 
    struct rwCaller<1>{

      template<typename Srl>
      inline static int read(DynamixelSerial<Srl>& ds,int id,int addr){
	unsigned char c;
	ds.recv(id,addr,&c,1);
	return c;
      }
      template<typename Srl>

      inline static void write(DynamixelSerial<Srl>& ds,int id,int addr,int val){
	unsigned char c=(unsigned char)val;
	ds.send(id,addr,&c,1);
      }



    };//rwCaller<1>

    
    template<> 
    struct rwCaller<2>{

      template<typename Srl>
      inline static int read(DynamixelSerial<Srl>& ds,int id,int addr){
	unsigned char c[2];
	ds.recv(id,addr,c,2);
	return fromLE(c);
      }
      
      template<typename Srl>
      inline static void write(DynamixelSerial<Srl>& ds,int id,int addr,int val){
	unsigned char c[2];
	toLE(c,val);
	//std::cout<<"Sending";
	//printHex(c,2);
	//std::cout<<std::endl;
	ds.send(id,addr,c,2);
      }



    };//rwCaller<2>

    
  }//ns dynamixel

  template<typename Srl>
  template<int bytes> int  DynamixelSerial<Srl>::read(int id,int addr){
    return dynamixel::rwCaller<bytes>::read(*this,id,addr);
  }

  template<typename Srl>
  template<int bytes> void DynamixelSerial<Srl>::write(int id,int addr,int val){
    dynamixel::rwCaller<bytes>::write(*this,id,addr,val);
  }



}
