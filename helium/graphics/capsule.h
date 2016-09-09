#ifndef _CAPSULE_
#define _CAPSULE_
#include <vector>

namespace CapsuleNS{
  static const int DATASIZE=6; //normal x, normal y,normal z,x,y,z
  static const int FACES=6;
}

class Capsule{

 private:
  static const int capped_cylinder_quality = 3;
  double* v[CapsuleNS::FACES];
  int length[CapsuleNS::FACES];
  std::vector<double>  vv[CapsuleNS::FACES];


  std::vector<double> strip;

  int triangles;
  void beginStrip(){
    strip.clear();
    triangles=0;
  }

  void pushStripVertex(double nx,double ny,double nz,
		       double x,double y,double z,int  f){
     
        
    strip.push_back(nx);
    strip.push_back(ny);
    strip.push_back(nz);
       
    strip.push_back(x);
    strip.push_back(y);
    strip.push_back(z);
    
    if (strip.size()==CapsuleNS::DATASIZE*3){
      /*std::cout<<"triangle"
	       <<" ("<<strip[3]<<","<<strip[4]<<","<<strip[5]<<") "
	       <<" ("<<strip[9]<<","<<strip[10]<<","<<strip[11]<<") "
	       <<" ("<<strip[15]<<","<<strip[16]<<","<<strip[17]<<") "
	       <<std::endl;*/
      if (triangles%2==0)
	for (int i=0;i<CapsuleNS::DATASIZE*3;i++){	
	  vv[f].push_back(strip[i]);
	}
      else{
	for (int i=0;i<CapsuleNS::DATASIZE;i++){	
	  vv[f].push_back(strip[CapsuleNS::DATASIZE+i]);
	}
	for (int i=0;i<CapsuleNS::DATASIZE;i++){	
	  vv[f].push_back(strip[i]);
	}
	for (int i=0;i<CapsuleNS::DATASIZE;i++){	
	  vv[f].push_back(strip[2*CapsuleNS::DATASIZE+i]);
	}
      }	
      triangles++;
      for (int j=0;j<CapsuleNS::DATASIZE;j++)
	strip[0+j]=strip[2*CapsuleNS::DATASIZE+j];      
      strip.erase(strip.begin(),strip.begin()+CapsuleNS::DATASIZE);
    }

  }


  

 public:

  int getNormalsAndVerteces(double** gv,int f){  
    *gv=v[f];
    return length[f];
  }

  
  
