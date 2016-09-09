#include <helium/math/bit.h>
namespace helium{
  
  template <typename I2C>
  MPC23017<I2C>::MPC23017(I2C& pi2c):i2c(pi2c){
    ddr[0]=0xFF;//all in
    ddr[1]=0xFF;
    port[0]=0;
    port[1]=0;
    pullup[0]=0;
    pullup[1]=0;
  }


  template <typename I2C>
  void MPC23017<I2C>::setRegister(Cache& c,unsigned char regbase,uint16_t val){
    unsigned char buff[3];
    c[0]=getByte<0>(val);
    c[1]=getByte<1>(val);
    buff[0]=regbase;
    buff[1]=c[0];
    buff[2]=c[1];
    
    i2c.write(buff,3);
  }

  template <typename I2C>
  void MPC23017<I2C>::setRegister(Cache& c,unsigned char regbase,unsigned char offset,unsigned char val){
    unsigned char buff[2];
    c[offset]=val;
    buff[0]=regbase+offset;
    buff[1]=val;
    
    i2c.write(buff,2);
  }

    template <typename I2C>
    void MPC23017<I2C>::setRegister(Cache& c,unsigned char regbase,unsigned char offset,unsigned char bit,bool val){
    unsigned char buff[2];
    setBit(c[offset],bit,val);
    buff[0]=regbase+offset;
    buff[1]=c[offset];
    
    i2c.write(buff,2);
  }




  template <typename I2C>
  void MPC23017<I2C>::setDirection(uint16_t dir){
    setRegister(ddr,paired::IODIRA,~dir);
  }

  template <typename I2C>
  void MPC23017<I2C>::setDirection(MPC23017::Name p,unsigned char dir){
    setRegister(ddr,paired::IODIRA,p,~dir);
  }

  template <typename I2C>
  void MPC23017<I2C>::setDirection(MPC23017::Name p,int bit,bool value){
    setRegister(ddr,paired::IODIRA,p,bit,!value);
  }


  template <typename I2C>
  void MPC23017<I2C>::setOutput(uint16_t val){
    setRegister(port,paired::OLATA,val);
  }

  template <typename I2C>
  void MPC23017<I2C>::setOutput(Name p,unsigned char val){
    setRegister(port,paired::OLATA,p,val);
  }

  template <typename I2C>
  void MPC23017<I2C>::setOutput(Name p,int bit,bool val){
    setRegister(port,paired::OLATA,p,bit,val);
  }


  template <typename I2C>
  void MPC23017<I2C>::setPullup(uint16_t val){
    setRegister(pullup,paired::GPPUA,val);
  }

  template <typename I2C>
  void MPC23017<I2C>::setPullup(Name p,unsigned char val){
    setRegister(pullup,paired::GPPUA,p,val);
  }

  template <typename I2C>
  void MPC23017<I2C>::setPullup(Name p,int bit,bool val){
    setRegister(pullup,paired::GPPUA,p,bit,val);
  }
  
  template <typename I2C>
  uint16_t MPC23017<I2C>::read(){
    unsigned char buffer[2];
    buffer[0]=paired::GPIOA;    
    i2c.write(buffer,1);
    i2c.read(buffer,2);
    uint16_t r=buffer[0];
    r+=buffer[1]<<8;
    return r;
  }
  
  template <typename I2C>
  unsigned char MPC23017<I2C>::read(Name p){
    unsigned char buffer;
    buffer=paired::GPIOA+p;
    i2c.write(&buffer,1);
    i2c.read(&buffer,1);
    return buffer;    
  }

  template <typename I2C>
  bool MPC23017<I2C>::read(Name p,int bit){
    unsigned char buffer;
    buffer=paired::GPIOA+p;
    i2c.write(&buffer,1);
    i2c.read(&buffer,1);
    return (buffer&(1<<bit));    
  }

  
}
