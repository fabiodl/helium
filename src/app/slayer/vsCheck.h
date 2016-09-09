/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef XPLAYER_VSCHECK
#define XPLAYRR_VSCHECK

#include <helium/vstone/vstoneSerial.h>
#include <helium/vstone/vsCrcSerial.h>


namespace helium{


  template<bool hasCrc> struct VsSerialType;
  

  template<> 
  struct VsSerialType<false>
  {
    typedef VstoneSerial value;
  };


  template<> 
  struct VsSerialType<true>
  {
    typedef VsCrcSerial<Serial> value;
  };

  

  template<bool hasCrc>
  class VsCheck{
  public:
    int devid;
    int productNumber,systemVersion;
    static const int TIMEOUT=300;    
    static const int BAUDRATE=115200;
    
    VsCheck():
      devid(-1),
      productNumber(-1),
      systemVersion(-1)
    {
    }

    void check(const std::string &name){      
      std::cout<<"testing serial "<<name<<std::endl;
      Serial s(name.c_str(),BAUDRATE,TIMEOUT);
      typename VsSerialType<hasCrc>::value vserial(s);
      unsigned char buffer[4];
      std::cout<<"could open"<<std::endl;
      vserial.recv(devid,0,buffer,4);
      std::cout<<"could receive"<<std::endl;

      if ( ((productNumber!=-1)&&(productNumber!=from14(buffer)))
	   ||
	   ((systemVersion!=-1)&&(systemVersion!=from14(buffer+2)))
	   )
	{
	  std::stringstream expected;
	  expected<<"Device 0x"<<std::hex<<devid<<std::dec<<" ";
	  if (productNumber!=-1){
	    expected<<"SYS_PN="<<productNumber<<" ";
	  }
	  if (systemVersion!=-1){
	    expected<<"SYS_VER="<<systemVersion;
	  }
	  throw exc::HardwareMismatch(expected.str(),stringBegin()<<"SYS_PN="<<productNumber<<" SYS_VER="<<systemVersion<<stringEnd());
	}


    }


  };
    
  
  template<bool hasCrc> 
  class DefDescr<VsCheck<hasCrc>  >:public Description<VsCheck<hasCrc> >{
  public:
    DefDescr(VsCheck<hasCrc>& p):Description<VsCheck<hasCrc> >(p){
    }
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
      members.push_back(scalarMapping(this->obj.devid,"deviceId"));
      members.push_back(fscalarMapping<SILENTFAIL>(this->obj.devid,"productNumber"));
      members.push_back(fscalarMapping<SILENTFAIL>(this->obj.devid,"systemVersion"));
    }
  };


}


#endif
