/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_FILTER_
#define HE_FILTER_

namespace helium{
class PGFilter{
 public:
  virtual void push(double v);
  virtual double getValue();
  virtual ~PGFilter(){}
};

class IIRFilter:public PGFilter{
  double* a;
  double* b;
  double *y;
  double *x;
  int bi;
  int an,bn;
 public:
  virtual void push(double v);  
  virtual double getValue();
  void setBandPass(double fCenter,double fBandwidth,double fSampling);
  void set(double *pa,double *pb);
  IIRFilter(int an,int bn);
  virtual ~IIRFilter();
};

class MMFilter:public PGFilter{
  double* buffer;
  int n;
  int i;
  double value;
 public:
  MMFilter(int sn);
  ~MMFilter();
  void push(double v);
  double getValue();
};
}//namespace helium
#endif
