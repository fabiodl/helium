#include "compactStructure.h"


namespace helium{


  const char* EnumStrings<Shape>::strings[]={"box","sphere","capsule"};
  const char* EnumStrings<ConnType>::strings[]={"hinge","servo","fixed"};
  const char* EnumStrings<Bool>::strings[]={"false","true"};

void CompactStructure::onInitializationFinished(Memory& mem){
  int ms=0;
  int fs=0;
  for (size_t e=0;e<elements.size();e++){
    if (elements[e].appe!=""){
      elements[e].app=static_cast< AppearanceW*>(  
						 mem.downPath(IDPath("robot.structure.appearances.appearance."+elements[e].appe,"."))
						 ->getData().getProperties()->directValuePointer());	  
     
    }//app
    
    if (elements[e].hasMouseSensitivity.val){	
      ms+=6;	
    }
    if (elements[e].hasForceSensor.val){	
	fs+=6;	
    }      
  }//for e
  mouseSensors.resize(ms);
  forceAppPoint.resize(fs);
  forceVector.resize(fs);
  force.resize(fs);
  withGround.resize(fs);
  ms=fs=0;
  
  for (size_t e=0;e<elements.size();e++){
    if (elements[e].hasMouseSensitivity.val){	
      elements[e].setElementCustomization(new MouseSensorBoxCustomization(ms,&mouseSensors[ms]),MOUSESENSORBOXCUSTOMIZATION);
      ms+=6;
      }
    
    if (elements[e].hasForceSensor.val){	
      elements[e].setElementCustomization(new ForceSensorBoxCustomization(fs,
									  &forceAppPoint[fs], 
									  &forceVector[fs],
									  &force[fs],
									  &withGround[fs]
									  ),FORCESENSORBOXCUSTOMIZATION);	
	fs+=6;
    }
    
  }//for e
  
  for (size_t p=0;p<parts.size();p++){
    for (size_t e=0;e<parts[p].elementsd.size();e++){
      parts[p].elements.push_back(static_cast<ElementW*> (mem.downPath(IDPath("robot.structure.elements.element."+parts[p].elementsd[e],"."))
							  ->getData().getProperties()->directValuePointer()));	  
      
				    
    }
    std::cout<<" part "<<p<<" has "<<parts[p].elements.size()<<" elements of"<< parts[p].elementsd.size()<<std::endl;
    
    
  }//for p
  
  int mid=0;
  for (size_t s=0;s<nodes.size();s++){
    skeletonNodes.push_back(&nodes[s]);
    mid=(std::max)(mid,nodes[s].motId);
    
    
    if (nodes[s].partd.length()>0){
	nodes[s].part=static_cast<PartW*>(
					  mem.downPath(IDPath("robot.structure.parts.part."+nodes[s].partd,"."))
					  ->getData().getProperties()->directValuePointer());	        	
    }
    
    nodes[s].ref=nodes[s].parent=="-1"?NULL:
      static_cast<SkeletonNodeW*>(mem.downPath(IDPath("robot.structure.joints.joint."+nodes[s].parent,"."))
				  ->getData().getProperties()->directValuePointer());
    
    }//for s
  motors.resize(mid+1);
  for (size_t s=0;s<nodes.size();s++){
    double* m=nodes[s].motId==-1?NULL:&motors[nodes[s].motId];
    switch(nodes[s].type){
    case FREEHINGE:
      nodes[s].fakeServo=m;
      break;
    case SERVOMOTOR:
      nodes[s].servoValue=m;
      break;
    case REFERENCEROTATION:
      if (m!=NULL) throw exc::InvalidOperation("motor ID specified for REFERENCE rotation");
      break;

    }
  }
  
  
  pivot=static_cast<PartW*>(mem.downPath(IDPath("robot.structure.parts.part."+pivotd,"."))
			    ->getData().getProperties()->directValuePointer());
  
  for (size_t i=0;i<noColls.size();i++){
      PartW* p1=static_cast<PartW*>(
				    mem.downPath(IDPath("robot.structure.parts.part."+noColls[i][0],"."))
				    ->getData().getProperties()->directValuePointer());
      
      PartW* p2=static_cast<PartW*>(
				    mem.downPath(IDPath("robot.structure.parts.part."+noColls[i][1],"."))
				    ->getData().getProperties()->directValuePointer());
      disableCollDet(p1,p2);
      
  }
  setAll(motors,0);
  setAll(mouseSensors,0);
  Vector3d v0(0,0,0);
  setAll(forceAppPoint,v0);
  setAll(forceVector,v0);
  setAll(force,0);
  setAll(withGround,false);
}//onInitFinished
}//helium
