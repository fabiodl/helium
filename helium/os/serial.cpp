/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/os/serial.h>
#include <helium/util/ioUtil.h>
#include <helium/os/time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif



//#define DEBUGSEND
//#define DEBUGRECV



namespace helium{

  const std::string& Serial::getName()
  {
    return portname;
  }

 namespace exc{
    SerialPortWriteException::SerialPortWriteException(const std::string& serialPort,int size):SerialPortException(serialPort,"while writing "+toString(size)+" bytes"){}
    SerialPortReadException::SerialPortReadException(const std::string& serialPort,int size,const std::string& msg):SerialPortException(serialPort,"while reading "+toString(size)+" bytes"+(msg==""?msg:":"+msg)){}
    SerialPortReadTimeoutException::SerialPortReadTimeoutException(const std::string& serialPort,int size,Time timeout):SerialPortReadException(serialPort,size,"timed out, additional time"+toString(timeout)){}
  }



#ifndef _WIN32  

  struct Handle{
    int handle;
  };


 

  void Serial::setOptions(int baudRate, int timeout,int dataBits,StopBit stopBits,Parity parityBit,HardwareControl hardwareControl){
          speed_t speed;
      tcflag_t data,stop,parity;
      switch(baudRate){
      case 0:
	speed=B0;break;
      case 50:
	speed=B50;break;
      case 75:
	speed=B75;break;
      case 110:
	speed=B110;break;
      case 134:
	speed=B134;break;
      case 150:
	speed=B150;break;
      case 200:
	speed=B200;break;
      case 300:      
      speed=B300;break;
      case 600:
	speed=B600;break;
      case 1200:
      speed=B1200;break;
      case 1800:
	speed=B1800;break;
      case 2400:
	speed=B2400;break;
      case 4800:
	speed=B4800;break;
      case 9600:
	speed=B9600;break;
      case 19200:
      speed=B19200;break;
      case 38400:
	speed=B38400;break;
      case 57600:
	speed=B57600;break;
      case 115200:
	speed=B115200;break;
      case 230400:
	speed=B230400;break;
#ifndef __APPLE__
      case 460800:
	speed=B460800;break;
      case 500000:
	speed=B500000;break;
      case  576000:
	speed=B576000;break;
      case 1000000:
	speed=B1000000;break;
#else
      case 460800:
      case 576000:
      case 500000:	
      case 1000000:
	speed=baudRate;
	break;	
#endif
      default:	
	throw exc::InvalidOperation("Invalid baud rate"+toString(baudRate));
      }
      
      
      switch(dataBits){
      case 8:
	data=CS8;
	break;
      case 7:
	data=CS7;
	break;
      case 6:
	data=CS6;
	break;
      case 5:
	data=CS5;
	break;
      default:
	throw exc::InvalidOperation("Invalid data bits"+toString(dataBits));
      }

      switch(stopBits){
      case ONE_SB:
	stop=0;
	break;
      case TWO_SB:
	stop=CSTOPB;
	break;
      default:
	throw exc::InvalidOperation("Invalid stop bits"+toString(stopBits));      
      }

      switch(parityBit){
      case SPACE_PARITY:
      case NO_PARITY:
	parity=0;
	break;
      case ODD_PARITY:
	parity= PARENB | PARODD;
	break;
      case EVEN_PARITY:
	parity= PARENB;
	break;	
      default:
	throw exc::InvalidOperation("Invalid parity"+toString(parityBit));      
      }
      /* comments from http://www.easysw.com/~mike/serial/serial.html */


      /*The O_NOCTTY flag tells UNIX that this program doesn't want to be the "controlling terminal" for that port. 
	If you don't specify this then any input (such as keyboard abort signals and so forth) will affect your process.
	Programs like getty(1M/8) use this feature when starting the login process, but normally a user program does not want this behavior.
      */

      /*
	The O_NDELAY flag tells UNIX that this program doesn't care what state the DCD signal line is in - 
	whether the other end of the port is up and running. 
	If you do not specify this flag, your process will be put to sleep until the DCD signal line is the space voltage.*/
   



      ///* cutecom
      //flush(); //edited fb, flush after opening


      /*The FNDELAY option causes the read function to return 0 if no characters are available on the port. 
	To restore normal (blocking) behavior, call fcntl() without the FNDELAY option:
	[FNDELAY, not O_NDELAY!]
	
	Timeouts are ignored when the NDELAY option is set on the file via open or fcntl.

      */

      int n = fcntl(h.handle, F_GETFL, 0); 
      fcntl(h.handle, F_SETFL, n & ~FNDELAY); //removes exception
      //*/

      struct termios attribs;
      
      if(tcgetattr(h.handle, &attribs) < 0){
	throw exc::SerialPortException(portname,"retrieving attribs failed");
      }


      /*
	The c_cflag member controls the baud rate, number of data bits, parity, stop bits, 
	and hardware flow control
      */

      if(cfsetispeed(&attribs, speed) < 0){
	throw exc::SerialPortException(portname,"setting the speed failed");
      }
      if(cfsetospeed(&attribs, speed) < 0){
	throw exc::SerialPortException(portname,"setting the speed failed");
      }
      

      /*CSIZE	Bit mask for data bits
	CSTOPB	2 stop bits (1 otherwise)
	CREAD	Enable receiver
	PARENB	Enable parity bit
	PARODD	Use odd parity instead of even
	HUPCL	Hangup (drop DTR) on last close
	CLOCAL	Local line - do not change "owner" of port
	LOBLK	Block job control output
	CNEW_RTSCTS 
	CRTSCTS	Enable hardware flow control (not supported on all platforms)
       */
            

      attribs.c_cflag &= ~(CSIZE | CSTOPB | PARENB | PARODD | HUPCL |  CRTSCTS);
      if (hardwareControl==CTS_CONTROL) attribs.c_cflag |=CRTSCTS;


      /*
	The c_cflag member contains two options that should always be enabled, CLOCAL and CREAD. 
	These will ensure that your program does not become the 'owner' of the port subject to 
	sporatic job control and hangup signals, and also that the serial interface driver will read incoming data bytes.
      */

      


      attribs.c_cflag |=data | stop | parity | CLOCAL | CREAD; 

      /* cutecom
      attribs.c_cflag = (attribs.c_cflag & ~CSIZE) | CS8;
      attribs.c_cflag |= CLOCAL | CREAD;
      attribs.c_cflag &= ~(PARENB | PARODD);
      attribs.c_cflag |= parity;
      attribs.c_cflag &= ~CRTSCTS; // CRTSCTS = output hardware flow control only used if the cable has all necessary lines
      attribs.c_cflag |= stop;
      */



      /*The local modes member c_lflag controls how input characters are managed by the serial driver. 
	In general you will configure the c_lflag member for canonical or raw input.

	Choosing Raw Input

	Raw input is unprocessed. Input characters are passed through exactly as they are received, when they are received. Generally you'll deselect the ICANON, ECHO, ECHOE, and ISIG options when using raw input:
	ISIG	Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals
	ICANON	Enable canonical input (else raw)
	ECHO	Enable echoing of input characters
	ECHOE	Echo erase character as BS-SP-BS
      */




      attribs.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);


