#ifndef HE_COMPACTSTUCTURE
#define HE_COMPACTSTUCTURE

#include <robot/structure.h>
#include <graphics/glTexture.h>
#include <helium/vector/array.h>
#include <helium/system/cellMapping.h>
#include <helium/core/basicEnum.h>

namespace helium{
struct Shape{
  typedef enum {BOX,SPHERE,CAPPED,NUM} vals; 
    static const char* strings[];
};

struct ConnType{
  typedef enum {FREE,SERVO,REFERENCE,NUM} vals; 
    static const char* strings[];
};




struct TextureW:public ClassMapping{
  Texture* t;
  std::string filename;

  TextureW():t(NULL){
  }
  
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){
    m.push_back(scalarMapping(filename,"filename",true));
  }
  
  void onInitializationFinished(Memory& mem){
    if (filename!="") t=new Texture(filename.c_str());
  }
  
  

};


  struct RgbW{
    float r;
    float g;
    float b;
    RgbW():r(0.5),g(0.5),b(0.5){
    }
  };

  inline std::istream&  operator>>(std::istream& i,RgbW & c){
    return i>>c.r>>c.g>>c.b;
  }

  inline std::ostream&  operator<<(std::ostream& o,const RgbW & c){
    return o<<" "<<c.r<<" "<<c.g<<" "<<c.b;
  }



class AppearanceW:public ClassMapping,public Appearance{
  TextureW *t;
  
  
  

  RgbW* rgb;

  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){
    t=new TextureW();   
    rgb=new RgbW;    
    m.push_back(scalarMapping(*t,"texture",true));    
    m.push_back(scalarMapping(*rgb,"rgb",true));    
    m.push_back(scalarMapping(useTexture,"useTexture",true));    
  }

  void onInitializationFinished(Memory& mem){
    if (t->t!=NULL){
      texture=t->t;
    }
    r=rgb->r;
    g=rgb->g;
    b=rgb->b;
    delete t;
    delete rgb;    
  }
  
  
};


  inline std::ostream& operator<<(std::ostream& o,const AppearanceW& a){
    return o<<" "<<a.r<<" "<<a.g<<" "<<a.b;
  }

  inline std::istream& operator>>(std::istream& i,AppearanceW& a){
    return i>>a.r>>a.g>>a.b;
  }


class ElementW:public ClassMapping,public Element{

  Vector3d sized;
  Enum<Bool> hasMass,hasShape;
  Enum<Shape> shapee;
  Enum<Bool> hasMouseSensitivity,hasForceSensor;
  Vector3d pos;
  friend class CompactStructure;
  std::string appe;
  Array<double,16,array::LINK> roto;


 
  
  
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){
    m.push_back(scalarMapping(id,"id"));    
    size=&sized;
    m.push_back(scalarMapping(sized,"size"));    
    m.push_back(scalarMapping(hasMass,"hasMass"));    
    m.push_back(scalarMapping(hasShape,"hasGeometry"));  
    m.push_back(scalarMapping(Element::m,"mass",true));
    m.push_back(scalarMapping(shapee,"shape"));
    m.push_back(scalarMapping(hasMouseSensitivity,"hasMouseSensitivity",true));
    m.push_back(scalarMapping(hasForceSensor,"hasForceSensor",true));
    m.push_back(scalarMapping(pos,"position",true));
    m.push_back(scalarMapping(roto,"rototranslation",true));
    m.push_back(scalarMapping(appe,"appearance",true));
    
  }


  void onInitializationFinished(Memory& mem){
    type=(hasMass.val?WEIGHTINFO:0)+(hasShape.val?ENCUMBRANCEINFO:0);
    if (hasMass.val&&m==0){
      throw exc::MissingData("mass");
    }
    shape=(ElementShape)shapee.val;
    if (hasShape.val&& appe.length()==0){
      throw exc::MissingData("appearance");
    }
    if (!isnan(pos.x)){
      setTranslation(roto.data,&pos);
    }
    std::cout<<"type is "<<type<<std::endl;
  }

