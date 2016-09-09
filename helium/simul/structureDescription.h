#ifndef HE_COMPACTSTUCTURE
#define HE_COMPACTSTUCTURE

#include <helium/simul/completeRobot.h>
#include <helium/system/cellDescription.h>
#include <helium/graphics/ppm.h>
#include <helium/vector/array.h>


namespace helium{


  template<> class EnumStrings<ConnectionType>{
  public:
    static const int NUM=4;
    static const char* strings[NUM];     
  };

  template<> class EnumStrings<ElementShape>{
  public:
    static const int NUM=3;
    static const char* strings[NUM];     
  };


  struct ColorAdapter{
    float &r,&g,&b;
    ColorAdapter(Appearance& a):r(a.r),g(a.g),b(a.b){
    }

    ColorAdapter& operator=(const ColorAdapter& c){
      r=c.r;
      g=c.g;
      b=c.b;
      return *this;
    }
  };

   inline std::ostream& operator<<(std::ostream& o,const ColorAdapter& a){
    return o<<" "<<a.r<<" "<<a.g<<" "<<a.b;
  }

  inline std::istream& operator>>(std::istream& i,ColorAdapter& a){
    return i>>a.r>>a.g>>a.b;
  }

 

  template<>
  class DefDescr<Appearance>:public Description<Appearance>{ 
    ColorAdapter col;
    std::string filename;
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m);	
    void onInitializationFinished(Memory& mem);
    
 public:
    DefDescr(Appearance& a);    
};




  template<>
  class DefDescr<Element>:public Description<Element>{
    friend class DefDescr<CompleteRobot>;
    bool hasMass,hasShape;
    bool hasMouseSensitivity,hasForceSensor;   
    Vector3d pos;
    Array<double,16> roto;
    int appId;
  
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m);
    void onInitializationFinished(Memory& mem);

public:
    DefDescr(Element& e);
  };



 template<>
 class DefDescr<Part>:public Description<Part>{
   friend class DefDescr<CompleteRobot>;
   Array<int> elementsId;
   
   void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m);

   
 public:
   DefDescr<Part>(Part& p);

    
  };

  /*inline std::ostream& operator<<(std::ostream& o,const Part& pw){
    return o<<"PartW "<<pw.id;
  };

  inline std::istream& operator>>(std::istream& i,PartW ew){
    return i>>ew.elementsd;

    };*/

template<>
class DefDescr<SkeletonNode>:public Description<SkeletonNode>{
  friend class DefDescr<CompleteRobot>;

  int parentId;
  int partId;
  void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m);

  void onInitializationFinished(Memory& mem);

public:
  DefDescr(SkeletonNode& s);

 
};


  /*inline std::ostream& operator<<(std::ostream& o,const SkeletonNodeW ew){
    return o<<"SkeketonNode "<<ew.id;
  };

  inline std::istream& operator>>(std::istream& i,SkeletonNodeW ew){
    throw exc::InvalidOperation("deserialize on a SkeletonNode");
    return i;
    };*/


  template<>
  class DefDescr<Robot>:public Description<Robot>{
    friend class DefDescr<CompleteRobot>;
    Array<std::pair<int,int> > noCollsId;
    int pivotId;
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m);

  public:
    DefDescr(Robot& r);
  };

 

  template<>
  class  DefDescr<CompleteRobot>:public Description<CompleteRobot>{


    VectorDesc<Array<Appearance> >  appearances;
    VectorDesc<Array<Element> >  elements;
    VectorDesc<Array<Part> >  parts;
    VectorDesc<Array<SkeletonNode> >  nodes;
    DefDescr<Robot> robot;
    std::vector<NodeMapping*> *members;

    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m);
    void onInitializationFinished(Memory& mem);

  public:

    DefDescr<CompleteRobot>(CompleteRobot& cr);    
    
  };//DefDescr
 

  

}//helium
#endif

