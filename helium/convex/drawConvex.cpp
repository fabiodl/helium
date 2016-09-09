/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include "drawConvex.h"
#include <helium/graphics/glUtil.h>
#include <helium/graphics/glHeader.h>
#include <helium/util/ioUtil.h>
#include <helium/math/algebra.h>
#include <helium/math/random.h>



using namespace std;
using namespace helium::convex;

void DrawConvex::fastReplaceModel(const ConvexCollisionModel* pcm){
  cm=pcm;
}

DrawConvex::DrawConvex(const ConvexCollisionModel* pcm):
  GlPrimitiveCanvas(800,600),
  cm(pcm),
  wireframe(false),
  bki(viewMatrix,wireframe){

  helium::eye(viewMatrix);  
  double wt[16];
  helium::eye(wt);
  helium::setRotation(wt,helium::Axis::X,-M_PI/2);
  helium::Array<double> minv(3),maxv(3);
  //minv.setAll(1E10); //old array
  //maxv.setAll(-1E10);
  setAll(minv ,1E10);
  setAll(maxv,-1E10);

  for (size_t i=0;i<cm->cd.size();i++){
    for (size_t j=0;j<cm->cd[i].parts.size();j++){  
      for (size_t k=0;k<cm->cd[i].parts[j].absolutePnt.size();k++){
	helium::accumulateMin(minv,cm->cd[i].parts[j].absolutePnt[k]);
	helium::accumulateMax(maxv,cm->cd[i].parts[j].absolutePnt[k]);
      }
    }
  }
  helium::Array<double> mean(3);
  helium::sum(mean,minv,maxv);
  helium::mulScalar(mean,-0.5);
  helium::setTranslation(wt,mean.data);  
  //wt[11]=-3*(maxv[2]-minv[2]);
  helium::transpose(worldMatrix,wt);
  //helium::print(worldMatrix);
  helium::sub(mean,maxv,minv);
  viewMatrix[11]=-1.3*mean[2];
  bki.setScale(0.02);
  bki.setRotStep(M_PI/8);
  addKeyboardInteraction(&bki);
  bg[0] = 1; bg[1] = 1; bg[2] = 1;
  aspect = getWidth()/getHeight();
  zoom = 1.0f;
}

void DrawConvex::face(helium::convex::Faces f) {
  double n[16];
  int fi = f;
  if(f<4) {
    setAsRotation(n,Axis::Y,-M_PI/2*(  fi   ));
  }
  else if(f<6){
    setAsRotation(n,Axis::X,M_PI/2+M_PI*(fi-4));
  }
  //isometric
  else {
    double o[16],p[16];
    setAsRotation(o,Axis::X,M_PI/8*1);
    setAsRotation(p,Axis::Y,-M_PI/4*1);
    mul(n,o,p);
  }
  setTranslation(n,viewMatrix[3],viewMatrix[7],viewMatrix[11]);
  memcpy(viewMatrix,n,sizeof(double)*16);
}

void DrawConvex::onDraw(){
  clearWindow(bg[0],bg[1],bg[2]);
  setPerspective(getWidth(),getHeight(),false,zoom,aspect);        
   
  setCamera(viewMatrix);
  //cout << "vm " << getWidth() << " " << getHeight() << " " << zoom << " " << aspect << endl;
  //cout<< viewMatrix << endl;
  drawConvex();
}

//original
static const double py[3]={0,0.05,0};
static const double my[3]={0,-0.05,0};

void DrawConvex::drawConvex(){
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix();
  glMultMatrixd(worldMatrix);

  if (wireframe){
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBegin(GL_LINES);
    for (size_t i=0;i<cm->cd.size();i++){
      for (size_t j=0;j<cm->cd[i].parts.size();j++){      
	for (size_t e=0;e<cm->cd[i].parts[j].peripheralEdges.size();e++){
	  pair<int,int> edge=cm->cd[i].parts[j].peripheralEdges[e];
	  if (cm->cd[i].parts[j].colliding){
	      glColor3f(1,
			0,
			0
			);
	    }else{
	    glColor3f(0,
		      0.6+sin((i+1)*(j+1)*2.0)/3,
		      0.6+sin((i+1)*(j+1)*3.0)/3
			);
	    }
	   
	  glVertex3dv(cm->cd[i].parts[j].absolutePnt[edge.first].data);
	  glVertex3dv(cm->cd[i].parts[j].absolutePnt[edge.second].data);
	}

	for (size_t e=0;e<cm->cd[i].parts[j].diagonals.size();e++){
	  pair<int,int> edge=cm->cd[i].parts[j].diagonals[e];
	  
	      glColor3f(1,
			1,
			1
			);
	      glVertex3dv(cm->cd[i].parts[j].absolutePnt[edge.first].data);
	      glVertex3dv(cm->cd[i].parts[j].absolutePnt[edge.second].data);
	}

      }//for j  
    }
    glEnd();
  }
  else{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBegin(GL_TRIANGLES);
    const_cast<ConvexCollisionModel*>(cm)->access.lock();
    for (size_t i=0;i<cm->cd.size();i++){
      for (size_t j=0;j<cm->cd[i].parts.size();j++){      
	for (size_t t=0;t<cm->cd[i].parts[j].obj.boxCoord.size();t++){
	  int* idx=cm->cd[i].parts[j].obj.boxCoord[t].data;
	  for (int z=0;z<3;z++){
	    if (cm->cd[i].parts[j].colliding){
	      glColor3f(1,
			0.3f*((i+1.0f)/cm->cd.size()+((idx[z]%3)+1.0f)/4),
		      0.3f*((i+1.0f)/cm->cd.size()+(t/2*2+1.0f)/cm->cd[i].parts[j].obj.boxCoord.size())				      
			);
	    }else{
	      glColor3f(0.5f*((i+1.0f)/cm->cd.size()+((idx[z]%3)+1.0f)/4),
			0.5f*((i+1.0f)/cm->cd.size()+(t/2*2+1.0f)/cm->cd[i].parts[j].obj.boxCoord.size()),
			(i+1.0f)/cm->cd.size()+sin(3.0*j)
			);
	    }
	    glVertex3dv(cm->cd[i].parts[j].absolutePnt[idx[z]].data);
	  }//triangle
	}//p
      }//j
    }//i  
    const_cast<ConvexCollisionModel*>(cm)->access.unlock();
    glEnd();
  }
  glPopMatrix();

}//onDraw


helium::PrimitiveKeyboardInteraction::KeyParsingState DrawConvex::onKeyPushed(unsigned char key,int x,int y) {
  return bki.onKeyPushed(key,x,y);
}
