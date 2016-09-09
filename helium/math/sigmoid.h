#ifndef HE_SIGMOID
#define HE_SIGMOID

inline double sigmoid(double x){
  return 1/(1+exp(-x));
}


inline double centeredSigmoid(double x){
  return 2/(1+exp(-x))-1;
}





#endif
