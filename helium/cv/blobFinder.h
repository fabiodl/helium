#ifndef HE_BLOBFINDER_
#define HE_BLOBFINDER_

#include <helium/cv/cvMain.h>
#include <helium/vector/array.h>
#include <helium/vector/operations.h>
namespace helium{
class BlobFinder{
 
  double colorDir[3];
  double offset;
  Array<double> v,w;


  
public:
  
  BlobFinder(int width,int height,const double* pcolorDir,double poffset):     
    offset(poffset),
    v(height*width),
    w(height*width)

  {
    helium::assign(colorDir,pcolorDir,3);
 }


  void setTarget(const double* target,const double* avg){
    sub<3>(colorDir,target,avg);
    normalize<3>(colorDir);
    mulScalar<3>(colorDir,255);
    double middle[3];
    sum<3>(middle,target,avg);
    divScalar<3>(middle,2);
    offset=dot<3>(middle,colorDir);    
  }
  
  void getThresholdedCentroid(double &x,double &y,double &p,const IplImage* frame){
   
    x=0;
    y=0;
    p=0;
    for (int i=0;i<frame->height;i++){
      for (int j=0;j<frame->width;j++){
	double& pv=v[i*frame->width+j];//acting on v
	
	pv=std::min(255.0,std::max(0.0,
				   colorDir[0]*getPixelChannel(frame,i,j,CV::R)
				   +colorDir[1]*getPixelChannel(frame,i,j,CV::G)
				   +colorDir[2]*getPixelChannel(frame,i,j,CV::B)-offset));		

	if (pv>0.0){
	  x=x+j;
	  y=y+i;
	  p++;
	}

      }//for j
    }//for i
    if (p){
      x=x/p;
      y=y/p;  
    }
    /*if (p<frame->width*frame->height/100){
      if (threshold>1){
	threshold/=1.1;
      }
    }else if(p>frame->width*frame->height/50){
      if (threshold<255){
	threshold+=1.1;
      }
      }*/
    
    double distFactor=pow(frame->height/2,2.0)+pow(frame->width/2,2.0);
    double sum=0,nx=0,ny=0; 
    int erode=1;

      for (int i=erode;i<frame->height-erode;i++){
	for (int j=erode;j<frame->width-erode;j++){
	  int idx=i*frame->width+j;	  
	  w[idx]=exp(-6*((y-i)*(y-i)+(x-j)*(x-j))/distFactor);
	  double m=v[idx];
	  int c=0;
	  if (v[idx]!=0){
	    //std::cout<<"min of ";
	    for (int k=-erode;k<=erode;k++){
	      for (int l=-erode;l<=erode;l++){
		int eidx=(i+k)*frame->width+(j+l);	  
		if (v[eidx]>0){
		  c++;
		}
	      }
	    }
	    //std::cout<<" is "<<m<<"vs "<<v[idx]<<std::endl;
	  }
	  
	  if (c<(2*erode+1)*(2*erode+1)*0.55 ){
	    m=0;
	  }
	  v[idx]=m*w[idx];
	  sum+=v[idx];
	  nx+=v[idx]*j;
	  ny+=v[idx]*i;
	}
      }      
      //std::cout<<nx<<","<<ny<<std::endl;
      if (sum){
	x=nx/sum;
	y=ny/sum;
      }
      //std::cout<<"p "<<p<<" th "<<threshold<<" "<<x<<","<<y<<std::endl;
  }


  
  
  void draw(IplImage* out,CV::Channel channel){
    for (int i=0;i<out->height;i++){
      for (int j=0;j<out->width;j++){
	setPixelChannel(out,i,j,channel,(int)v[i*out->width+j]);
      }
    }
    
  }//draw

  
};
}

#endif