      /*
	ECHO
	Enable echo.
	ECHOE
	Echo erase character as error-correcting backspace.
	ECHOK
	Echo KILL.
	ECHONL
	Echo NL.
	ICANON
	Canonical input (erase and kill processing).
	IEXTEN
	Enable extended input character processing.
	ISIG
	Enable signals.
	NOFLSH
	Disable flush after interrupt or quit.
	TOSTOP
	Send SIGTTOU for background output.
	XCASE
	Canonical upper/lower presentation (LEGACY).*/
      
      
      attribs.c_lflag &= ~(ECHOK | ECHOCTL | ECHOKE |IEXTEN  );

      /*
	The input modes member c_iflag controls any input processing that is done to characters received 
	on the port. 

	INPCK	Enable parity check
	ISTRIP	Strip parity bits

	IGNBRK	Ignore break condition(used in cutecom)
       */

      if (parity& PARENB){
	attribs.c_iflag |= (INPCK | ISTRIP);
      }else{
	attribs.c_iflag &= ~(INPCK | ISTRIP);
	/* IGNBRK	Ignore break condition*/
	 attribs.c_iflag |=IGNBRK; //from cutecom
      }

     

      /*Software flow control is enabled using the IXON, IXOFF, and IXANY constants:

	options.c_iflag |= (IXON | IXOFF | IXANY);
	To disable software flow control simply mask those bits:

	IXON	Enable software flow control (outgoing)
	IXOFF	Enable software flow control (incoming)
	IXANY	Allow any character to start flow again

       */

