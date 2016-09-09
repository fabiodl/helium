#include <helium/simul/structureDescription.h>

namespace helium{


  const char* EnumStrings<ElementShape>::strings[]={"box","sphere","capsule"};
  const char* EnumStrings<ConnectionType>::strings[]={"hinge","slider","servo","fixed"};
  //const char* EnumStrings<Bool>::strings[]={"false","true"};


  
 
  void DefDescr<Appearance>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){   
      m.push_back(fscalarMapping<SILENTFAIL>(filename,IDPath("texture.filename",".")));    
      m.push_back(fscalarMapping<SILENTFAIL|AVOIDCELLTRANSFORM>(col,"rgb"));    
      m.push_back(fscalarMapping<SILENTFAIL>(obj.useTexture,"useTexture"));    
  }


  void DefDescr<Appearance>::onInitializationFinished(Memory& mem){
    if (filename!=""){
      obj.texture=new PpmTexture(filename.c_str());
    }    
  }

   DefDescr<Appearance>::DefDescr(Appearance& a):Description<Appearance>(a),
						col(a){}

  
  void DefDescr<Element>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){
      m.push_back(scalarMapping(obj.id,"id"));    
      Vector3d* size=new Vector3d();
      obj.size=size;
      m.push_back(scalarMapping(*size,"size"));    
      m.push_back(scalarMapping(hasMass,"hasMass"));    
      m.push_back(scalarMapping(hasShape,"hasGeometry"));  
      m.push_back(fscalarMapping<SILENTFAIL>(obj.m,"mass"));
      m.push_back(scalarMapping(obj.shape,"shape"));
      m.push_back(fscalarMapping<SILENTFAIL>(hasMouseSensitivity,"hasMouseSensitivity"));
      m.push_back(fscalarMapping<SILENTFAIL>(hasForceSensor,"hasForceSensor"));
      
      m.push_back(fscalarMapping<SILENTFAIL>(pos,"position"));
      m.push_back(fscalarMapping<SILENTFAIL>(roto,"rototranslation"));
      m.push_back(fscalarMapping<SILENTFAIL>(appId,"appearance"));      
    }
  
  void DefDescr<Element>::onInitializationFinished(Memory& mem){
    obj.type=(hasMass?WEIGHTINFO:0)+(hasShape?ENCUMBRANCEINFO:0);
    if (hasMass&&obj.m==0){
      throw exc::MissingData("mass");
    }
    if (hasShape&& appId==-1){
      throw exc::MissingData("appearance");
    }
    if (!isnan(pos.x())){
      setTranslation(roto.data,&pos);
    }
    //std::cout<<"roto is";print((const double*)roto.data);
    assign(obj.rotoTransl,roto);
    //std::cout<<"type is "<<obj.type<<std::endl;
    
  }

  DefDescr<Element>::DefDescr(Element& e):
    Description<Element>(e),
    hasMouseSensitivity(false),hasForceSensor(false),pos(NAN,NAN,NAN),appId(-1){

    eye(roto.data);    
  }
  
  
  void DefDescr<Part>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){
       m.push_back(scalarMapping(obj.id,"id"));    
       m.push_back(scalarMapping(elementsId,"elements"));
  }

  DefDescr<Part>::DefDescr(Part& p):Description<Part>(p){
  }

  
  
  void DefDescr<SkeletonNode>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){
    m.push_back(scalarMapping(obj.id,"id"));    
    m.push_back(fscalarMapping<SILENTFAIL>(obj.motId,"motorID"));    
    obj.pos=new Vector3d();
    m.push_back(scalarMapping(*obj.pos,"pos")); 
    m.push_back(scalarMapping(obj.type,"type")); 
    m.push_back(scalarMapping(obj.axis,"axis")); 
    m.push_back(scalarMapping(obj.offset,"offset")); 
    m.push_back(scalarMapping(obj.multiplier,"multiplier")); 
    m.push_back(scalarMapping(parentId,"parent")); 
    m.push_back(fscalarMapping<SILENTFAIL>(partId,"part")); 
  }

  void DefDescr<SkeletonNode>::onInitializationFinished(Memory& mem){      
     if (obj.type==SERVOMOTOR&&obj.motId==-1){
       throw exc::MissingData("motor ID for Node"+toString(obj.id));
     }
     //std::cout<<"this is "<<obj.id<<" the part id is"<<partId<<std::endl;
   }  


  DefDescr<SkeletonNode>::DefDescr(SkeletonNode& s):Description<SkeletonNode>(s){
  }


  void DefDescr<Robot>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){
      
      m.push_back(scalarMapping(pivotId,IDPath("pivot")));
      m.push_back(fscalarMapping<ATTACHELEMENT>(noCollsId,"noCollision"));
    }

  DefDescr<Robot>::DefDescr(Robot& r):Description<Robot>(r){
    }
  

  void DefDescr<CompleteRobot>::getMapMembers(GenericIDPath& hopname,std::vector<NodeMapping*> &m){
    hopname=IDPath("robot.structure",".");      
      m.push_back(psvectMapping(appearances,"appearances","appearance"));
      m.push_back(psvectMapping(elements,"elements","element"));
      m.push_back(psvectMapping(parts,"parts","part"));
      m.push_back(psvectMapping(nodes,"joints","joint"));    
      m.push_back(&robot); //this is not on the heap
      members=&m; //we save the pointer, and remove it;
    }


  void DefDescr<CompleteRobot>::onInitializationFinished(Memory& mem){

    /*std::cout<<"Robot has "<<
      obj.appearances.size()<<" appearances"<<std::endl<<
      obj.elements.size()<<" elements"<<std::endl<<
      obj.parts.size()<<" parts"<<std::endl<<
      obj.nodes.size()<<" nodes"<<std::endl;
    */

    members->erase(std::remove(members->begin(), members->end(), &robot), members->end());//this is not on the heap, we remove it

      int ms=0;
      int fs=0;
      for (size_t e=0;e<elements.size();e++){
	obj.elements[e].app=obj.appearances.data+elements[e].appId;	
	if (elements[e].hasMouseSensitivity){	
	  ms++;
	}
	if (elements[e].hasForceSensor){	
	  fs++;	
	}      
      }
      obj.mouseSensors.resize(ms*6);
      obj.forceAppPoint.resize(fs*6);
      obj.forceVector.resize(fs*6);
      obj.force.resize(fs*6);
      obj.withGround.resize(fs*6);
     
      obj.mouseCustomizations.resize(ms);
      obj.forceCustomizations.resize(fs);





      
      ms=0;
      fs=0;
      for (size_t e=0;e<elements.size();e++){
	if (elements[e].hasMouseSensitivity){
	  obj.elements[e].setElementCustomization(
					      &obj.mouseCustomizations[ms++],
					      MOUSESENSORBOXCUSTOMIZATION
					      );
	}
	if (elements[e].hasForceSensor){
	  obj.elements[e].setElementCustomization(
					      &obj.forceCustomizations[fs++],
					      FORCESENSORBOXCUSTOMIZATION
					      );
	}
	
      }
      


      for (size_t p=0;p<parts.size();p++){
	for (size_t i=0;i<parts[p].elementsId.size();i++){
	  obj.parts[p].elements.push_back(obj.elements.data+parts[p].elementsId[i]);
	}
      }

      int mid=0;
      for (size_t n=0;n<nodes.size();n++){
	mid=(std::max)(mid,obj.nodes[n].motId);		
      }
      
      obj.motors.resize(mid+1);
      
      
      for (size_t n=0;n<nodes.size();n++){
	if (nodes[n].parentId!=-1){
	  obj.nodes[n].ref=obj.nodes.data+nodes[n].parentId;
	}
	double* m=obj.nodes[n].motId==-1?NULL:obj.motors.data+obj.nodes[n].motId;
	switch(obj.nodes[n].type){
	case FREESLIDER:
	case FREEHINGE:
	  obj.nodes[n].fakeServo=m;
	  break;
	case SERVOMOTOR:
	  obj.nodes[n].servoValue=m;
	  break;
	case REFERENCEROTATION:
	  if (m!=NULL) throw exc::InvalidOperation("motor ID specified for REFERENCE rotation");
	  break;
	  
	}

	if (nodes[n].partId!=-1){
	  obj.nodes[n].part=obj.parts.data+nodes[n].partId;
	}
	obj.addSkeletonNode(obj.nodes.data+n);	
      }

      for (size_t i=0;i<robot.noCollsId.size();i++){
	obj.disableCollDet(obj.parts.data+robot.noCollsId[i].first,
			   obj.parts.data+robot.noCollsId[i].second);
      }
      obj.pivot=obj.parts.data+robot.pivotId;


      setAll(obj.motors,0);
      setAll(obj.mouseSensors,0);
       Vector3d v0(0,0,0);
       setAll(obj.forceAppPoint,v0);
       setAll(obj.forceVector,v0);
       setAll(obj.force,0);
       setAll(obj.withGround,false);      


    }


  DefDescr<CompleteRobot>::DefDescr(CompleteRobot& cr): Description<CompleteRobot>(cr),
      appearances(cr.appearances),
      elements(cr.elements),
      parts(cr.parts),
      nodes(cr.nodes),
      robot(cr)
    {
    }


}
