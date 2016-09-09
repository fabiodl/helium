/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */


#include "slayer.h"

#include <helium/core/enum.h>
#include <helium/util/mout.h>
#include <helium/util/stringUtil.h>
#include <helium/system/cellServer.h>
#include <helium/system/keyboardCellServer.h>
#include <helium/signal/destroyableSignal.h>
using namespace helium;

namespace helium{    

  
  
  const char* EnumStrings<Slayer::State>::strings[4]={
    "?",
    "SKIP",
    "OK",
    "FAIL"
  };
  
}


void DefDescr<HwComponent>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
  members.push_back(scalarMapping(obj.name,"name"));
  members.push_back(scalarMapping(obj.type,"type"));
  members.push_back(scalarMapping(obj.level,"level"));		      
  members.push_back(scalarMapping(obj.configPath,"configPath"));		      
  members.push_back(scalarMapping(obj.servicePath,"servicePath"));
  members.push_back(fscalarMapping<SILENTFAIL>(obj.reminder,"reminder"));
}


void DefDescr<HwCmpList>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
  members.push_back(vectMapping(vd(obj),IDPath("components.component","."),"",IDPath("components.num",".")));
}




Slayer::FactoryMap::iterator Slayer::getFactory(const HwFactoryName& fn){
  FactoryMap::iterator it=factories.find(fn);
  if (it==factories.end()){
    std::stringstream ss("Unkown hardware type. Known types:");
      for (FactoryMap::iterator it=factories.begin();it!=factories.end();++it){
	ss<<" "<<it->first;
      }
      throw exc::InvalidDataValue("harware component :"+fn,ss.str());
  }
  return it;
}



void Slayer::printSummary(){
  
  size_t maxl=0;
  for (size_t i=0;i<components.size();i++){
    maxl=(std::max)(maxl,components[i].name.length());
  }
 


  std::cout<<std::endl<<std::endl;

  
  std::cout<<spaces(maxl+1+8+1+4,'-')<<std::endl;
  for (size_t i=0;i<components.size();i++){
    std::cout<<rightStringAlign(components[i].name,maxl);
    std::cout<<" "<<rightStringAlign(toString(components[i].level),8);
    std::cout<<" "<<toString(hwState[i]);
    if ((hwState[i]==FAILED)&&(components[i].reminder!="")){
      std::cout<<" "<<components[i].reminder;
    }
    
    std::cout<<std::endl;      
  }
  std::cout<<spaces(maxl+1+8+1+4,'-') <<std::endl;
  std::cout<<std::endl<<std::endl;
}




void Slayer::run(){  
  try{
    dimport(components,mem,IDPath("robot.hardware","."));
    hwState.resize(components.size());  
    setAll(hwState,Slayer::UNKNOWN);
    
    for (size_t i=0;i<components.size();i++){
      HwComponent& c=components[i]; 
      FactoryMap::iterator it=getFactory(c.type);
      if (c.level==HwComponent::DISABLED){
	hwState[i]=SKIP;
	continue;
      }    
      try{      
	it->second->create(mem,c.name,c.configPath,c.servicePath);
	hwState[i]=OK;
      }catch(exc::Exception& e){
	hwState[i]=FAILED;
	if (c.level==HwComponent::OPTIONAL){
	  std::cerr<<e<<std::endl;
	  std::cerr<<"Component was optional, continuing"<<std::endl;
	}else{
	  throw;
	}
      }    
    }
    printSummary();
    for (FactoryMap::iterator it=factories.begin();it!=factories.end();){
      if (it->second->canBeDestroyed()){
	FactoryMap::iterator toDel=it;
	++it;
	factories.erase(toDel);
      }else{
	++it;
      }      
    }

  }catch(exc::Exception& e){
    std::cerr<<"Slayer terminating for fatal error "<<e;
    printSummary();
    std::cout<<"Print tree? [Y/N]"<<std::endl;
    char c;
    std::cin>>c;
    if ((c=='y')||(c=='Y')){
      std::cerr<<*mem.downPath("tree");
    }
    throw;
  }  
  
  Semaphore termination;      
  int port=CELLPORT;
  fdimport<SILENTFAIL>(port,mem,IDPath("cellserver.port","."));

  CellServer tcp(port,mem);    
  KeyboardCellServer keyb(mem);
  keyb.start();
  DestroyableSignal<void>::SignalReference termRef(&termination);
  keyb.onStreamTermination.connect(termRef);
  termination.wait();
}




Slayer:: ~Slayer(){
  for (FactoryMap::iterator it=factories.begin();it!=factories.end();++it){
    delete it->second;
  }
}
