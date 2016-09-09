/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/vstone/serialProbe.h>
#include <helium/vector/array.h>
#include <helium/util/ioUtil.h>
#include <helium/util/mout.h>
#include <helium/core/exception.h>
#include <helium/debug/trace.h>

#include <helium/vstone/vstoneSerial.h>
#include <helium/vstone/vsCrcSerial.h>

static const int TIMEOUT=30;
namespace helium{
  SerialProbe::Prober* SerialProbe::getProber(const std::string& s){
    for (size_t j=0;j<probers.size();j++){
      if (probers[j]->name==s){
	return probers[j];
      }
    }
    return NULL;
  }

  void SerialProbe::fillOnTest(){
    Target t=TALL;
    Prober* sp;
    for (size_t i=0;i<probe.size();i++){
      if (probe[i]=="all"){
	t=TALL;
	continue;
      }else{
	Prober* nsp=getProber(probe[i]);
	if (nsp){
	  sp=nsp;
	  t=TSPECIFIC;
	  continue;
	}	
      }      
      if (t==TALL){
	for (size_t j=0;j<probers.size();j++){	  
	  onTest[probe[i]].push_back(probers[j]);
	}
      }else{
	onTest[probe[i]].push_back(sp);
      }      
    }
  }//fillOnTest
    void SerialProbe::setSpecified(){
    if (serial.size()%2==1){
      throw exc::InvalidDataValue("serial list","size is not even. Format [target1 serial1 .. targetn serialn]");
    }//if
    for(size_t i=0;i<serial.size();i=i+2){
      Prober* p=getProber(serial[i]);
      if (!p){
	throw exc::InvalidDataValue("serial peripheral",serial[i]+" is not a valid identifier");
      }//if !p
      p->serial.reset(new Serial(serial[i+1].c_str(),BAUDRATE,TIMEOUT));
      onTest[serial[i+1].c_str()].clear();
    }//for
  }//setSpecified


  void SerialProbe::removeProber(Prober* p){
    for (std::map<std::string,std::vector<Prober*> >::iterator it=onTest.begin();
	 it!=onTest.end();
	 ++it){
      std::vector<Prober*>& v=it->second;
      for (std::vector<Prober*>::iterator i=v.begin();i!=v.end();){
	if (*i==p){
	  i=v.erase(i);
	}else{
	  ++i;
	}
      }
    }
  }

    void SerialProbe::execProbe(){
#ifdef TRACEDEBUG
      exc::ExceptionsAreSilent=true;
#endif

    fillOnTest();
    setSpecified();
    for (std::map<std::string,std::vector<Prober*> >::iterator it=onTest.begin();
	 it!=onTest.end();
	 ++it){
      if (it->second.size()>0){
	mout(msg::OK)("Analyze "+it->first+"with "+toString(it->second.size())+" probers");
	try{
	  std::auto_ptr<Serial> s(new Serial(it->first.c_str(),BAUDRATE,TIMEOUT));  
	  for (size_t i=0;i<it->second.size();i++){
	    mout(msg::OK,1)("Probing "+it->first+" as "+it->second[i]->name);
	    if (it->second[i]->probe(s.get())){	 
	      mout(msg::PINFO,2)("success");
	      it->second[i]->serial=s;
	      removeProber(it->second[i]);
	      break;
	    }	  
	  }//for
	}catch(exc::SerialPortException& ){
	  mout(msg::NINFO)("Error while opening port "+it->first);
	}
      }//if vect.size>0
    }//for it
#ifdef TRACEDEBUG
    exc::ExceptionsAreSilent=false;
#endif
  }//execProbe


    void SerialProbe::addProber(Prober& p){
      probers.push_back(&p);
    }

 


   SerialProbe::SerialProbe(const std::string& probeStrings,
			   const std::string& serialStrings){
    tokenize(probe,probeStrings);
    tokenize(serial,serialStrings);
  }
  


  VsIdProber::VsIdProber(const std::string& name,int pid,int paddr,int psize):
    Prober(name),
    id(pid),addr(paddr),size(psize){
  }
  
  bool VsIdProber::probe(Serial* s){
    VstoneSerial vserial(*s);
    Array<unsigned char> buffer(size+1);
    try{
      // std::cout<<"asking id"<<id<<"address "<<addr<<" size "<<size<<std::endl;
      vserial.recv(id,addr,buffer.data,size);
    }catch(exc::Exception& ){      
      return false;
    }
    return true;
  }
  
  VsCrcIdProber::VsCrcIdProber(const std::string& name,int pid,int paddr,int psize):Prober(name),
										    id(pid),addr(paddr),size(psize){
  }
  
  bool VsCrcIdProber::probe(Serial* s){
    VsCrcSerial<Serial> vserial(*s);
    Array<unsigned char> buffer(size+1);
    try{     
      vserial.recv(id,addr,buffer.data,size);
    }catch(exc::Exception& ){      
      return false;
    }
    return true;
  }
  

  

  
  



}
