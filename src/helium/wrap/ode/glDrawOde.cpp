#include <helium/wrap/ode/glDrawOde.h>
#include <helium/graphics/ppm.h>
#include <helium/graphics/glTexture.h>
#include <helium/graphics/glHeader.h>

using namespace std;

namespace helium{

  class BasicTextures{
    PpmImage objectImg,groundImg,skyImg;
  public:
    
    GlTexture object,ground,sky;
    BasicTextures():objectImg("textures/object.ppm"),groundImg("textures/ground.ppm"),skyImg("textures/sky.ppm"),
		    object(&objectImg),ground(&groundImg),sky(&skyImg){
      //std::cout<<"got the sky"<<std::endl;
    }

  };


  static  BasicTextures textures;
  



#define USESHADOWS
#define USETEXTURES



const float ground_scale = 15*1.0f/4.0f;
const float ground_ofsx = 0.5;		// offset of ground texture
const float ground_ofsy = 0.5;
const float gsize = 1000.0f;

const float sky_scale = 1.0f/40.0f;	// sky texture scale (1/size)
const float sky_height = 1.0f;		// sky height above viewpoint
const float minBoxTextSize=0.15;

const float offset = 0; 

const float  SHADOW_INTENSITY=0.65f;
const float GROUND_R=0.0f; 	// ground color for when there's no texture
const float GROUND_G=0.5f;
const float GROUND_B=0.0f;

