#ifndef HE_GLDRAWODE_
#define HE_GLDRAWODE_

#include <helium/wrap/ode/odeStructure.h>

#include <helium/graphics/capsule.h>

#include <map>

static const float LIGHTX= 1.0f;
static const float LIGHTY=0.4f;



namespace helium{

class GlDrawODE{
 private:
  ODEWorld* world;
  const double * cx,*cy,*cz;
  bool pushNames;
  std::map<int,Capsule*> capsules; //they need some preprocessing
  GlTexture* currTexture;
 public:
  enum WireframeMode{SOLID,WIREFRAME,INACTIVEWIREFRAME} wireframeMode;
 private:
  
  Element * currElement;
  void setTexture(bool state);
  void drawGround();
  void drawSky(double time);
  void setShadowDrawingMode();
  void drawSphereShadow (float px, float py, float pz, float radius);
  //void dsDrawSphere (const float pos[3], const float R[12],float radius);
  //void dsDrawSphere (const double pos[3], const double R[12], double radius);
  //void dsDrawCapsule (const float pos[3], const float R[12],float length, float radius);
  //void dsDrawCapsule (const double pos[3], const double R[12], double length, double radius);
  //void dsDrawBox (const float pos[3], const float R[12],const float sides[3]);
  //void dsDrawBox (const double pos[3], const double R[12], const double sides[3]);
  //double getColorVal(int f,bool &withGround,int &id);
  //void drawBox2(const float sides[3]);
  //void drawCapsule (float l, float r);

  void drawGeom (dGeomID g, const dReal *pos, const dReal *R);

  void setFaceProperties(int f);
  int getSensorID(int f);

  
public: //for 
  void drawBox(const double sides[3],bool colorize);
private:
  void drawBox (const double pos[3], const double R[12],
		const double sides[3]);

  void drawCapsule (double l, double r,bool colorize);
  void drawCapsule (const double pos[3], const double R[12],
		    double length, double radius);


  void drawSphere (const double pos[3], const double R[12],
		   double radius);

 public:
  void drawODERobot(ODERobot* robot,bool pushNames=true,WireframeMode wireframe=SOLID);
  void drawODEEnvironment();
  void drawODERobot(std::vector<ODERobot*> robots,bool pushNames=true,WireframeMode wireframe=SOLID);
  void printTime(long t,long realt);
  void printTime(ODEWorld* w);
  //cx,cy,cz are needed to draw the sky
  GlDrawODE(ODEWorld* w,const double *px,const double *py,const double *pz,bool printT=true);
  virtual ~GlDrawODE();
  //void setupDrawingMode(bool textures=true);
};


}//helium

/*inline void GlDrawODE::dsDrawBox (const double pos[3], const double R[12], const double sides[3])
{
  int i;
  float pos2[3],R2[12],fsides[3];
  for (i=0; i<3; i++) pos2[i]=(float)pos[i];
  for (i=0; i<12; i++) R2[i]=(float)R[i];
  for (i=0; i<3; i++) fsides[i]=(float)sides[i];
  dsDrawBox (pos2,R2,fsides);
  }*/


 /*inline void GlDrawODE::dsDrawSphere (const double pos[3], const double R[12], double radius)
{
  int i;
  float pos2[3],R2[12];
  for (i=0; i<3; i++) pos2[i]=(float)pos[i];
  for (i=0; i<12; i++) R2[i]=(float)R[i];
  dsDrawSphere (pos2,R2,(float)radius);
  };*/

/*inline void GlDrawODE::dsDrawCapsule (const double pos[3], const double R[12],double length, double radius)
{
  int i;
  float pos2[3],R2[12];
  for (i=0; i<3; i++) pos2[i]=(float)pos[i];
  for (i=0; i<12; i++) R2[i]=(float)R[i];
  dsDrawCapsule (pos2,R2,(float)length,(float)radius);
};
*/

#endif