      attribs.c_iflag &= ~(IXON | IXOFF | IXANY);
      
      /*The c_iflag field describes the basic terminal input control:
	BRKINT
	Signal interrupt on break.
	ICRNL
	Map CR to NL on input.
	IGNBRK
	Ignore break condition.
	IGNCR
	Ignore CR
	IGNPAR
	Ignore characters with parity errors.
	INLCR
	Map NL to CR on input.
	INPCK
	Enable input parity check.
	ISTRIP
	Strip character
	IUCLC
	Map upper-case to lower-case on input (LEGACY).
	IXANY
	Enable any character to restart output.
	IXOFF
	Enable start/stop input control.
	IXON
	Enable start/stop output control.
	PARMRK
	Mark parity errors.*/
      
      attribs.c_iflag &=  ~(BRKINT | ICRNL | IGNCR | INLCR);
      

      /*
	The c_oflag member contains output filtering options.
	Like the input modes, you can select processed or raw data output.
	Raw output is selected by resetting the OPOST option in the c_oflag member:

	options.c_oflag &= ~OPOST;

	When the OPOST option is disabled, all other option bits in c_oflag are ignored.


       */

      attribs.c_oflag &= ~OPOST & ~ONLCR & ~OCRNL;


      /*The c_cc character array contains control character definitions as well as timeout parameters.
	The VSTART and VSTOP elements of the c_cc array contain the characters used for software flow control. 
	Normally they should be set to DC1 (021 octal) and DC3 (023 octal) which represent the ASCII standard 
	XON and XOFF characters.
	
	Timeouts are ignored in canonical input mode or when the NDELAY option is set on the file
	via open or fcntl.
       */


      /*
	VMIN specifies the minimum number of characters to read. 
	If VMIN=0, then the VTIME value specifies the time to wait for every character read.

         Note that this does not mean that a read call for N bytes will wait for N characters to come in. 
	 Rather, the timeout will apply to the first character and the read call will return the number 
	 of characters immediately available (up to the number you request)

	If VMIN is non-zero, VTIME specifies the time to wait for the first character read.
         If a character is read within the time given, any read will block (wait) until all VMIN characters 
          are read. That is, once the first character is read, the serial interface driver expects to receive 
          an entire packet of characters (VMIN bytes total)
         If no character is read within the time allowed, then the call to read returns 0.
          This method allows you to tell the serial driver you need exactly N bytes and any read call 
          will return 0 or N bytes. However, the timeout only applies to the first character read, so 
          if for some reason the driver misses one character inside the N byte packet then the read call 
          could block forever waiting for additional input characters. 
        

        VTIME specifies the amount of time to wait for incoming characters in tenths of seconds. 
         If VTIME=0 (the default), reads will block (wait) indefinitely 
         unless the NDELAY option is set on the port with open or fcntl.         
      */

      //std::cout<<"Timeout "<<timeout<<std::endl;