  GlDrawODE::GlDrawODE(ODEWorld* w,const double *px,const double *py,const double *pz,bool printT):world(w),cx(px),cy(py),cz(pz),currTexture(&textures.ground),wireframeMode(SOLID){};

void setCoordinateTransform (const double pos[3], const double R[12])
{
  GLfloat matrix[16];
  matrix[0]=R[0];
  matrix[1]=R[4];
  matrix[2]=R[8];
  matrix[3]=0;
  matrix[4]=R[1];
  matrix[5]=R[5];
  matrix[6]=R[9];
  matrix[7]=0;
  matrix[8]=R[2];
  matrix[9]=R[6];
  matrix[10]=R[10];
  matrix[11]=0;
  matrix[12]=pos[0];
  matrix[13]=pos[1];
  matrix[14]=pos[2];
  matrix[15]=1;
  glMultMatrixf (matrix);
}

#ifdef USESHADOWS
static void setShadowCoordinateTransform()
{
  GLfloat matrix[16];
  memset(matrix,0,sizeof(GLfloat)*16); 
  matrix[0]=1;
  matrix[5]=1;
  matrix[8]=-LIGHTX;
  matrix[9]=-LIGHTY;
  matrix[15]=1;
  matrix[14]=2E-3;
  glMultMatrixf (matrix);
}




void GlDrawODE::setShadowDrawingMode(){
  if (pushNames) glLoadName(-1);
  glDisable (GL_LIGHTING);
#ifdef USETEXTURES  
  glEnable (GL_TEXTURE_2D);
  glEnable (GL_TEXTURE_GEN_S);
  glEnable (GL_TEXTURE_GEN_T);
  textures.ground.bind (1);
  glColor3f (SHADOW_INTENSITY,SHADOW_INTENSITY,SHADOW_INTENSITY);
  glEnable (GL_TEXTURE_2D);
  
  glTexGeni (GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
  glTexGeni (GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
  static GLfloat s_params[4] = {ground_scale,0,0,ground_ofsx};
  static GLfloat t_params[4] = {0,ground_scale,0,ground_ofsy};
  glTexGenfv (GL_S,GL_EYE_PLANE,s_params);
  glTexGenfv (GL_T,GL_EYE_PLANE,t_params);
#else
  glDisable (GL_TEXTURE_2D);
  glColor3f (GROUND_R*SHADOW_INTENSITY,GROUND_G*SHADOW_INTENSITY,
	       GROUND_B*SHADOW_INTENSITY);
#endif
  glDepthRange (0,0.9999);  
}
#endif

void GlDrawODE::drawGround(){
  glDisable (GL_LIGHTING);
  glShadeModel (GL_FLAT);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LESS);
#ifdef USETEXTURES  
  glEnable (GL_TEXTURE_2D);
  textures.ground.bind (0);  
#else 
  glDisable (GL_TEXTURE_2D);
  glColor3f (GROUND_R,GROUND_G,GROUND_B);
#endif 
  glBegin (GL_QUADS);
  glNormal3f (0,0,1);
  glTexCoord2f (-gsize*ground_scale + ground_ofsx,
		-gsize*ground_scale + ground_ofsy);
  glVertex3f (-gsize,-gsize,offset);
  glTexCoord2f (gsize*ground_scale + ground_ofsx,
		-gsize*ground_scale + ground_ofsy);
  glVertex3f (gsize,-gsize,offset);
  glTexCoord2f (gsize*ground_scale + ground_ofsx,
		gsize*ground_scale + ground_ofsy);
  glVertex3f (gsize,gsize,offset);
  glTexCoord2f (-gsize*ground_scale + ground_ofsx,
		gsize*ground_scale + ground_ofsy);
  glVertex3f (-gsize,gsize,offset);
  glEnd();
}


void GlDrawODE::drawSky(double time){
  glDisable (GL_LIGHTING);
#ifdef USETEXTURES
    glEnable (GL_TEXTURE_2D);
    glDisable (GL_TEXTURE_GEN_S);
    glDisable (GL_TEXTURE_GEN_T);
    textures.sky.bind (0);
#else 
    glDisable (GL_TEXTURE_2D);
    glColor3f (0,0.5,1.0);
#endif
  // make sure sky depth is as far back as possible
  glShadeModel (GL_FLAT);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);
  glDepthRange (1,1);

  const float ssize = 1000.0f;
  float offset =(float)time;  
  float x = ssize*sky_scale;
  float z = -*cz + sky_height;
  glBegin (GL_QUADS);
  glNormal3f (0,0,-1);
  glTexCoord2f (-x+offset,-x+offset);
  glVertex3f (-ssize+*cx,-ssize+*cy,z);
  glTexCoord2f (-x+offset,x+offset);
  glVertex3f (-ssize+*cx,ssize+*cy,z);
  glTexCoord2f (x+offset,x+offset);
  glVertex3f (ssize+*cx,ssize+*cy,z);
  glTexCoord2f (x+offset,-x+offset);
  glVertex3f (ssize+*cx,-ssize+*cy,z);
  glEnd();
  offset = offset + 0.002f;
  if (offset > 1) 
    offset -= 1;
  glDepthFunc (GL_LESS);
  glDepthRange (0,1);
}


static void drawPatch (float p1[3], float p2[3], float p3[3], int level)
{
  int i;
  if (level > 0) {
    float q1[3],q2[3],q3[3];		 // sub-vertices
    for (i=0; i<3; i++) {
      q1[i] = 0.5f*(p1[i]+p2[i]);
      q2[i] = 0.5f*(p2[i]+p3[i]);
      q3[i] = 0.5f*(p3[i]+p1[i]);
    }
    float length1 = (float)(1.0/sqrt(q1[0]*q1[0]+q1[1]*q1[1]+q1[2]*q1[2]));
    float length2 = (float)(1.0/sqrt(q2[0]*q2[0]+q2[1]*q2[1]+q2[2]*q2[2]));
    float length3 = (float)(1.0/sqrt(q3[0]*q3[0]+q3[1]*q3[1]+q3[2]*q3[2]));
    for (i=0; i<3; i++) {
      q1[i] *= length1;
      q2[i] *= length2;
      q3[i] *= length3;
    }
    drawPatch (p1,q1,q3,level-1);
    drawPatch (q1,p2,q2,level-1);
    drawPatch (q1,q2,q3,level-1);
    drawPatch (q3,q2,p3,level-1);
  }
  else {
    glNormal3f (p1[0],p1[1],p1[2]);
    glVertex3f (p1[0],p1[1],p1[2]);
    glNormal3f (p2[0],p2[1],p2[2]);
    glVertex3f (p2[0],p2[1],p2[2]);
    glNormal3f (p3[0],p3[1],p3[2]);
    glVertex3f (p3[0],p3[1],p3[2]);
  }
}

static int sphere_quality = 1;

static void drawUnitSphere()
{
  // icosahedron data for an icosahedron of radius 1.0
# define ICX 0.525731112119133606f
# define ICZ 0.850650808352039932f
  static GLfloat idata[12][3] = {
    {-ICX, 0, ICZ},
    {ICX, 0, ICZ},
    {-ICX, 0, -ICZ},
    {ICX, 0, -ICZ},
    {0, ICZ, ICX},
    {0, ICZ, -ICX},
    {0, -ICZ, ICX},
    {0, -ICZ, -ICX},
    {ICZ, ICX, 0},
    {-ICZ, ICX, 0},
    {ICZ, -ICX, 0},
    {-ICZ, -ICX, 0}
  };

  static int index[20][3] = {
    {0, 4, 1},	  {0, 9, 4},
    {9, 5, 4},	  {4, 5, 8},
    {4, 8, 1},	  {8, 10, 1},
    {8, 3, 10},   {5, 3, 8},
    {5, 2, 3},	  {2, 7, 3},
    {7, 10, 3},   {7, 6, 10},
    {7, 11, 6},   {11, 0, 6},
    {0, 1, 6},	  {6, 1, 10},
    {9, 0, 11},   {9, 11, 2},
    {9, 2, 5},	  {7, 2, 11},
  };

  glBegin (GL_TRIANGLES);
  for (int i=0; i<20; i++) {
    drawPatch (&idata[index[i][2]][0],&idata[index[i][1]][0],
	       &idata[index[i][0]][0],sphere_quality);
  }
  glEnd();
}


void GlDrawODE::drawSphereShadow (float px, float py, float pz, float radius)
{
  // calculate shadow constants based on light vector
  if (pushNames) glLoadName(-1);
  static int init=0;
  static float len2,len1,scale;
  if (!init) {
    len2 = LIGHTX*LIGHTX + LIGHTY*LIGHTY;
    len1 = 1.0f/(float)sqrt(len2);
    scale = (float) sqrt(len2 + 1);
    init = 1;
  }
  // map sphere center to ground plane based on light vector
  px -= LIGHTX*pz;
  py -= LIGHTY*pz;
  const float kx = 0.96592582628907f;
  const float ky = 0.25881904510252f;
  float x=radius, y=0;
  glBegin (GL_TRIANGLE_FAN);
  for (int i=0; i<24; i++) {
    // for all points on circle, scale to elongated rotated shadow and draw
    float x2 = (LIGHTX*x*scale - LIGHTY*y)*len1 + px;
    float y2 = (LIGHTY*x*scale + LIGHTX*y)*len1 + py;
    glTexCoord2f (x2*ground_scale+ground_ofsx,y2*ground_scale+ground_ofsy);
    glVertex3f (x2,y2,0);

    // rotate [x,y] vector
    float xtmp = kx*x - ky*y;
    y = ky*x + kx*y;
    x = xtmp;
  }
  glEnd();
}








void GlDrawODE::drawSphere (const double pos[3], const double R[12],
			      double radius){  
    
  setFaceProperties(0);
  glEnable (GL_NORMALIZE);
  glShadeModel (GL_SMOOTH);
  glPushMatrix();
  setCoordinateTransform (pos,R);
  glScaled (radius,radius,radius);
  glEnable (GL_TEXTURE_GEN_S);
     glEnable (GL_TEXTURE_GEN_T);
     glTexGeni (GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
      glTexGeni (GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
      static GLfloat s_params[4] = {1.0f,1.0f,0.0f,1};
      static GLfloat t_params[4] = {0.817f,-0.817f,0.817f,1};
      glTexGenfv (GL_S,GL_OBJECT_PLANE,s_params);
      glTexGenfv (GL_T,GL_OBJECT_PLANE,t_params);  
  drawUnitSphere();
  glPopMatrix();
#ifdef USESHADOWS
  setShadowDrawingMode();
  glShadeModel (GL_FLAT);
  glDepthRange (0,0.9999);
  drawSphereShadow (pos[0],pos[1],pos[2],radius);
  glDepthRange (0,1);    
//USESHADOWS
#endif
}



static void setMaterialColor (float r, float g, float b, float alpha)
{ 
  if (alpha < 1) {
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  }
  else {
    glDisable (GL_BLEND);
  }
  glColor3f(r,g,b);
  glEnable (GL_LIGHTING);
  GLfloat light_ambient[4],light_diffuse[4],light_specular[4];
  static const float lightA=1; //0.3f;
  static const float lightD=0.7;//0.3f;
  static const float lightS=0.2;//0.3f;
  light_ambient[0] = r*lightA;
  light_ambient[1] = g*lightA;
  light_ambient[2] = b*lightA;
  light_ambient[3] = alpha;
  light_diffuse[0] = r*lightD;
  light_diffuse[1] = g*lightD;
  light_diffuse[2] = b*lightD;
  light_diffuse[3] = alpha;
  light_specular[0] = r*lightS;
  light_specular[1] = g*lightS;
  light_specular[2] = b*lightS;
  light_specular[3] = alpha;
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, light_ambient);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, light_diffuse);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, light_specular);
  glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS,0.0f);
}




  void GlDrawODE::setTexture(bool state){
  if (state&&currElement->app->useTexture){
    glEnable (GL_LIGHTING);
    glEnable (GL_TEXTURE_2D);  

    currTexture=currElement->app->texture!=NULL?
      currElement->app->texture:
      &textures.object;

    currTexture->bind(true);
    
    glDisable (GL_TEXTURE_GEN_S);
    glDisable (GL_TEXTURE_GEN_T);
 
  }else{
    glDisable (GL_LIGHTING);
    glDisable (GL_TEXTURE_GEN_S);
    glDisable (GL_TEXTURE_GEN_T);    
    glDisable (GL_TEXTURE_2D);
    glBindTexture (GL_TEXTURE_2D,0);  
  }         
}



