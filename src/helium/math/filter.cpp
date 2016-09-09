/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include "filter.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <helium/core/math.h>


using namespace std;

using helium::PGFilter;
using helium::IIRFilter;
using helium::MMFilter;

void PGFilter::push(double v){
  cerr<<"pushed "<<v<<endl;
}

double PGFilter::getValue(){
  cerr<<"returning 0"<<endl;
  return 0; 
}


IIRFilter::IIRFilter(int pan,int pbn):an(pan),bn(pbn){
    a=new double[an];
    y=new double[an];
    b=new double[bn];
    x=new double[bn];
    memset(x,0,sizeof(double)*bn);
    memset(y,0,sizeof(double)*an);
    bi=0;
  }

IIRFilter::~IIRFilter(){
  delete[] a;
  delete[] b;
  delete[] y;
  delete[] x;
}

void IIRFilter::push(double v){
  x[bi%bn]=v;
  y[bi%an]=0;
  for (int i=0;i<bn;i++){
    //cout<<"b["<<i<<"]="<<b[i]<<" x["<<i<<"]="<<x[i]<<endl;
    y[bi%an]+=b[i]*x[(bn+bi-i)%bn];
  }
  for (int i=1;i<an;i++){
    //cout<<"a["<<i<<"]="<<a[i]<<" y["<<i<<"]="<<y[i]<<endl;
    y[bi%bn]-=a[i]*y[(an+bi-i)%an];
  }
  bi++;
}  

double IIRFilter::getValue(){
  return y[(an+bi-1)%an];
}

void IIRFilter::set(double *pa,double *pb){
  memcpy(a,pa,sizeof(double)*an);
  memcpy(b,pb,sizeof(double)*bn);
}


void IIRFilter::setBandPass(double fCenter,double fBandwidth,double fSampling){
  double r=1-2*M_PI*fBandwidth/(2*fSampling);
  double omega0=2*M_PI*fCenter/fSampling;
  double g=(1-r)*sqrt(1-2*r*cos(2*omega0)+r*r);
  a[0]=1;
  a[1]=-2*r*cos(omega0);
  a[2]=r*r;
  b[0]=g;
  b[1]=0;
  b[2]=-g;
  
  /*cout<<"created filter"<<endl<<" A=";
  for (int i=0;i<an;i++){
    cout<<a[i]<<" ";
  }
  cout<<endl<<" B=";
  for (int i=0;i<an;i++){
    cout<<b[i]<<" ";
  }
  cout<<endl;*/
}


MMFilter::MMFilter(int sn):n(sn),value(0){
  buffer=new double[sn];    
  i=0;
  for (int i=0;i<n;i++)
    buffer[i]=0;
}//Filter

MMFilter::~MMFilter(){
  delete[] buffer;
}

void MMFilter::push(double v){
  value-=buffer[i];
  buffer[i]=v/n;
  value+=buffer[i];
  i=(i+1)%n;
};

double MMFilter::getValue(){
  return value;
}


/*int main(){
  IIRFilter iif(3,3);
  iif.setBandPass(0.05,0.0034,1);
  iif.push(1);
  for (int i=0;i<10;i++){
    cout<<iif.getValue()<<endl;
    iif.push(0);
    
  }
  }*/