       if (timeout>0){
	attribs.c_cc[VTIME]=(timeout)/100;
	attribs.c_cc[VMIN]=0;
      }else{
	//blocking
	attribs.c_cc[VTIME]=0;
	attribs.c_cc[VMIN]=1;
      }


      //if time not set likethis, sometimes it will give exception
      //attribs.c_cc[VTIME]=1;
      //attribs.c_cc[VMIN]=60;

      ///* cutecom
      //prevent sudden dropof connection
      //attribs.c_lflag=0;
      //attribs.c_oflag=0;
      //if (tcsetattr(h.handle, TCSANOW, &attribs)!=0)
      //std::cerr<<"tcsetattr() 1 failed"<<std::endl;
      //int mcs=0;
      //ioctl(h.handle, TIOCMGET, &mcs);
      //mcs |= TIOCM_RTS;
      //ioctl(h.handle, TIOCMSET, &mcs);
      //*/





      /*
	TCSANOW	Make changes now without waiting for data to complete
	TCSADRAIN	Wait until everything has been transmitted
	TCSAFLUSH	Flush input and output buffers and make the change
       */
       
       //std::cout<<"ATTRIBS"<< attribs.c_cflag<<" "<< attribs.c_lflag<<" "<<	attribs.c_iflag<<" "<< attribs.c_oflag<<" "<<attribs.c_cc[VTIME]<<" "<<attribs.c_cc[VMIN]<<std::endl;
       
       
       ///attribs.c_lflag=0; //2592
       //attribs.c_iflag=1;  //257
	 
      if(tcsetattr(h.handle, TCSANOW, &attribs) <0 ) {
	throw exc::SerialPortException(portname,"setting attribs failed");
      }
  }



  Serial::Serial(const char *pportname, int baudRate, int timeout,int dataBits,StopBit stopBits,Parity parityBit,HardwareControl hardwareControl):h(* new Handle()),portname(pportname){
    h.handle=-1;
    try{
      h.handle = open(portname.c_str(), O_RDWR | O_NOCTTY | O_NDELAY); 
      if (h.handle<0){
	throw exc::SerialPortException(portname,"opening failed");
      }
      setOptions(baudRate,timeout,dataBits,stopBits,parityBit,hardwareControl);      
    }catch(exc::Exception& e){
      if (h.handle>=0){
	::close(h.handle);
      }
      delete &h;
      throw;
    }
    
  }
  
  void Serial::flush(){
    if (tcflush(h.handle,TCIOFLUSH)==-1){
      throw exc::SerialPortException(portname,"flush failed");
    }
  }
  
  
  void Serial::send(const unsigned char* buff,int size){
    /*if (tcflush(h.handle,TCIOFLUSH)==-1){
       throw exc::SerialPortException(portname,"flush failed");
       }*/
    /*if (size<0){
      throw exc::Bug("WTF!");
      }*/


#ifdef DEBUGSEND
    std::cout<<"sending packet ("<<size<<")"<<std::hex;
    for (size_t i=0;i<size;i++){
      std::cout<<(int)buff[i]<<" ";
      //sum+=buff[i];
    }
    std::cout<<std::dec<<std::endl;
#endif

    /*
    ::write(h.handle,buff,size);
    milliSleep(1);
    char c=0;
    ::write(h.handle,&c,1);
    return;
    */

    int n=::write(h.handle,buff,size);
    if (n!=size){
      //std::cout<<"error "<<n<< " instread of "<<size<<std::endl;
      throw exc::SerialPortWriteException(portname,size);
    }else{
      //std::cout<<"wrote "<<size<<std::endl;
    }
  }

  int Serial::recv(unsigned char* data,int size){
    int n=::read(h.handle,data,size);
    

#ifdef DEBUGRECV
    std::cout<<"received packet ("<<n<<")"<<std::hex;
    for (size_t i=0;i<n;i++){
      std::cout<<(int)data[i]<<" ";
    }
    std::cout<<std::dec<<std::endl;
#endif

    //std::cout<<"received "<<n<<std::endl;
    if (n<0){
      throw exc::SerialPortReadException(portname,size);
    }   
    return n;
  }


  void Serial::close(){    
    ::close(h.handle);         
  }

  

  Serial::~Serial(){    
    close();
    delete &h;
  }



