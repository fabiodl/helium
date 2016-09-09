/*!
 * \brief   slayer
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#ifndef XPLAYER_HWCOMPONENT
#define XPLAYER_HWCOMPONENT

#include <helium/system/cell.h>
#include <helium/system/cellDescription.h>
namespace helium{

typedef std::string HwFactoryName;
  typedef std::string HwComponentName;
  
  class HwFactoryInterface{
    HwFactoryName type;
  public:
    HwFactoryInterface(const HwFactoryName& name):type(name){
    }
    virtual ~HwFactoryInterface(){}
 const HwFactoryName& getType(){
      return type;
    }        
    virtual void create(Memory& m,const HwComponentName& name,const IDPath& confPath,const IDPath& servicePath)=0;
    virtual bool canBeDestroyed()=0;
   
  };



  //Compoment classes should have the constructor
  //Component(mem,name,confPath)
  //and include the method
  //bool isTemporary();

  template<typename Component>
  class HwFactory:public HwFactoryInterface{
    std::vector<Component* > comps;    
    std::vector<Component* > tempComps;    

    void create(Memory& m,const HwComponentName& name,const IDPath& confPath,const IDPath& servicePath);
  public:    
    HwFactory(const HwFactoryName& name);
    bool canBeDestroyed();
    ~HwFactory();
  };

  



  
  class HwComponent{
  public:
    HwComponentName name;
    HwFactoryName type;
    IDPath configPath,servicePath;
    enum Level{DISABLED,OPTIONAL,REQUIRED} level;
    std::string reminder;
  };
  
  typedef std::vector<HwComponent> HwCmpList;
  
  
  template<>  struct EnumStrings<HwComponent::Level>{
    static const int NUM=3;
    static const char* strings[3];       
  };


  template<>
  class DefDescr<HwComponent>:public Description<HwComponent>{
  public:    
    DefDescr(HwComponent& s):Description<HwComponent>(s){}
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);
  };
  
  template<>
  class DefDescr<HwCmpList>:public Description<HwCmpList>{
  public:
    DefDescr(HwCmpList& s):Description<HwCmpList>(s){}
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &members);    
  };
  
}

#include "hwComponent.hpp"

#endif
