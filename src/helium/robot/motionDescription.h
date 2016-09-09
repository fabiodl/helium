#ifndef HE_MOTIONDESCRIPTION
#define HE_MOTIONDESCRIPTION

#include <helium/robot/motion.h>
#include <helium/system/cellDescription.h>

namespace helium{



  class PostDesc{
    Array<double> pos;
    int link;
    PostDesc():link(-1){}
  };

  template<> class DefDescr<PostDesc>:public Description<PostDesc>{
    
    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){
      m.push_back(fscalarMapping<SILENTFAIL>(obj.pos),"value");
      m.push_back(fscalarMapping<SILENTFAIL>(obj.pos),"link");
    }

    void onInitializationFinished(Memory& mem){
      if (obj.pos.length()==0&&obj.link==-1){
	throw exc::BadFileFormat("posture is nether a link nor specifies a set of angles");
      }
    }
  public:
    DefDescr<PostDesc>(PostDesc& pd):Description<PostDesc>(pd){
    }
    
  };


  template<> class DefDescr<Motion>:public Description<Motion>{


    std::map<PostDesc> postures;

    void getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){      
      m.push_back(mapMapping(postrures),IDPath("motion.postures.posture.time"));      
    }


  public:    
    DefDescr<CompleteRobot>(CompleteRobot& cr):Description<Motion>(cr){
    };

    
    
  };





}




#endif