 Capsule(double l,double r):triangles(0){    
    int i,j;
    float tmp,nx,ny,nz,start_nx,start_ny,a,ca,sa;   
    const int n = capped_cylinder_quality*4;
    l *= 0.5;
    a = float(M_PI*2.0)/float(n);
    sa = (float) sin(a);
    ca = (float) cos(a);
    ny=cos(a/2); nz=sin(a/2);		    
    int f=-1;
    beginStrip();
    for (i=0; i<=n; i++) {
      switch (i*12/n) {
      case 0:
      case 1: 
      case 11:
      case 12: f=3;break; // right ear
      case 2:
      case 3:
      case 4: f=4;break; // front head up
      case 5:
      case 6:
      case 7: f=5;break; // left ear
      case 8:
      case 9:
      case 10:
      default:f=2; // rear head up
      }
    
      pushStripVertex(ny,nz,0,ny*r,nz*r,l,f);
      pushStripVertex(ny,nz,0,ny*r,nz*r,-l,f);
      // rotate ny,nz
      tmp = ca*ny - sa*nz;
      nz = sa*ny + ca*nz;
      ny = tmp;
    }
    float cah=cos(a/2); float sah=sin(a/2);
    
  // draw first cylinder cap
    start_nx = 0;
    start_ny = 1;//cos(a/2);
    for (j=0; j<(n/4); j++) {
      // get start_n2 = rotated start_n
      float start_nx2 =  ca*start_nx + sa*start_ny;
      float start_ny2 = -sa*start_nx + ca*start_ny;
    // get n=start_n and n2=start_n2
      nx = start_nx; ny = start_ny; nz = 0;
      float nx2 = start_nx2, ny2 = start_ny2, nz2 = 0;
      tmp = cah*ny - sah*nz;
      nz = sah*ny + cah*nz;
      ny = tmp;
      tmp = cah*ny2- sah*nz2;
      nz2 = sah*ny2 + cah*nz2;
      ny2 = tmp;

      beginStrip();
      for (i=0; i<=n; i++) {
	if(j<(n/12)) {
	  switch (i*12/n) {
          case 0: 
          case 1: 
          case 11:
          case 12: f=3;break; // right ear
          case 2:
          case 3:
          case 4: f=4;break; // front head up
          case 5:
          case 6:
          case 7: f=5;break; // left ear
          case 8:
          case 9:
          case 10:
          default:f=2; // rear head up
	  }
	}
	else{
	  //top head  
	  //setColor(0,0,0,1);
	  f=1;
	}

	
	pushStripVertex(ny2,nz2,nx2,ny2*r,nz2*r,l+nx2*r,f);
	pushStripVertex(ny,nz,nx,ny*r,nz*r,l+nx*r,f);
	
	// rotate n,n2
	tmp = ca*ny - sa*nz;
	nz = sa*ny + ca*nz;
	ny = tmp;
	tmp = ca*ny2- sa*nz2;
	nz2 = sa*ny2 + ca*nz2;
	ny2 = tmp;
    } 
    

    start_nx = start_nx2;
    start_ny = start_ny2;
    }
    // draw second cylinder cap
    start_nx = 0;
    start_ny = 1;
    for (j=0; j<(n/4); j++) {
    // get start_n2 = rotated start_n
    float start_nx2 = ca*start_nx - sa*start_ny;
    float start_ny2 = sa*start_nx + ca*start_ny;
    // get n=start_n and n2=start_n2
    nx = start_nx; ny = start_ny; nz = 0;
    float nx2 = start_nx2, ny2 = start_ny2, nz2 = 0;
    tmp = cah*ny - sah*nz;
    nz = sah*ny + cah*nz;
    ny = tmp;
    tmp = cah*ny2- sah*nz2;
    nz2 = sah*ny2 + cah*nz2;
    ny2 = tmp;

    beginStrip();

    for (i=0; i<=n; i++) {	
      switch (i*12/n) {
      case 0: 
      case 1: 
      case 11:
      case 12: f=3;break; // right ear
      case 2:
      case 3:
      case 4: f=4;break; // front head low
      case 5:
      case 6:
      case 7: f=5;break; // left ear
      case 8:
      case 9:
      case 10:
      default:f=2; // rear head low
      }//switch
      pushStripVertex(ny,nz,nx,ny*r,nz*r,-l+nx*r,f);
      pushStripVertex(ny2,nz2,nx2,ny2*r,nz2*r,-l+nx2*r,f);      
      // rotate n,n2
      tmp = ca*ny - sa*nz;
      nz = sa*ny + ca*nz;
      ny = tmp;
      tmp = ca*ny2- sa*nz2;
      nz2 = sa*ny2 + ca*nz2;
      ny2 = tmp;
    }
    start_nx = start_nx2;
    start_ny = start_ny2;
    }
    
    for (int i=0;i<CapsuleNS::FACES;i++){
      if (vv[i].size()==0)
	v[i]=NULL;
      else
	v[i]=new double[vv[i].size()];
      for (unsigned int j=0;j<vv[i].size();j++)
	v[i][j]=vv[i][j];    
      length[i]=(int)vv[i].size()/CapsuleNS::DATASIZE;
      vv[i].clear();
      //std::cout<<"face "<<i<<" has "<<length[i]<<" data "<<std::endl;
    }


  }//Capsule(l,r)
    

  virtual ~Capsule(){
    for (int i=0;i<CapsuleNS::FACES;i++){
      if (v[i]!=NULL) delete v[i];
    }

  }


};

#endif