void GlDrawODE::setFaceProperties(int f){
  bool withGround=true;
  static double maxForce=1E-3;
  double val=0;
  if (currElement->hasCustomization(MOUSESENSORBOXCUSTOMIZATION)){
    //cout<<"is mouse"<<endl;
    val=*((MouseSensorBoxCustomization*)(currElement->elementCustomizations[MOUSESENSORBOXCUSTOMIZATION]))
      ->mouseActivation[f];
  }
  
  if (val==0&&currElement->hasCustomization(BINARYSENSORBOXCUSTOMIZATION)){
    //cout<<"is binary"<<endl;
    if (*(((BinarySensorBoxCustomization*)(currElement->elementCustomizations[BINARYSENSORBOXCUSTOMIZATION]))
	  ->contactActivation[f])){
      val=1;
    }
  }
  if (val==0&&currElement->hasCustomization(FORCESENSORBOXCUSTOMIZATION)){
    //cout<<"is force"<<endl;
    double force=*(((ForceSensorBoxCustomization*)(currElement->elementCustomizations[FORCESENSORBOXCUSTOMIZATION]))
		   ->force[f]);    
    maxForce=max(maxForce,force);
    val=force/maxForce;
  }
  if (currElement->hasCustomization(FORCESENSORBOXCUSTOMIZATION)){ //even if val>0 is set by the mouse
    withGround=*(((ForceSensorBoxCustomization*)(currElement->elementCustomizations[FORCESENSORBOXCUSTOMIZATION]))
		 ->withGround[f]);
  }
  //cout<<"val="<<val<<endl;
  if (val==0){
    setMaterialColor(currElement->app->r,currElement->app->g,currElement->app->b,currElement->app->alpha);
#ifdef USETEXTURES
    setTexture(true);
#endif

  }else if (val<0){
    setMaterialColor(0,-val,0,currElement->app->alpha);      
#ifdef USETEXTURES	
    setTexture(false);
#endif

  }else{
    setMaterialColor(0.5+0.5*val,0,withGround?0:0.5+0.5*val,currElement->app->alpha);
#ifdef USETEXTURES
    setTexture(false);
#endif

  }  
  if ((wireframeMode==WIREFRAME&&val==0)||
      (wireframeMode==INACTIVEWIREFRAME
       &&currElement->hasCustomization(MOUSESENSORBOXCUSTOMIZATION)
       &&!((MouseSensorBoxCustomization*)(currElement->elementCustomizations[MOUSESENSORBOXCUSTOMIZATION]))->active[f])
      ){
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  
  }else{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      
  }
}



