/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef HE_CONFIG
#define HE_CONFIG
#include <helium/system/cellMapping.h>
#include <helium/xml/xmlLoader.h>
namespace helium{

  class Config:public ClassMapping{
    XmlLoader xml;
  public:
    struct ConfigElement:public ClassMapping{
      std::string filename;
      bool loaded;
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
      ConfigElement();
      //~ConfigElement();    
    };//ConfigElement
    

  private:
    std::map<std::string,ConfigElement> confs;
    
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
    
    void onInitializationFinished(Memory& mem);
    
    
  public:    
    Config(Memory & pmem);      
    Config(Memory & mem,const std::string& mainFilename);
    Config();

    void load(Memory & pmem);      
    const std::string& operator[](const std::string& s);
    
  };//configSet


  inline std::ostream& operator<<(std::ostream& o, const Config::ConfigElement& c){
    return o<<c.filename;
  }

}//helium
#endif
