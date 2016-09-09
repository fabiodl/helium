/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/robot/config.h>
#include <helium/robot/robotProtocol.h>
#include <helium/system/cellDescription.h>

namespace helium{
  void Config::ConfigElement::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    members.push_back(scalarMapping(filename,"filename"));
  }
  
  Config::ConfigElement::ConfigElement():loaded(false){
    
  }
  
  /*Config::ConfigElement::~ConfigElement(){
    if (xml!=NULL){
      delete xml;
    }
    }*/
  
  void Config::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members){
    hopname="config";
    members.push_back(mapMapping(confs,"configfile"));
  }

  void Config::onInitializationFinished(Memory& mem){
    int imported=0;
    for (std::map<std::string,ConfigElement>::iterator it=confs.begin();it!=confs.end();it++){
      if (!it->second.loaded){
	xml.loadData(mem,it->second.filename);
	it->second.loaded=true;
	std::cout<<"Loading "<<it->second.filename<<std::endl;
	//it->second.xml=new XmlLoader(it->second.filename);
	//it->second.xml->exportRef(mem);
	imported++;
      }
    }
    if (imported){
      import(mem);
    }    
  }//onInitFinished

  Config::Config(){}
  
  void Config::load(Memory& mem){
    mem.assurePath(MAINCONFIGFILEFULLLOCATION)->setData(mem.downPath(MAINCONFIGFILELOCATION)->getDataP());
    //std::cout<<mem<<std::endl;
    import(mem);        
  }


  Config::Config(Memory & mem,const std::string& mainFilename){
    recursiveRefToMem<NOFLAGS>(mainFilename,mem,MAINCONFIGFILELOCATION);
    load(mem);
  }

  Config::Config(Memory & mem){
    load(mem);
  }

  const std::string& Config::operator[](const std::string& s){
    std::map<std::string,ConfigElement>::iterator it;
    if ((it=confs.find(s))==confs.end()){
      throw exc::MissingData(s);
    }
    return it->second.filename;
  }
}//helium