public:
  ElementW():hasMouseSensitivity(Bool::FALSE),hasForceSensor(Bool::FALSE),roto(rotoTransl){
  }


};

  inline std::ostream& operator<<(std::ostream& o,const ElementW ew){
    return o<<"Element "<<ew.id;
  };

  inline std::istream& operator>>(std::istream& i,ElementW ew){
    throw exc::InvalidOperation("deserialize on an Element");
    return i;
  };


  struct PartW:public Part,public ClassMapping{
    Array<std::string> elementsd;
     void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){
       m.push_back(scalarMapping(id,"id"));    
       m.push_back(scalarMapping(elementsd,"elements"));
     }

    void onInitializationFinished(Memory& mem){
      std::cout<<"the size is"<<elementsd.size()<<std::endl;
    }
  };

  inline std::ostream& operator<<(std::ostream& o,const PartW pw){
    return o<<"PartW "<<pw.id;
  };

  inline std::istream& operator>>(std::istream& i,PartW ew){
    return i>>ew.elementsd;

  };


class SkeletonNodeW:public SkeletonNode,public ClassMapping{
  Vector3d posd;
  Enum<ConnType> typed;
  Enum<Axis> axisd;
  friend class CompactStructure;
  std::string parent;
  std::string partd;
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){
    m.push_back(scalarMapping(id,"id"));    
    m.push_back(scalarMapping(motId,"motorID",true));    
    pos=&posd;
    m.push_back(scalarMapping(posd,"pos")); 
    m.push_back(scalarMapping(typed,"type")); 
    m.push_back(scalarMapping(axisd,"axis")); 
    m.push_back(scalarMapping(offset,"offset")); 
    m.push_back(scalarMapping(multiplier,"multiplier")); 
    m.push_back(scalarMapping(parent,"parent")); 
    m.push_back(scalarMapping(partd,"part",true)); 
  }

   void onInitializationFinished(Memory& mem){
     type=static_cast<ConnectionType>(typed.val);
     axis=static_cast<TurnAxis>(axisd.val);
     if (type==SERVOMOTOR&&motId==-1){
       throw exc::MissingData("motor ID for Node"+toString(id));
     }
   }

  
};


  inline std::ostream& operator<<(std::ostream& o,const SkeletonNodeW ew){
    return o<<"SkeketonNode "<<ew.id;
  };

  inline std::istream& operator>>(std::istream& i,SkeletonNodeW ew){
    throw exc::InvalidOperation("deserialize on a SkeletonNode");
    return i;
  };

  class CompactStructure:public ClassMapping,public Robot{
    Array<AppearanceW> apps;
    Array<ElementW> elements; 
    Array<PartW> parts;
    Array<SkeletonNodeW> nodes;
    Array<Array<std::string,2> > noColls;
    std::string pivotd;

    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){
      hopname=IDPath("robot.structure",".");
      m.push_back(fpsvectMapping<ATTACHELEMENT>(apps,"appearances","appearance"));
      m.push_back(fpsvectMapping<ATTACHELEMENT>(elements,"elements","element"));
      m.push_back(fpsvectMapping<ATTACHELEMENT>(parts,"parts","part"));
      m.push_back(fpsvectMapping<ATTACHELEMENT>(nodes,"joints","joint"));
      m.push_back(scalarMapping(pivotd,"pivot"));
      m.push_back(fscalarMapping<ATTACHELEMENT>(noColls,"noCollision"));
    }
  
  
    void onInitializationFinished(Memory& mem);
    


public:
  Array<double> mouseSensors;
  Array<Vector3d> forceAppPoint;
  Array<Vector3d> forceVector;
  Array<double> force;
  Array<bool> withGround;
  Array<double> motors;

  
};

}//helium
#endif
