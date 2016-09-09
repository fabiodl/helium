/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/graphics/glUtil.h>
#include <helium/graphics/glHeader.h>
#include <helium/math/algebra.h>



helium::Rectangle helium::fullRectangle={0,0,
		       1,0,
		       1,1,
		       0,1};


namespace helium{
  
  void clearWindow(float r,float g,float b){
    glClearColor (r,g,b,0.0f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void getLookAt(double* viewMatrix,const Vector3d &target,const Vector3d& pos,const Vector3d& up){
  Vector3d dir;
  dir.sub(pos,target);
  dir.normalize();
  Vector3d horiz;
  horiz.cross(dir,up);
  horiz.normalize();
  Vector3d nup;

  nup.cross(horiz,dir);
  horiz.negate();
  nup.normalize();
  double vmi[16];
  setColumn(vmi,horiz,0);
  setColumn(vmi,nup,1);
  setColumn(vmi,dir,2);
  setColumn(vmi,pos,3);
  vmi[12]=vmi[13]=vmi[14]=0;
  vmi[15]=1;
  invert(viewMatrix,vmi);  
}


void setCamera(const double* viewMatrix){
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
  static GLdouble transp[16];
  for (int i=0;i<4;i++)
    for (int j=0;j<4;j++){
      transp[i*4+j]=viewMatrix[j*4+i];
    }
  glMultMatrixd(transp);
}


  void setPerspective(double width,double height,bool skipinit,float zoom,float k,float vnear,float vfar){
    if (!skipinit){
      glMatrixMode (GL_PROJECTION);  
      glLoadIdentity();
    }  
    if (width >= height) {
      float k2 = float(height)/float(width);
      glFrustum (-vnear*k*zoom,vnear*k*zoom,-vnear*k*k2*zoom,vnear*k*k2*zoom,vnear,vfar);
    }else {
      float k2 = float(width)/float(height);
      glFrustum (-vnear*k*k2*zoom,vnear*k*k2*zoom,-vnear*k*zoom,vnear*k*zoom,vnear,vfar);
    }  
  }



void setLight(){
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  static GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
  static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
  glShadeModel (GL_FLAT);

}


void initGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
  glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
  glDepthFunc(GL_LESS);			        // The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);		        // Enables Depth Testing
  //  std::cout << "-----------------------------------------------------------------------------------------depth test enabled" << std::endl;
  glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();				// Reset The Projection Matrix
  gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f); // Calculate The Aspect Ratio Of The Window
  glMatrixMode(GL_MODELVIEW);  
}


  void getPixels(unsigned char* dst,int w,int h){
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,dst);
  } 

  void flipUD(unsigned char* buffer,int w,int h){
    Array<unsigned char> row(w*3);
    for (int i=0;i<h/2;i++){
      unsigned char* r1=buffer+(w*3*i);
      unsigned char* r2=buffer+(w*3*(h-i-1));
      row=r2;
      assign(r2,r1,w*3);      
      assign(r1,row);
    }
  }



void drawBox(double *v){
  for (int f=0;f<6;f++){	        
    glBegin(GL_QUADS);	
    for (int vc=0;vc<4;vc++){          
      int vertex=boxQuadFaces[f*4+vc];	    	    
      glVertex3f((float)v[vertex*3], (float)v[vertex*3+1],(float)v[vertex*3+2]);
    }	  	  
    glEnd();
  }
}



void drawBox(double sx,double sy,double sz,double* rotoTra){
  double v[24];
  double v2[24];
  getBoxVertex(v,sx,sy,sz,true);
  for (int i=0;i<8;i++){
    mulPoint(&v2[3*i],rotoTra,&v[3*i]);
  }
  drawBox(v2);
}


double getPointZ(int x, int y){
  GLfloat winZ;
  glReadPixels( x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
  return winZ;
}

void getSpacePos(double * pos,int x, int y,double z)
{
  GLint viewport[4];
  GLdouble modelview[16];
  GLdouble projection[16];	  
  glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
  glGetDoublev( GL_PROJECTION_MATRIX, projection );
  glGetIntegerv( GL_VIEWPORT, viewport );	
  gluUnProject(x, y, z, modelview, projection, viewport, &pos[0], &pos[1], &pos[2]);	
}

void setOrthogonal (int w,int h) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);
  glScalef(1, -1, 1);
  glTranslatef(0, -h, 0);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_MODELVIEW);
}