#else
  
  struct Handle{
    HANDLE handle;
  };

  
  void Serial::setOptions(int baudRate, int timeout,int dataBits,StopBit stopBits,Parity parityBit, HardwareControl hardwareControl){
    DCB    stDcb;
    if(GetCommState(h.handle, &stDcb) == FALSE){
      throw exc::SerialPortException(portname,"retrieving attribs failed");
    }
    DWORD speed; BYTE stop,parity;
    
    switch(baudRate){
    case 110:
      speed=CBR_110;break;
    case 300:
      speed=CBR_300;break;
    case 600:
      speed=CBR_600;break;
    case 1200:
      speed=CBR_1200;break;
    case 2400:
      speed=CBR_2400;break;
    case 4800:
      speed=CBR_4800;break;
    case 9600:
      speed=CBR_9600;break;
    case 14400:
      speed=CBR_14400;break;
    case 19200:
      speed=CBR_19200;break;
    case 38400:
      speed=CBR_38400;break;
    case 56000:
      speed=CBR_56000;break;
    case 57600:
      speed=CBR_57600;break;
    case 115200:
      speed=CBR_115200;break;
    case 128000:
      speed=CBR_128000;break;
    case 256000:
      speed=CBR_256000;break;
    default:
      throw exc::InvalidOperation("Invalid baud rate"+toString(baudRate));
    };

    switch(parityBit){
    case NO_PARITY:
      parity=NOPARITY;
      break;
    case ODD_PARITY:
      parity=ODDPARITY;
      break;
    case EVEN_PARITY:
      parity=EVEN_PARITY;
      break;
    case MARK_PARITY:
      parity=MARKPARITY;
      break;
    case SPACE_PARITY:
      parity=SPACEPARITY;
      break;
    default:
      throw exc::InvalidOperation("Invalid parity"+toString(parityBit));      
    }

    switch(stopBits){
    case ONE_SB:
      stop=ONESTOPBIT;
      break;
    case TWO_SB:
      stop=TWOSTOPBITS;
      break;
    case ONEHALF_SB:
      stop=ONE5STOPBITS;
      break;
    default:
      throw exc::InvalidOperation("Invalid stopBits"+toString(stopBits));      
    }
    
    stDcb.BaudRate = speed;
    stDcb.fBinary = 1;
    stDcb.fParity = 0;
    stDcb.fOutxCtsFlow = hardwareControl==CTS_CONTROL;
    stDcb.fOutxDsrFlow = 0;
    stDcb.fDtrControl = DTR_CONTROL_HANDSHAKE; /*DTR_CONTROL_ENABLE;*/
    stDcb.fDsrSensitivity = 0;
    stDcb.fTXContinueOnXoff = 0;
    stDcb.fOutX = 0;
    stDcb.fInX = 0;
    stDcb.fErrorChar = 0;
    stDcb.fNull = 0;
    stDcb.fRtsControl = RTS_CONTROL_ENABLE;
    stDcb.fAbortOnError = 0;
    stDcb.ByteSize = dataBits;
    stDcb.Parity = parity;
    stDcb.StopBits = stop;
    stDcb.EofChar = 0;
    if(SetCommState(h.handle, &stDcb) == FALSE){
      throw exc::SerialPortException(portname,"setting attribs failed");
    }
    COMMTIMEOUTS cto;
    cto.ReadIntervalTimeout         = timeout;  /* msec */
    cto.ReadTotalTimeoutMultiplier  = timeout;  /* msec */
    cto.ReadTotalTimeoutConstant    = timeout;  /* msec */
    cto.WriteTotalTimeoutMultiplier = 0;    /* not used */
    cto.WriteTotalTimeoutConstant   = 0;    /* not used */

    if(SetCommTimeouts(h.handle, &cto) == FALSE) {
      throw exc::SerialPortException(portname,"setting timeouts failed");
    }

  }


 
  Serial::Serial(const char *pportname, int baudRate, int timeout,int dataBits,StopBit stopBits,Parity parityBit, HardwareControl hardwareControl):h(*new Handle()),portname(pportname){
    h.handle=(HANDLE)-1;
    try{
      
      char name[256];
      sprintf(name, "\\\\.\\%s", portname);
      h.handle=CreateFileA(name,
			   GENERIC_READ | GENERIC_WRITE,
			   0,//exclusive access
			   NULL,// no security attrs
			   OPEN_EXISTING,
			   FILE_ATTRIBUTE_NORMAL |FILE_FLAG_WRITE_THROUGH, //addFILE_FLAG_OVERLAPPED for asynchronous 
			   NULL);
      if (h.handle==(HANDLE)-1){
	throw exc::SerialPortException(portname,"opening failed");
      }
      setOptions(baudRate,timeout,dataBits,stopBits,parityBit,hardwareControl);
    }catch(exc::Exception& ){
      if (h.handle!=(HANDLE)-1){
	CloseHandle(h.handle);
      }
      delete &h;
      throw;
      
    }//catch
    
    
  }//Serial

  Serial::~Serial(){
    CloseHandle(h.handle);
    delete &h;
    //no need to the delete the reference, which is copied
  }

  void Serial::flush(){
    DWORD bytes;
	static char buff[256];
    while(true) {      
      PeekNamedPipe(h.handle, NULL, 0, NULL, &bytes, NULL);
      if(bytes==0) break;
      ReadFile( h.handle, buff, 256, &bytes, NULL);
    }//while
  }

  void Serial::send(const unsigned char* buff,int size){
    DWORD written;
    if (WriteFile(h.handle, (char*) buff, size, &written, NULL) == FALSE){
      throw exc::SerialPortWriteException(portname,size);
    }
    if (written!=size){
      throw exc::SerialPortWriteException(portname,size);
    }
  }

  int Serial::recv(unsigned char* data,int size){
    DWORD read;
    if (ReadFile(h.handle,data,size,&read,NULL)==FALSE){
      throw exc::SerialPortReadException(portname,size);
    }
    return read;
  }

  //_WIN32