int GlDrawODE::getSensorID(int f){
  if (currElement->hasCustomization(MOUSESENSORBOXCUSTOMIZATION)){
    return ((MouseSensorBoxCustomization*)(currElement->elementCustomizations[MOUSESENSORBOXCUSTOMIZATION]))
      ->id[f];
  }
  if (currElement->hasCustomization(BINARYSENSORBOXCUSTOMIZATION)){
    return ((BinarySensorBoxCustomization*)(currElement->elementCustomizations[BINARYSENSORBOXCUSTOMIZATION]))
      ->id[f];    
  }
  if (currElement->hasCustomization(FORCESENSORBOXCUSTOMIZATION)){
    return ((ForceSensorBoxCustomization*)(currElement->elementCustomizations[FORCESENSORBOXCUSTOMIZATION]))
      ->id[f];
  }
  return -1;
}




void GlDrawODE::drawCapsule (double l, double r,bool colorize){
  if (capsules.count(currElement->id)==0){
    //cout<<"creating new capsule"<<endl;
    capsules[currElement->id]=new Capsule(l,r);
  }
  Capsule* c=capsules[currElement->id];
  glEnable (GL_TEXTURE_GEN_S);
  
  for (int f=0;f<CapsuleNS::FACES;f++){
    double* data;
    int vn=c->getNormalsAndVerteces(&data,f);
    if (colorize){
      setFaceProperties(f);
      if (pushNames){
	//std::cout<<"pushed name "<<getSensorID(f)<<std::endl;
	glLoadName(getSensorID(f));    
      }
    }     

    glBegin (GL_TRIANGLES);
    for (int i=0;i<vn;i++){
      glTexCoord2f(data[i*6+0],data[i*6+1]);
      glNormal3d(data[i*6+0],data[i*6+1],data[i*6+2]);
      glVertex3d(data[i*6+3],data[i*6+4],data[i*6+5]);

    }
    glEnd();
    
  }
  if (pushNames)
    glLoadName(-1);
  //glPopMatrix();
}//drawCapsule



