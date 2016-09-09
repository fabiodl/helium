#ifndef HE_STEREOBLOB
#define HE_STEREOBLOB

#include <iostream>
#include <helium/system/idTree.h>

namespace helium{

  static const int STEREOBLOBPORT=49002;

  class BlobInfo{
  public:
    double x;
    double y;
    double p;
    BlobInfo():x(-1),y(-1),p(0){
    }
  };

  inline std::ostream& operator<<(std::ostream& o,const BlobInfo& b){
    return o<<b.x<<" "<<b.y<<" "<<b.p;
  }

  inline std::istream& operator>>(std::istream& i,BlobInfo& b){
    return i>>b.x>>b.y>>b.p;
  }

  extern IDPath RIGHTBLOB;
  extern IDPath LEFTBLOB;
}

#endif
