/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_SERIALPROBE
#define HE_SERIALPROBE


#include <helium/system/cellDescription.h>



namespace helium{



  //CheckCode is a class which 
  //- has a no param constructor
  //- has a description so that configs can be imported
  //- has a void check(const string& serialname) method 
  //                      that throws in case of failure


  template<typename CheckCode>
  class SerialProbe{
    
    class NotFoundException:public exc::Exception{        

     

    public:
      NotFoundException(const HwComponentName& name,const IDPath &out,const std::vector<std::string>& set):
	Exception(
		  stringBegin()<<
		  "Cannot initialize "<<out<<" for "<<name<<" with any of "<<toString(set)<<stringEnd())
      {      
      }
      
     virtual void rethrow(){
	throw *this;
      }
      
      
    Exception* clone() const{
      return new NotFoundException(*this);
    }
      
    };

  public:
    

    bool isTemporary(){
      return true;
    }
    
    SerialProbe(helium::Memory& mem,const HwComponentName& name,const helium::IDPath& confPath,const helium::IDPath& servicePath){
      std::vector<std::string> probeSet;
      IDPath setPath,outPath;
      dimport(setPath,mem,confPath+"setPath");
      dimport(outPath,mem,confPath+"outPath");
      dimport(probeSet,mem,setPath);
      CheckCode c;
      dimport(c,mem,confPath);            
      exc::TraceSilencer silence;
      for (size_t i=0;i<probeSet.size();i++){
	try{
	    c.check(probeSet[i]);
	    fdexport<USEDATACELL>(mem,probeSet[i],outPath);
	    mout(msg::PINFO)(name+": found at "+ probeSet[i]);
	    return ;
	}catch(exc::Exception& ){
	}
	//std::cout<<"trying next"<<std::endl;
      }
      
      throw NotFoundException(name,outPath,probeSet);
    }
    
  
};

}

#endif
