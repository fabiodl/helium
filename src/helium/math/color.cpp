#include <helium/core/math.h>
#include <helium/vector/operations.h>
#include <iostream>
namespace helium{


  
  double hue2rgb(double p,double q,double t){
    //std::cout<<"h="<<t<<std::endl;
    if(t < 0) t += 1;
    if(t > 1) t -= 1;
    if(t < 1.0/6) return p + (q - p) * 6 * t;
    if(t < 1.0/2) return q;
    if(t < 2.0/3) return p + (q - p) * (2.0/3 - t) * 6;
    return p;
  }


  void hsl2rgb(int* rgb,double h,double s,double l){
    double r, g, b;

    if(s == 0){
        r = g = b = l; // achromatic
    }else{
      double q = l < 0.5 ? l * (1 + s) : l + s - l * s;
       double p = 2 * l - q;
       //std::cout<<"pq="<<p<<" "<<q<<std::endl;
       r = hue2rgb(p, q, h + 1.0/3);
       g = hue2rgb(p, q, h);
       b = hue2rgb(p, q, h - 1.0/3);
    }
    //std::cout<<"HSL"<<h<<" "<<s<<" "<<l<<std::endl;
    //std::cout<<"RGB"<<r<<" "<<g<<" "<<b<<std::endl;
    rgb[0]=round(r*255);
    rgb[1]=round(g*255);
    rgb[2]=round(b*255);
  }




  void yuv2rgb(int* rgb,double y,double u,double v){
    rgb[0]=round(255*(y+1.13983*v));
    rgb[1]=round(255*(y-0.39465*u-0.58060*v));
    rgb[2]=round(255*(y+2.03211*u));
  }

 void yCbCr2rgb(int* rgb,double y,double cb,double cr){
   static const double kb=0.114;
   static const double kr=0.299;

   rgb[0]=round(255*limit(0.0,1.0,y+2*(1-kr) *cr));
   rgb[2]=round(255*limit(0.0,1.0,y+2*(1-kb)*cb   ));
   rgb[1]=round(255*limit(0.0,1.0,(y-kr*rgb[0]-kb*rgb[2])/(1-kr-kb) ));
 }

}