void GlDrawODE::drawCapsule (const double pos[3], const double R[12],
				     double length, double radius)
{
  glShadeModel (GL_SMOOTH);
  glPushMatrix();
  setCoordinateTransform (pos,R);
  drawCapsule (length,radius,true);
  glPopMatrix();
#ifdef USESHADOWS
  //setShadowDrawingMode();
  glPushMatrix();
  setShadowDrawingMode();
  setShadowCoordinateTransform();
  setCoordinateTransform (pos,R);
  drawCapsule (length,radius,false);
  //drawCapsule (length,radius);
  glPopMatrix();  
  glDepthRange (0,1);
#endif
}



void GlDrawODE::drawBox(const double sides[3],bool colorize){
  static float tempVertex[24];
  getBoxVertex(tempVertex,sides[0],sides[1],sides[2],true);
  for(int f=0;f<6;f++){ 
    if (colorize){
      setFaceProperties(f);
      if (pushNames){
	glLoadName(getSensorID(f));    
	//std::cout<<"pushed name "<<getSensorID(f)<<std::endl;
      }
    }//colorize
   
    glBegin(GL_QUADS);
    float a=distance(&tempVertex[boxQuadFaces[f*4]*3],&tempVertex[boxQuadFaces[f*4+1]*3]);
    float b=distance(&tempVertex[boxQuadFaces[f*4+1]*3],&tempVertex[boxQuadFaces[f*4+2]*3]);      
    //float min,max;
    bool off=0;
    if (a>b){
      //max=a;
      //min=b;      
      b=b/a;
      a=1;
    }else{
      //max=b;
      //min=a;
      a=a/b;
      b=1;
      off=true;
    }
    //a*=max/minBoxTextSize;
    //b*=max/minBoxTextSize;    
    for (int v=0;v<4;v++){          
#ifdef USETEXTURES
      glTexCoord2d(currTexture->getXscale()*(v==1||v==2?off?b:a:0),currTexture->getYscale()*(v==2||v==3?off?a:b:0));  
#endif
      glVertex3d(tempVertex[boxQuadFaces[f*4+(v+off)%4]*3],
		 tempVertex[boxQuadFaces[f*4+(v+off)%4]*3+1] ,
		 tempVertex[boxQuadFaces[f*4+(v+off)%4]*3+2]);      
    }
    glEnd();
  }//for f
}

void GlDrawODE::drawBox (const double pos[3], const double R[12],
				     const double sides[3])
{
  glShadeModel (GL_FLAT);  
  glPushMatrix();
  setCoordinateTransform (pos,R);
  drawBox (sides,true);
  glPopMatrix();
#ifdef USESHADOWS
  glPushMatrix();
  setShadowDrawingMode();
  setShadowCoordinateTransform();
  setCoordinateTransform (pos,R);
  drawBox (sides,false);
  glPopMatrix();  
  glDepthRange (0,1);
#endif
}