#endif

  void Serial::recvAll(unsigned char* data,int size,Time additionalTime){
    //Time z=getSystemMillis();
    int n=recv(data,size);
    Time now=getSystemMillis();


    /*std::cout<<getElapsedMillis(z)<<" buffer is "<<n<<"("<<size<<")"<<std::hex;
    for (int i=0;i<n;i++){
      std::cout<<" "<<(int)data[i];
    }
    std::cout<<std::dec<<std::endl;
    */

    //std::cout<<"now n is"<<n<<std::endl;
    while (n<size&&(getSystemMillis()<now+additionalTime)){
      n=n+recv(&data[n],size-n);

      /*std::cout<<now<<"upbuffer is "<<n<<"("<<size<<")"<<std::hex;
      for (int i=0;i<n;i++){
	std::cout<<" "<<(int)data[i];
      }
      std::cout<<std::dec<<std::endl;
      */
    }
    if (n!=size){
      //std::cout<<n<<" of "<<size<<std::endl;
      //throw 7;
      throw exc::SerialPortReadTimeoutException(portname,size,additionalTime);
    }
  }

  int Serial::recvUpToTerminator(unsigned char* data,unsigned char terminator,Time additionalTime){
    unsigned char* p=data;
    do{
      recv(p,1); //this already throw exceptions
      ++p;
    }while(*p!=terminator);
    return p-data;
    
  }

  /*void Serial::sendAll(unsigned char* data,int size){
    int s=0;
    while(s<size){
      s=send(data+s,size-s);
    }
    }*/


}
