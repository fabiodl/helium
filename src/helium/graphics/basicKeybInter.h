/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef _BASICKEYBINTER_
#define _BASICKEYBINTER_

#include <helium/graphics/keybMouseInteraction.h>
#include <helium/core/basicEnum.h>
#include <helium/math/algebra.h>

namespace helium{
  class BasicKeybInter:public PrimitiveKeyboardInteraction{
    
    double* viewMatrix;
    bool& wireframe;
    double scale;
    double rotStep;
  public:
    BasicKeybInter(double* pviewMatrix,bool& pwireframe):viewMatrix(pviewMatrix),wireframe(pwireframe),scale(1),rotStep(M_PI/4){
      
    }
    void setScale(double s){
      scale=s;
    }
  void setRotStep(double s){
    rotStep=s;
  }
    
    KeyParsingState onKeyPushed(unsigned char key,int x,int y){
      static double def3 = viewMatrix[3];
      static double def7 = viewMatrix[7];
      static double def11 = viewMatrix[11];

      double m[16],n[16],rot[16];
      bool relative=false;
      bool absolute=false;
      switch (key){ //fb 20110813 return PrimitiveKeyboardInteraction::MANAGED;  added
      case '0':
	viewMatrix[3] = def3;
	viewMatrix[7] = def7;
	viewMatrix[11] = def11;
	setAsRotation(n,Axis::Y,0);
	absolute=true;
	break;
      case 'g'://fb 20141003 added isometric view
	double o[16],p[16];
	setAsRotation(o,Axis::X,M_PI/8*1);
	setAsRotation(p,Axis::Y,-M_PI/4*1);
	mul(n,o,p);

	absolute=true;
	break;
      case 'c':
	viewMatrix[11]+=scale;
	return PrimitiveKeyboardInteraction::MANAGED;
	break;
      case 'f':
	viewMatrix[11]-=scale;
	return PrimitiveKeyboardInteraction::MANAGED;
	break;    
      case 'l':
	viewMatrix[3]+=scale;
	return PrimitiveKeyboardInteraction::MANAGED;
	break;
      case 'j':
	viewMatrix[3]-=scale;
	return PrimitiveKeyboardInteraction::MANAGED;
	break;
      case 'i':
	viewMatrix[7]+=scale;
	return PrimitiveKeyboardInteraction::MANAGED;
	break;
      case 'k':
	viewMatrix[7]-=scale;
	return PrimitiveKeyboardInteraction::MANAGED;
	break;
      case 'a':
	setAsRotation(m,Axis::Y,-rotStep);
	relative=true;
	break;
      case 'd':
	setAsRotation(m,Axis::Y,+rotStep);
	relative=true;
	break;
      case 'w':
	setAsRotation(m,Axis::X,-rotStep);
	relative=true;
	break;
      case 's':
	setAsRotation(m,Axis::X,+rotStep);
	relative=true;
	break;
      case '1':
      case '2':
      case '3':
      case '4':
	setAsRotation(n,Axis::Y,-M_PI/2*(key-'1'));
	absolute=true;
	break;
      case '5':
      case '6':
	setAsRotation(n,Axis::X,M_PI/2+M_PI*(key-'5'));
	absolute=true;
      break;
      case 't':
	wireframe=(wireframe==false?true:false);
	std::cout<<"wireframe "<<wireframe<<std::endl;
	return PrimitiveKeyboardInteraction::MANAGED;
      }
      if (!relative&&!absolute) return PrimitiveKeyboardInteraction::UNMANAGED;
      memcpy(rot,viewMatrix,sizeof(double)*16);
      if (relative){
	setTranslation(rot,0,0,0);
	mul(n,m,rot);
      }
      setTranslation(n,viewMatrix[3],viewMatrix[7],viewMatrix[11]);
      memcpy(viewMatrix,n,sizeof(double)*16);
      return PrimitiveKeyboardInteraction::MANAGED;
    }
  };
  
}//helium
#endif
