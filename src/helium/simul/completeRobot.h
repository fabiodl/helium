#ifndef HE_COMPLETEROBOT
#define HE_COMPLETEROBOT

#include <helium/simul/structure.h>
#include <helium/system/cellDescription.h>
#include <helium/graphics/glTexture.h>

namespace helium{



  class CompleteRobot:public Robot{
    friend class DefDescr<CompleteRobot>;
  public:
    Array<Appearance> appearances;
    Array<Element> elements;
    Array<Part> parts;
    
  private:
    class MSBInitFunc{
      CompleteRobot& owner;
    public:      
      MSBInitFunc(CompleteRobot& powner):owner(powner){
      }

      void init(MouseSensorBoxCustomization* m,int idx){
	idx*=6;
	new (m) MouseSensorBoxCustomization(idx,owner.mouseSensors.data+idx);
      }
    } mouseCustomInit;

    

    class FSBInitFunc{
      CompleteRobot& owner;
    public:
      FSBInitFunc(CompleteRobot& powner):owner(powner)
      {
      }
      void init( ForceSensorBoxCustomization* f,int idx){
	idx=idx*6;
	new (f) ForceSensorBoxCustomization(idx,
					    owner.forceAppPoint.data+idx,
					    owner.forceVector.data+idx,
					    owner.force.data+idx,
					    owner.withGround.data+idx
					    );
      }
      
    } forceCustomInit;


    Array<MouseSensorBoxCustomization,
	  array::RESIZABLE,
	  array::InternalAllocator<
	    ClassPlacerAllocator<MouseSensorBoxCustomization,MSBInitFunc,&MSBInitFunc::init>
	    >
	  > mouseCustomizations;


      Array<ForceSensorBoxCustomization,
	  array::RESIZABLE,
	  array::InternalAllocator<
	    ClassPlacerAllocator<ForceSensorBoxCustomization,FSBInitFunc,&FSBInitFunc::init>
	    >
	    > forceCustomizations;

    
  public:
    Array<SkeletonNode> nodes;
    Array<double> mouseSensors;
    Array<Vector3d> forceAppPoint;
    Array<Vector3d> forceVector;
    Array<double> force;
    Array<bool> withGround;

    Array<double> motors;
    



    CompleteRobot():mouseCustomInit(*this),
		    forceCustomInit(*this),
		    mouseCustomizations(&mouseCustomInit,0),
		    forceCustomizations(&forceCustomInit,0)
		    
    {        
    }


    ~CompleteRobot(){
      for (size_t a=0;a<appearances.size();a++){
	delete appearances[a].texture;
      }
      for (size_t e=0;e<elements.size();e++){
	delete elements[e].size;
      }
      
    }
   
  };

}
#endif
