/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \author  Fransiska Basoeki
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef _CONVEXMODEL_
#define _CONVEXMODEL_
#include "boxNode.h"
#include "jointParser.h"

//#include <robot/motors.h>
//#include <thread/synchronization.h>
#include <helium/thread/mutex.h>
#include <map>
//#include <helium/vector/destroy.h>
#include <helium/vector/array.h>
#include <helium/destroy/autoDestroy.h>
//#include <helium/math/algebra.h>
// left hand
// right leg
namespace helium{
namespace convex{
  class ConvexLink{
    
  public:
    int parent;
    int id;
    int motorId;
    double offset,multiply;
    std::vector<BoxNode*> part;
    JointInfo* info;
    bool toMirror; 
  };
  
  
  
  class ConvexModel{
    AutoDestroy ad;
  protected:
    bool loaded;
  public:
    
    std::vector<ConvexLink*> links;
    double defaultMargin;
    std::map<std::pair< std::pair<int,int>,std::pair<int,int> >,double > margin;
    
    JointInfo* mirrorPointL,*mirrorPointR;
    //bool canCollide(int o1,int p1,int o2,int p2) const;
    double getMargin(int o1,int p1,int o2,int p2) const;
    //-----
    JointParser jointParser;
    BoxNode boxNode;
    ConvexModel():loaded(false),mirrorPointL(NULL),mirrorPointR(NULL){
    }
    ConvexModel(const char* modelFilename):loaded(false),mirrorPointL(NULL),mirrorPointR(NULL){
      loadFromFile(modelFilename);
    }
    virtual bool loadFromFile(const char* modelFilename);
    void printLink();
    void printMargin();
    int getId(const char* name);
    std::pair<int,int> getPartId(const char* name,bool returnVectorId) const;
    void initializeLink(TiXmlElement*);
    void initializeCollMargin(TiXmlElement*,const char* modelFilename);
  };
  
  
  class ConvexData{
  public:
    class EdgeData{
    public:
      int from,to;
      int mini, maxi;
      double scale;
    EdgeData(std::pair<int,int> p,double pscale):from(p.first),to(p.second),scale(pscale){
      }
      void setLimits(int pmini,int pmaxi){
	mini=pmini;
	maxi=pmaxi;
      }
    };
    
    class EnclosingSphere{
    public:
      helium::Array<double> center;
      double radius;
      int part1,point1,part2,point2;//the furthest two points
      EnclosingSphere();
    };
    
    class PartData{
    public:
      bool isCopy;
      BoxNode obj; //20111017 converting to an object
      bool colliding;
      //helium::Array<double*> absolutePnt; //replaced with array 20111017
      helium::Array<helium::Array<double,3> > absolutePnt;
      std::vector<EdgeData> edges;   
      std::vector<std::pair<int,int> > peripheralEdges;
      std::vector<std::pair<int,int> > diagonals;//for drawing only
      EnclosingSphere sphere;
      void makeEdges();
      void makeSphere();
      inline void updateSphere();
      
      PartData();
      
      virtual ~PartData();
      inline void getDirection(double* dir,int eid) const ;
      inline void getDirection(double* dir,double &mine,double &maxe,int eid) const;
      inline void getExtent(double &mine,double &maxe,const double* dir) const;
      inline void getSphereExtent(double &mine,double &maxe,const double* dir) const;
      int getVertexNum(){
	return absolutePnt.size();
      }
      PartData(const PartData&);
      PartData& operator=(const PartData& p);	
    };
    
  public:
    ConvexLink* src;  
    helium::Array<double,16> relative;//double relative[16]; //convert to array 20111017
    helium::Array<double,16> absolute;//double absolute[16]; //convert to array 20111017
    const double* angle;
    helium::Array<PartData> parts;
    EnclosingSphere sphere;
    void makeSphere();
    inline void updateSphere();
    ConvexData();
    ConvexData(const ConvexData& b);
    ConvexData& operator=(const ConvexData& b);
  };//ConvexData




  class ConvexCollisionModel{    
    class CollisionCheck{
    public:  
      CollisionCheck* nextCheck;
      CollisionCheck* nextCouple;
      std::pair<int,int> obj1,obj2;
      int suggestedEdge; //last colliding edge
      double margin;
      
    CollisionCheck(int o1,int p1,int o2,int p2,double pmargin):nextCheck(NULL),nextCouple(NULL),obj1(o1,p1),obj2(o2,p2),suggestedEdge(0),margin(pmargin){
      }
    CollisionCheck():nextCheck(NULL),nextCouple(NULL),obj1(-1,-1),obj2(-1,-1),suggestedEdge(-1),margin(-1){}
      void flipObj(){
	std::pair<int,int> tmp=obj1;
	obj1=obj2;
	obj2=tmp;
      }    
      void push_back(CollisionCheck* c);
      int size();
      virtual ~CollisionCheck();
    }; 
    friend void copyCheckChain(CollisionCheck  **dst,CollisionCheck  *src);
    //tmp vars ---
    double mine1,maxe1,mine2,maxe2;
    double dir[3],dir1[3],dir2[3];
    
    //---
    
    double mirrorMatrix[16];
    double jointOffsetMatrix[16];
    CollisionCheck  *checks;
    
    
    const ConvexModel* src;
    
    helium::Array<double> internalAngles;
    void getJointRoto(double* m,const JointInfo* b);
    void initAngles(const double* angles);
  private:
    ConvexCollisionModel(const ConvexCollisionModel&);//make impossible
  public:  
    double* getAngles() {return internalAngles.data;}
    helium::CountMutex access;
    helium::Array<ConvexData> cd; 
    ConvexCollisionModel(const ConvexModel* cm,const double* angles=NULL);
    virtual ~ConvexCollisionModel();
    void oldRotate();
    virtual void rotate();
    inline bool isDisjoint(const ConvexData::PartData &p1,const ConvexData::PartData &p2,int e,const double &margin);
    inline bool isDisjoint(const ConvexData::PartData &p1,const ConvexData::PartData &p2,const double* dir,const double& margin);
    bool isColliding(bool fastCheck=false);
    virtual void rotate(const double *v);
    virtual void rotate(int j,double v);
    const ConvexModel* getSrc(){
      return src;
    }
    ConvexCollisionModel(const ConvexCollisionModel&,double* newangles);
    
    
    //void getLowest(int* cdidx, int k,const double* dir);
    
  };

}//namespace convex
}
#endif
