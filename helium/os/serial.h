/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SERIAL
#define HE_SERIAL
#include <helium/core/exception.h>
#include <helium/core/time.h>

namespace helium{

  struct Handle;
  
  namespace exc{
    class SerialPortException:public Exception{
    public:
      SerialPortException(const std::string& serialPort,const std::string& msg):Exception("Serial communication, port "+serialPort+":"+msg){
      }
      
      virtual void rethrow(){
	throw *this;
      }
      Exception* clone() const{
	return new SerialPortException(*this);
      }
    };
    
    class SerialPortWriteException:public SerialPortException{
    public:
      SerialPortWriteException(const std::string& serialPort,int size);
      virtual void rethrow(){
	throw *this;
      }
      Exception* clone() const{
	return new SerialPortWriteException(*this);
      }
};
    
      
    class SerialPortReadException:public SerialPortException{
    public:
      SerialPortReadException(const std::string& serialPort,int size,const std::string& msg="");
      
      virtual void rethrow(){
	throw *this;
      }
      Exception* clone() const{
	return new SerialPortReadException(*this);
      }
    };

    class SerialPortReadTimeoutException:public SerialPortReadException{
    public:
      SerialPortReadTimeoutException(const std::string& serialPort,int size,Time timeout);
      
      virtual void rethrow(){
	throw *this;
      }
      Exception* clone() const{
	return new SerialPortReadTimeoutException(*this);
      }
    };

  }//ns exc

  class Serial{
    Handle& h;
    std::string portname;
  public:   

    const std::string& getName();
    
    enum Parity{NO_PARITY,ODD_PARITY,EVEN_PARITY,MARK_PARITY,SPACE_PARITY};
    enum StopBit{ONE_SB,TWO_SB,ONEHALF_SB};
    enum HardwareControl{NO_HW_CONTROL,CTS_CONTROL};

    Serial(const char *portname, int BaudRate,  int timeout,int dataBits=8,StopBit stopBits=ONE_SB,Parity parityBit=NO_PARITY,HardwareControl hardwareControl=NO_HW_CONTROL);          
    ~Serial();
    void send(const unsigned char* buff,int size);
    int recv(unsigned char* data,int size);

    //void sendAll(unsigned char* data,int size);
    void recvAll(unsigned char* data,int size,Time additionalTime);
    int recvUpToTerminator(unsigned char* data,unsigned char terminator,Time additionalTime);
    void close();
    void flush();
    void setOptions(int BaudRate,int timeout,int dataBits=8,StopBit stopBits=ONE_SB,Parity parityBit=NO_PARITY,HardwareControl hardwareControl=NO_HW_CONTROL);

  };
  
}
#endif
