#ifndef HE_MPC23017
#define HE_MPC23017
#include <stdint.h>

namespace helium{


  template <typename I2C>
  class MPC23017{
  public:
    enum AddrTable{
      DEVICEADDR=0x20, //start address,      
    };

    
    enum Name{
      A,B
    };

    enum Direction{IN,OUT,ALLIN=0,ALLOUT=0xFF};

      
    MPC23017(I2C& pi2c);

    void setDirection(uint16_t dir);
    void setDirection(Name p,unsigned char mask);
    void setDirection(Name p,int bit,bool value);    


    void setOutput(uint16_t dir);
    void setOutput(Name p,unsigned char val);
    void setOutput(Name p,int bit,bool x);
    
    void setPullup(uint16_t dir);
    void setPullup(Name p,unsigned char val);
    void setPullup(Name p,int bit,bool x);



    uint16_t read();
    unsigned char read(Name p);
    bool read(Name p,int bit);


  private:
    
    typedef unsigned char Cache[2];
    
    void setRegister(Cache& c,unsigned char regbase,uint16_t val);
    void setRegister(Cache& c,unsigned char regbase,unsigned char offset,unsigned char val);
    void setRegister(Cache& c,unsigned char regbase,unsigned char offset,unsigned char bit,bool val);
    
    I2C& i2c;    
    Cache ddr,port,pullup;

    class paired{
    public:
      enum Regs{
	IODIRA,
	IODIRB,
	IPOLA,
	IPOLB,
	GPINTENA,
	GPINTENB,
	DEFVALA,
	DEFVALB,
	INTCONA,
	INTCONB,
	IOCON,
	IOCON2,
	GPPUA,
	GPPUB,
	INTFA,
	INTFB,
	INTCAPA,
	INTCAPB,
	GPIOA,
	GPIOB,
	OLATA,
	OLATB,
      };
      
      };

    



  };



}


#include "mpc23017.hpp"

#endif