void GlDrawODE::drawGeom (dGeomID g, const dReal *pos, const dReal *R){
  if (!g) return;
  if (!pos) pos = dGeomGetPosition (g);
  if (!R) R = dGeomGetRotation (g);
  switch(int type=dGeomGetClass(g)){      
    case dBoxClass:{
      dVector3 sides;
      dGeomBoxGetLengths (g,sides);    
      drawBox (pos,R,sides);
      break;
    }
  case dGeomTransformClass: {
    dGeomID g2 = dGeomTransformGetGeom (g);
    const dReal *pos2 = dGeomGetPosition (g2);
    const dReal *R2 = dGeomGetRotation (g2);
    dVector3 actual_pos;
    dMatrix3 actual_R;
    dMULTIPLY0_331 (actual_pos,R,pos2);
    actual_pos[0] += pos[0];
    actual_pos[1] += pos[1];
    actual_pos[2] += pos[2];
    dMULTIPLY0_333 (actual_R,R,R2);
    drawGeom (g2,actual_pos,actual_R);
    break;
  }
  case dSphereClass:
    drawSphere (pos,R,dGeomSphereGetRadius (g));
    break;
  case dCCylinderClass:
    dReal radius,length;
    dGeomCapsuleGetParams (g,&radius,&length);
    drawCapsule (pos,R,length,radius);
    break;
  case dPlaneClass:
    
  default:
    printf("I don't know how to draw a %d\n",type);
  }//case
}//drawGeom



void GlDrawODE::drawODERobot(ODERobot* robot,bool pNames,WireframeMode wireframe){
  world->lock();
  pushNames=pNames;
  wireframeMode=wireframe;
  if (wireframeMode!=SOLID)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  for (unsigned int p=0;p<robot->odeParts.size();p++){
    int s=(int)robot->odeParts[p]->geoms.size();
    for (int g=0;g<s;g++){
      currElement=robot->odeParts[p]->srcElems[g];
      if (currElement->app->alpha!=0){
	drawGeom (robot->odeParts[p]->geoms[g],0,0);	
      }
    }//for
  }//for
  if (wireframeMode!=SOLID)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  world->unlock();
}


void GlDrawODE::drawODERobot(vector<ODERobot*> robots,bool pushNames,WireframeMode wireframe){
  for (unsigned int i=0;i<robots.size();i++){
    drawODERobot(robots[i],pushNames,wireframe);
  }
}


void resetPerspectiveProjection() {
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

void renderBitmapString(float x, float y, void *font,char *string)
{
  
  char *c;
  glRasterPos2f(x, y);
  glColor4f(0,0,0,1);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}
    
void setOrthographicProjection() {  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();  
  gluOrtho2D(0, 640, 0, 480);// set a 2D orthographic projection
  glScalef(1, -1, 1);  // invert the y axis
  glMatrixMode(GL_MODELVIEW);
}


void GlDrawODE::printTime(ODEWorld* w){
  printTime(w->paused?0:(long)round(w->t*1000),getSystemMillis()-w->timeZero);
}




void GlDrawODE::printTime(long t,long realt){
  double ratio=(((double)t)/realt);
  long millis=t%1000;
  t/=1000;
  long secs=t%60;
  t/=60;
  long mins=t%60;
  t/=60;
  char buffer[512];
#ifdef VIDEO_RECORDING
  sprintf(buffer,"%02ld:%02ld:%02ld.%03ld",t,mins,secs,millis);
#else
  sprintf(buffer,"%02ld:%02ld:%02ld.%03ld (%.3fX)",t,mins,secs,millis,ratio);
#endif


  glPushMatrix();  
  setOrthographicProjection();
  glLoadIdentity();
  glDisable (GL_LIGHTING);
  glDisable (GL_TEXTURE_2D);    
#ifdef VIDEO_RECORDING
  glColor4f(0,0,0,1);
#else
  glColor4f(1,1,1,1);
#endif


  renderBitmapString(10,-10,GLUT_BITMAP_8_BY_13,buffer);
  resetPerspectiveProjection();
  glPopMatrix();
}


Element freeElement(NULL,NULL,0,NULL,0);

void GlDrawODE::drawODEEnvironment(){
  drawGround();
  drawSky(world->t/30);  
  currElement=&freeElement;
  for (unsigned int i=0;i<world->freeObjects.size();i++){
    freeElement.app=world->freeObjects[i]->app;
    drawGeom (world->freeObjects[i]->geom,0,0);
  }
}


GlDrawODE::~GlDrawODE(){
  for (std::map<int,Capsule*>::iterator it=capsules.begin();
       it!=capsules.end();
       ++it){
    delete it->second;
  }
  
}

}//helium