void drawString(const char *str, int x, int y, const double* color,double alpha,void *font){
  static const double black[3]={0,0,0};
  if (color==NULL)
    color=black;
  if (font==NULL)
    font=GLUT_BITMAP_8_BY_13;
  //glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
  glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
    glDisable(GL_TEXTURE_2D);

    glColor4d(color[0],color[1],color[2],alpha);          // set text color
    glRasterPos2i(x, y);        // place text position

    // loop all characters in the string
    while(*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    //glEnable(GL_TEXTURE_2D);
    //glEnable(GL_LIGHTING);
    glPopAttrib();
}

void enlargeRectangle(double* v,double dw,double dh){
  v[0]-=dw; v[1]-=dh;
  v[2]+=dw; v[3]-=dh;
  v[4]+=dw; v[5]+=dh;
  v[6]-=dw; v[7]+=dh;
}

void drawRectangle(double l,double t,double w,double h,const double* color){
  if (color!=NULL) glColor3dv(color); 
  glBegin(GL_QUADS);
  glVertex2f(l,t);
  glVertex2f(l+w,t);
  glVertex2f(l+w,t+h);
  glVertex2f(l,t+h);
  glEnd();  
}

  void drawTexturedRectangle(const double* v,const double* tex){
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    
    
    glTexCoord2dv(&tex[0]);
    glVertex2f(v[0],v[1]);
    
    glTexCoord2dv(&tex[2]);
    glVertex2f(v[2],v[3]);
    
    glTexCoord2dv(&tex[4]);
    glVertex2f(v[4],v[5]);
    
    glTexCoord2dv(&tex[6]);
    glVertex2f(v[6],v[7]);

    glEnd();
    glPopAttrib();
  }
  

void getRectangle(double* v,const GlPrimitiveCanvas* pc,double l,double t,double w,double h,bool centered){
  double cw=pc->getWidth();
  double ch=pc->getHeight();
  //cout<<"assume "<<cw<<","<<ch<<endl;
  if (centered){
    l-=w/2;
    t-=h/2;
  }
  v[0]=cw*l;        v[1]=ch*t;
  v[2]=cw*(l+w);    v[3]=ch*t;
  v[4]=cw*(l+w);    v[5]=ch*(t+h);
  v[6]=cw*l;        v[7]=ch*(t+h);
 }


void drawFilledRectangle(const double *v,const double *color){
  if (color!=NULL) glColor3dv(color); 
  glBegin(GL_QUADS);
  glVertex2f(v[0],v[1]);
  glVertex2f(v[2],v[3]);
  glVertex2f(v[4],v[5]);
  glVertex2f(v[6],v[7]);
  glEnd();
}


void drawBorder(double l,double t,double w,double h,double bw,double bh,double* color){
  if (color!=NULL) glColor3dv(color); 
  drawRectangle(l-bw,t-bh,w+bw,bh,color);
  drawRectangle(l,t+h,w+bw,bh,color);
  drawRectangle(l+w,t-bh,bw,h+bh,color);
  drawRectangle(l-bw,t,bw,h+bh,color);
}
void drawCenterGradientRectangle(double* v,const double* c1,const double* c2,const double* cb){
  double vc[2];
  vc[0]=(v[0]+v[2])/2;
  vc[1]=(v[1]+v[7])/2;
  glBegin(GL_TRIANGLES);
  
  glColor3dv(c1); 
  glVertex2d(v[0],v[1]);
  glVertex2d(v[2],v[3]);
  glColor3dv(c2); 
  glVertex2d(vc[0],vc[1]);
  
  glColor3dv(c1); 
  glVertex2d(v[2],v[3]);
  glVertex2d(v[4],v[5]);
  glColor3dv(c2); 
  glVertex2d(vc[0],vc[1]);
  
  glColor3dv(c1); 
  glVertex2d(v[4],v[5]);
  glVertex2d(v[6],v[7]);
  glColor3dv(c2); 
  glVertex2d(vc[0],vc[1]);

  glColor3dv(c1); 
  glVertex2d(v[6],v[7]);
  glVertex2d(v[0],v[1]);
  glColor3dv(c2); 
  glVertex2d(vc[0],vc[1]);
  glEnd();

  glColor3dv(cb); 
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBegin(GL_QUADS);
  glVertex2d(v[0],v[1]);
  glVertex2d(v[2],v[3]);
  glVertex2d(v[4],v[5]);
  glVertex2d(v[6],v[7]);
  glEnd();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void drawDiagonalGradientRectangle(const double* v,const double* c1,const double* c2,const double* cb){
  glShadeModel(GL_SMOOTH);

  glBegin(GL_TRIANGLES);  
    glColor3dv(c1); 
  glVertex2d(v[0],v[1]);
  glColor3dv(c2); 
  glVertex2d(v[2],v[3]);
  glVertex2d(v[6],v[7]); 
  glVertex2d(v[6],v[7]);
  glVertex2d(v[2],v[3]);
  glColor3dv(c1); 
  glVertex2d(v[4],v[5]);
  glEnd();
  /*glBegin(GL_QUADS);
  glColor3dv(c1); 
  glVertex2d(v[0],v[1]);
  glColor3dv(c2); 
  glVertex2d(v[2],v[3]);
  glColor3dv(c1); 
  glVertex2d(v[4],v[5]);
  glColor3dv(c2); 
  glVertex2d(v[6],v[7]);
  glEnd();*/
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3dv(cb); 
  glBegin(GL_QUADS);
  glVertex2d(v[0],v[1]);
  glVertex2d(v[2],v[3]);
  glVertex2d(v[4],v[5]);
  glVertex2d(v[6],v[7]);
  glEnd();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}





static const double arrowx[7]={-123,25,0,-32,123}; 
static const double arrowy[7]={18,18,0,76,0};

void drawArrow(double x,double y,double scale,int direction){

    switch(direction){
    case 0:
      glBegin(GL_QUADS);
      glVertex2f(x+arrowx[0]*scale,y+arrowy[0]*scale);
      glVertex2f(x+arrowx[1]*scale,y+arrowy[1]*scale);
      glVertex2f(x+arrowx[1]*scale,y-arrowy[1]*scale);
      glVertex2f(x+arrowx[0]*scale,y-arrowy[0]*scale);
      glEnd();
      glBegin(GL_TRIANGLES);
      glVertex2f(x+arrowx[2]*scale,y+arrowy[2]*scale);
      glVertex2f(x+arrowx[3]*scale,y+arrowy[3]*scale);
      glVertex2f(x+arrowx[4]*scale,y+arrowy[4]*scale);

      glVertex2f(x+arrowx[2]*scale,y+arrowy[2]*scale);
      glVertex2f(x+arrowx[3]*scale,y-arrowy[3]*scale);
      glVertex2f(x+arrowx[4]*scale,y+arrowy[4]*scale);
      glEnd();
      break;
    case 1:
      glBegin(GL_QUADS);
      glVertex2f(x-arrowy[0]*scale,y+arrowx[0]*scale);
      glVertex2f(x-arrowy[1]*scale,y+arrowx[1]*scale);
      glVertex2f(x+arrowy[1]*scale,y+arrowx[1]*scale);
      glVertex2f(x+arrowy[0]*scale,y+arrowx[0]*scale);
      glEnd();
      glBegin(GL_TRIANGLES);
      glVertex2f(x-arrowy[2]*scale,y+arrowx[2]*scale);
      glVertex2f(x-arrowy[3]*scale,y+arrowx[3]*scale);
      glVertex2f(x-arrowy[4]*scale,y+arrowx[4]*scale);

      glVertex2f(x-arrowy[2]*scale,y+arrowx[2]*scale);
      glVertex2f(x+arrowy[3]*scale,y+arrowx[3]*scale);
      glVertex2f(x-arrowy[4]*scale,y+arrowx[4]*scale);
      glEnd();
      break;
      
    case 2:
      glBegin(GL_QUADS);
      glVertex2f(x-arrowx[0]*scale,y+arrowy[0]*scale);
      glVertex2f(x-arrowx[1]*scale,y+arrowy[1]*scale);
      glVertex2f(x-arrowx[1]*scale,y-arrowy[1]*scale);
      glVertex2f(x-arrowx[0]*scale,y-arrowy[0]*scale);
      glEnd();
      glBegin(GL_TRIANGLES);
      glVertex2f(x-arrowx[2]*scale,y+arrowy[2]*scale);
      glVertex2f(x-arrowx[3]*scale,y+arrowy[3]*scale);
      glVertex2f(x-arrowx[4]*scale,y+arrowy[4]*scale);
      glVertex2f(x-arrowx[2]*scale,y+arrowy[2]*scale);
      glVertex2f(x-arrowx[3]*scale,y-arrowy[3]*scale);
      glVertex2f(x-arrowx[4]*scale,y+arrowy[4]*scale);
      glEnd();
      break;

    case 3:
      glBegin(GL_QUADS);
      glVertex2f(x-arrowy[0]*scale,y-arrowx[0]*scale);
      glVertex2f(x-arrowy[1]*scale,y-arrowx[1]*scale);
      glVertex2f(x+arrowy[1]*scale,y-arrowx[1]*scale);
      glVertex2f(x+arrowy[0]*scale,y-arrowx[0]*scale);
      glEnd();
      glBegin(GL_TRIANGLES);
      glVertex2f(x-arrowy[2]*scale,y-arrowx[2]*scale);
      glVertex2f(x-arrowy[3]*scale,y-arrowx[3]*scale);
      glVertex2f(x-arrowy[4]*scale,y-arrowx[4]*scale);

      glVertex2f(x-arrowy[2]*scale,y-arrowx[2]*scale);
      glVertex2f(x+arrowy[3]*scale,y-arrowx[3]*scale);
      glVertex2f(x-arrowy[4]*scale,y-arrowx[4]*scale);
      glEnd();
      break;
      
    }

    
    
}

 


void drawCircle(double x,double y,double r,const double* centerColor,const double* outColor){
  glBegin(GL_TRIANGLE_FAN);
  glColor3dv(centerColor);
  glVertex2f(x, y);
  glColor3dv(outColor);
  for (double d=0;d<2*M_PI;d=d+M_PI/180*5){
      glVertex2d(x+r*cos(d), y+r*sin(d));
    }
  glEnd();
}

void drawPie(double x,double y,double r,double start,double stop,const double* centerColor,const double* outColor){
  glBegin(GL_TRIANGLE_FAN);
  glColor3dv(centerColor);
  glVertex2f(x, y);
  glColor3dv(outColor);
  while (stop<start) stop+=M_PI*2;
  for (double d=start;d<stop;d=d+M_PI/180*5){
      glVertex2d(x+r*cos(d), y+r*sin(d));
    }
  glEnd();
}



};//helium

